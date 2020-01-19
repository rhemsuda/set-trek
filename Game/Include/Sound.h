/*
File Name:		Sound.h
Description:	This file holds the definition of the wave file header type and the methods for loading a wave file and playng a wave file
Programmer:		Kyle Jensen
Date:			April 14, 2017
*/

#include "platform.h"

struct WaveHeaderType
{
	char chunkId[4];
	unsigned long chunkSize;
	char format[4];
	char subChunkId[4];
	unsigned long subChunkSize;
	unsigned short audioFormat;
	unsigned short numChannels;
	unsigned long sampleRate;
	unsigned long bytesPerSecond;
	unsigned short blockAlign;
	unsigned short bitsPerSample;
	char dataChunkId[4];
	unsigned long dataSize;
};

typedef IDirectSoundBuffer8* SoundHandle;

bool LoadWaveFile(char* fileName, IDirectSound8* directSound, SoundHandle* secondaryBuffer);
bool PlayWaveFile(SoundHandle secondaryBuffer, long volume, bool looping = false);