#include <YRPP.h>
#include "WarheadTypeExt.h"

EXT_P_DECLARE(WarheadTypeClass);

void __stdcall WarheadTypeClassExt::Create(WarheadTypeClass* pThis)
{
	if(!CONTAINS(Ext_p, pThis))
	{
		ALLOC(ExtData, pData);

		pData->IsCustom     = 0;

		Ext_p[pThis] = pData;
	}
}

void __stdcall WarheadTypeClassExt::Delete(WarheadTypeClass* pThis)
{
	if(CONTAINS(Ext_p, pThis))
	{
		DEALLOC(Ext_p, pThis);
	}
}

void __stdcall WarheadTypeClassExt::Load(WarheadTypeClass* pThis, IStream* pStm)
{
	if(CONTAINS(Ext_p, pThis))
	{
		Create(pThis);

		ULONG out;
		pStm->Read(&Ext_p[pThis], sizeof(ExtData), &out);
	}
}

void __stdcall WarheadTypeClassExt::Save(WarheadTypeClass* pThis, IStream* pStm)
{
	if(CONTAINS(Ext_p, pThis))
	{
		ULONG out;
		pStm->Write(&Ext_p[pThis], sizeof(ExtData), &out);
	}
}

void __stdcall WarheadTypeClassExt::LoadFromINI(WarheadTypeClass* pThis, CCINIClass* pINI)
{
	const char * section = pThis->get_ID();
	if(!CONTAINS(Ext_p, pThis) || !pINI->GetSection(section))
	{
		return;
	}

	ExtData *pData = Ext_p[pThis];

	if(pThis->get_MindControl())
	{
		pData->MindControl_Permanent = pINI->ReadBool(section, "MindControl.Permanent", pData->MindControl_Permanent);
	}
	if(pThis->get_EMEffect()) {
		pData->EMP_Duration = pINI->ReadInteger(section, "EMP.Duration", pData->EMP_Duration);
	}
	/*
	char buffer[256];
	if(pINI->ReadString(section, "IvanBomb.Warhead", pData->Ivan_WH->get_ID(), buffer, 256) > 0)
	{
	pData->Ivan_WH           = WarheadTypeClass::FindOrAllocate(buffer);
	}
	*/
}

// 5240BD, 7 
EXPORT_FUNC(CyborgParsingMyArse)
{
	return 0x5240C4;
}

//Feature #200: EMP Warheads
//4C5824, 5
EXPORT_FUNC(EMPulseClass_Initialize1)
{
	GET(int, Duration, EDX);
	GET(TechnoClass *, curVictim, ESI);
	Duration += curVictim->get_EMPLockRemaining();
	R->set_EDX(Duration);
	return 0;
}

//4C5718, 6
EXPORT_FUNC(EMPulseClass_Initialize2)
{
	GET(int, Duration, EAX);
	GET(TechnoClass *, curVictim, ESI);
	Duration += curVictim->get_EMPLockRemaining();
	R->set_EAX(Duration);
	return 0;
}

//4C575E, 7
EXPORT_FUNC(EMPulseClass_CyborgCheck)
{
	GET(TechnoClass *, curVictim, ESI);
	TechnoTypeClass* pType = (TechnoTypeClass*)curVictim->GetType();	
	return pType->get_Cyborg_() ? 0x4C577A : 0;
}


// feature #384: Permanent MindControl Warheads + feature #200: EMP Warheads
// 46920B, 6
EXPORT_FUNC(BulletClass_Fire)
{
	GET(BulletClass *, Bullet, ESI);
	WarheadTypeClass *pThis = Bullet->get_WH();

	RET_UNLESS(CONTAINS(WarheadTypeClassExt::Ext_p, pThis));
	WarheadTypeClassExt::WarheadTypeClassData *pData = WarheadTypeClassExt::Ext_p[pThis];

	RET_UNLESS(pData->MindControl_Permanent || pData->EMP_Duration);

	TechnoClass *pTarget = (TechnoClass *)Bullet->get_Target();
	RET_UNLESS(pTarget->get_AbstractFlags() & ABSFLAGS_ISTECHNO);

	TechnoTypeClass *pType = (TechnoTypeClass *)pTarget->GetType();

	if(pTarget->IsIronCurtained()) return 0;

	//Electro@pd: get working on support for multiple callbacks in Syringe
	if(pData->EMP_Duration)
	{
		new EMPulseClass(*(Bullet->get_Target()->GetCell()->get_MapCoords()), int(pThis->get_CellSpread()), pData->EMP_Duration);
	}

	if(pData->MindControl_Permanent)
	{
		if(pType->get_ImmuneToPsionics()) return 0;
		if(pTarget->get_MindControlledBy())
		{
			pTarget->get_MindControlledBy()->get_CaptureManager()->FreeUnit(pTarget);
		}
		pTarget->SetOwningHouse(Bullet->get_Owner()->get_Owner(), 1);
		pTarget->set_MindControlledByAUnit(1);
		pTarget->QueueMission(mission_Guard, 0);

		CoordStruct coords;
		pTarget->GetCoords(&coords);
		coords.Z += pType->get_MindControlRingOffset();

		AnimClass *MCAnim = new AnimClass(RulesClass::Global()->get_PermaControlledAnimationType(), coords);
		pTarget->set_MindControlRingAnim(MCAnim);
		MCAnim->SetOwnerObject(pTarget);

		return 0x469AA4;
	}

	return 0;
}