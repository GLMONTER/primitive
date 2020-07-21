#pragma once

#include<string>
#include<vector>
#include<iostream>

#include<fmod/fmod.hpp>
#include<fmod/fmod_errors.h>
#include<glm/glm.hpp>
class Sound
{
public:
	FMOD::Channel* sound;
	std::string refName;
};

class SoundSystem
{
public:
    static FMOD::System* SSystem;
    static bool stopSound(const std::string& referenceName);
	static std::vector<Sound> Sounds;
    static bool F_PlaySound(const std::string& filePath, float volume, const std::string& referenceName, glm::vec3& playPosition);
	static bool init();
	static void destroySS();
    static void set3DAttribs(glm::vec3& camPos, glm::vec3& camForward, glm::vec3& camUp);

private:
	static bool FM_CALL(FMOD_RESULT result, bool& flag);
	static bool checkFlag(bool flag);
	static FMOD::ChannelGroup* chGroup;
};
