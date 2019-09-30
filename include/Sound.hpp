#pragma once

#include<string>
#include<iostream>

#include<fmod/fmod.hpp>
#include<fmod/fmod_errors.h>


class SoundSystem
{
public:
	static bool F_PlaySound(std::string filePath, float volume);
	static bool init();
	static void destroySS();
private:
	static FMOD::System* SSystem;
	static bool FM_CALL(FMOD_RESULT result, bool& flag);
	static bool checkFlag(bool flag);
	static FMOD::ChannelGroup* chGroup;
};