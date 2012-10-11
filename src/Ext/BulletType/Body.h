#ifndef BULLETTYPE_EXT_H
#define BULLETTYPE_EXT_H

#include <CCINIClass.h>
#include <BulletTypeClass.h>

#include "../_Container.hpp"
#include "../../Utilities/Constructs.h"
#include "../../Utilities/Template.h"
#include "../../Ares.h"

#include "../../Misc/Debug.h"

#define FOUNDATION_CUSTOM	0x7F

class BulletTypeExt
{
public:
	typedef BulletTypeClass TT;

	class ExtData : public Extension<TT>
	{
	public:
		// solid
		bool SubjectToSolid;

		// firewall
		bool SubjectToFirewall;

		bool Parachuted;

		// added on 11.11.09 for #667 (part of Trenches)
		bool SubjectToTrenches; //! if false, this projectile/weapon *always* passes through to the occupants, regardless of UC.PassThrough

		// cache for the image animation's palette convert
		OptionalStruct<ConvertClass*> ImageConvert;

		Nullable<bool> AV; // Firestorm's Anti-Vehicle tag

		ExtData(TT* const OwnerObject) : Extension<TT>(OwnerObject),
			AV (),
			SubjectToSolid (false),
			SubjectToFirewall (true),
			Parachuted (false),
			SubjectToTrenches (true),
			ImageConvert ()
			{ };

		virtual ~ExtData() {
		}

		virtual void LoadFromINIFile(TT *pThis, CCINIClass *pINI);

		virtual void InvalidatePointer(void *ptr, bool bRemoved) {
		}

		ConvertClass* GetConvert();
	};

	static Container<BulletTypeExt> ExtMap;
};

#endif
