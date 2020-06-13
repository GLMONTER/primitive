#pragma once

#include<string>
#include<vector>
#include<iostream>

#include<fmod/fmod.hpp>
#include<fmod/fmod_errors.h>

class Sound
{
public:
	FMOD::Channel* sound;
	std::string refName;
};

class SoundSystem
{
public:
	static bool stopSound(std::string referenceName);
	static std::vector<Sound> Sounds;
	static bool F_PlaySound(std::string filePath, float volume, std::string referenceName);
	static bool init();
	static void destroySS();
private:
	static FMOD::System* SSystem;
	static bool FM_CALL(FMOD_RESULT result, bool& flag);
	static bool checkFlag(bool flag);
	static FMOD::ChannelGroup* chGroup;
};