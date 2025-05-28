#ifndef AUDIO_H
#define AUDIO_H

#include <SDL2/SDL_mixer.h>
#include <map>
#include <vector>
#include <iostream>

class AudioManager
{
public:
	void init();
	~AudioManager();
	
	void play(std::string name); // play sound
	void playm(std::string name); // play music
	void stopm(); // stop music

	std::map<std::string, Mix_Chunk*> chunks;
	std::map<std::string, Mix_Music*> music;
	
	unsigned int num_channels;
	unsigned int free_channels;
};

// global audio manager
extern AudioManager audio;

void aud_chan_finish(int channel);

#endif