#include<Sound.hpp>
#include<cassert>

#ifdef _WIN32
#include<combaseapi.h>
#endif

//error checking is flawed because with multiple calls of FM_CALL the flag may be false for one call then the next be true and return true.

//actual definition of static member
FMOD::System* SoundSystem::SSystem = nullptr;
FMOD::ChannelGroup* SoundSystem::chGroup = nullptr;

//Actually play a sound using FMOD
bool SoundSystem::F_PlaySound(std::string filePath, float volume)
{
	bool flag = false;

	//create the sound from the file
	FMOD::Sound* s = nullptr;
	FM_CALL(SoundSystem::SSystem->createSound(filePath.c_str(), FMOD_DEFAULT, nullptr, &s), flag);

	//make a channel for the sound to play on and push it into the main ChannelGroup
	FMOD::Channel* ch = nullptr;
	ch->setChannelGroup(SoundSystem::chGroup);

	//actually play the channel from the sound group
	FM_CALL(SoundSystem::SSystem->playSound(s, SoundSystem::chGroup, false, &ch), flag);
	
	//set the channel volume from the incoming volume var
	ch->setVolume(volume);
	
	return checkFlag(flag);
}

//initializes FMOD
bool SoundSystem::init()
{
	bool flag = false;
#ifdef _WIN32

	assert(CoInitialize(nullptr) == S_OK);
	
#endif

	//create the FMOD system/instance
	FM_CALL(FMOD::System_Create(&SoundSystem::SSystem), flag);

	//initialize the FMOD system
	FM_CALL(SoundSystem::SSystem->init(512, FMOD_INIT_NORMAL, nullptr), flag);

	//create the ChannelGroup used to store the sound channels
	FM_CALL(SoundSystem::SSystem->createChannelGroup("main", &SoundSystem::chGroup), flag);

	return checkFlag(flag);
}

void SoundSystem::destroySS()
{
#ifdef _WIN32
	CoUninitialize();
#endif

	//deinit FMOD
	SoundSystem::SSystem->close();
	//release all of the memory FMOD used
	SoundSystem::SSystem->release();
}

//basically a function that checks FMOD related functions for errors and reports them
bool SoundSystem::FM_CALL(FMOD_RESULT result, bool& flag)
{
	if (result != FMOD_OK)
	{
		std::cerr << "FMOD Error : " << FMOD_ErrorString(result) << std::endl;
		flag = false;
		return false;
	}
	flag = true;
	return true;
}

//just a bool checker that returns true or false based on the bools state
bool SoundSystem::checkFlag(bool flag)
{
	if (flag)
		return true;
	else
		return false;
}
