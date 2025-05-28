#include "audio.h"

AudioManager audio;

#define DEF_CHANNEL_NUM 10

#define MUSIC_DIRECTORY "data/music/"
#define SOUND_DIRECTORY "data/audio/"

void AudioManager::init()
{
	// filenames
	#define N_FN_SOUNDS 4
	const char* fn_sounds[] = {
		"push.wav", "explode.wav", "artillery.wav", "furnace.wav"
	};
	
	#define N_FN_MUSIC 1
	const char* fn_music[] = {
		"menu.wav"
	};
	
	// initialize SDL Mixer
	Mix_Init(MIX_INIT_MID | MIX_INIT_MP3);
	
	Mix_OpenAudioDevice(
		48000, AUDIO_S16SYS,
		2, 2048, NULL, 0
	);
	
	num_channels = DEF_CHANNEL_NUM;
	free_channels = DEF_CHANNEL_NUM;
	
	Mix_ChannelFinished(aud_chan_finish); // callback when channel finished

	// load sound files
	for(int i=0;i<N_FN_SOUNDS;i++)
	{
		std::string fn = SOUND_DIRECTORY;
		fn += fn_sounds[i];
		Mix_Chunk* chnk = Mix_LoadWAV(fn.c_str());
		chunks[std::string(fn_sounds[i])] = chnk;
	}
	
	// load music
	for(int i=0;i<N_FN_MUSIC;i++)
	{
		std::string fn = MUSIC_DIRECTORY;
		fn += fn_music[i];
		Mix_Music* mus = Mix_LoadMUS(fn.c_str());
		music[std::string(fn_music[i])] = mus;
	}
	
}

AudioManager::~AudioManager()
{
	// free chunks
	for(auto const& x : chunks)
		if(x.second != nullptr)
			Mix_FreeChunk(x.second);
	// free music
	for(auto const& x : music)
		if(x.second != nullptr)
			Mix_FreeMusic(x.second);

	Mix_CloseAudio();	
	Mix_Quit();
}

void AudioManager::play(std::string name)
{
	// safety padding because idk where music is played
	if(free_channels >= 2)
	{
		free_channels --;
	} else {
		num_channels ++;
		Mix_AllocateChannels(num_channels);
	}
	Mix_PlayChannel(-1, chunks.at(name), 0);
}

void AudioManager::playm(std::string name)
{
	Mix_PlayMusic(music.at(name), 0);
}

void AudioManager::stopm()
{
	Mix_HaltMusic();
}

void aud_chan_finish(int channel)
{
	if(audio.num_channels > DEF_CHANNEL_NUM) // if already exceeds default
	{
		audio.num_channels --;
		Mix_AllocateChannels(audio.num_channels); // remove old channel
	} else {
		audio.free_channels ++; // otherwise keep default
	}
}