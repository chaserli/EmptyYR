#pragma once

#include <Audio.h>

#include <map>
#include <string>

// do not change! this is not a game constant, but a technical one.
// memory will not be allocated below this address, thus only values
// below are guaranteed to be indexes. -AlexB
auto const MinimumAresSample = 0x10000;

class LooseAudioCache {
public:
	static LooseAudioCache Instance;

	struct LooseAudioFile {
		LooseAudioFile()
			: Offset(-1), Size(-1), Data()
		{ }

		int Offset;
		int Size;
		AudioSampleData Data;
	};

	LooseAudioFile& GetData(const char* pFilename) {
		return this->Files[pFilename];
	}

	int GetIndex(const char* pFilename) {
		auto it = this->Files.find(pFilename);
		if(it == this->Files.end()) {
			it = this->Files.emplace(pFilename, LooseAudioFile()).first;
		}

		return reinterpret_cast<int>(it->first.c_str());
	}

private:
	std::map<std::string, LooseAudioFile> Files;
};

class AresAudioHelper {
public:
	struct FileStruct {
		int Size;
		int Offset;
		RawFileClass* File;
		bool Allocated;
	};

	static AudioSampleData* GetData(int index);

	static FileStruct GetFile(int index);

	static const char* ToLooseFile(int index) {
		if(index >= MinimumAresSample) {
			return reinterpret_cast<const char*>(index);
		}
		return nullptr;
	}

	static int ToSampleIndex(int index) {
		if(index < MinimumAresSample) {
			return index;
		}
		return -1;
	}
};
