
#ifndef APPLE
 #include<Sound.hpp>
#include<cassert>

#ifdef _WIN32
#include<combaseapi.h>
#endif

//error checking is flawed because with multiple calls of FM_CALL the flag may be false for one call then the next be true and return true.

//actual definition of static member
FMOD::System* SoundSystem::SSystem = nullptr;
FMOD::ChannelGroup* SoundSystem::chGroup = nullptr;
//define for static vector in header
std::vector<Sound> SoundSystem::Sounds;
void SoundSystem::deleteAllSounds()
{
    for(Sound s : SoundSystem::Sounds)
    {
            s.sound->stop();
            s.actualSound->release();
    }
    SoundSystem::Sounds.clear();
}
bool SoundSystem::deleteSound(const std::string& referenceName)
{
    for(std::vector<Sound>::iterator s = SoundSystem::Sounds.begin(); s != SoundSystem::Sounds.end(); s++)
    {
        if (s->refName == referenceName)
        {
            s->sound->stop();
            s->actualSound->release();
            SoundSystem::Sounds.erase(s);
            return true;
        }
    }
    return false;
}
bool SoundSystem::stopSound(const std::string& referenceName)
{
	for (Sound s : SoundSystem::Sounds)
	{
		if (s.refName == referenceName)
		{
			s.sound->stop();
			return true;
		}
	}
	return false;
}

//Actually play a sound using FMOD
bool SoundSystem::F_PlaySound(const std::string& filePath, float volume, const std::string& referenceName, glm::vec3& playPosition)
{
	bool flag = false;

	//create the sound from the file
	FMOD::Sound* s = nullptr;
    FM_CALL(SoundSystem::SSystem->createSound(filePath.c_str(), FMOD_3D, nullptr, &s), flag);

	//make a channel for the sound to play on and push it into the main ChannelGroup
	FMOD::Channel* ch = nullptr;
	ch->setChannelGroup(SoundSystem::chGroup);
    FMOD_VECTOR playPositionTemp;
    playPositionTemp.x = playPosition.x;
    playPositionTemp.y = playPosition.y;
    playPositionTemp.z = playPosition.z;

    ch->set3DAttributes(&playPositionTemp, nullptr);
	//actually play the channel from the sound group
	FM_CALL(SoundSystem::SSystem->playSound(s, SoundSystem::chGroup, false, &ch), flag);
	//set the channel volume from the incoming volume var
	ch->setVolume(volume);

	Sound tempSound;
	tempSound.refName = referenceName;
	tempSound.sound = ch;
	tempSound.actualSound = s;
	Sounds.push_back(tempSound);

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

void SoundSystem::set3DAttribs(glm::vec3& camPos, glm::vec3& camForward, glm::vec3& camUp)
{
    //temp variables for listener vectors;
    FMOD_VECTOR camPosTemp;
    FMOD_VECTOR camForwardTemp;
    FMOD_VECTOR camUpTemp;

    //converting glm::vec3 to FMOD_VECTOR for FMOD
    camPosTemp.x = camPos.x;
    camPosTemp.y = camPos.y;
    camPosTemp.z = camPos.z;

    camForwardTemp.x = camForward.x;
    camForwardTemp.y = camForward.y;
    camForwardTemp.z = camForward.z;

    camUpTemp.x = camUp.x;
    camUpTemp.y = camUp.y;
    camUpTemp.z = camUp.z;

    SoundSystem::SSystem->set3DListenerAttributes(0, &camPosTemp, NULL, &camForwardTemp, &camUpTemp);
}

//just a bool checker that returns true or false based on the bools state
bool SoundSystem::checkFlag(bool flag)
{
	if (flag)
		return true;
	else
		return false;
}
#endif APPLE