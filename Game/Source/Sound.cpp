/*
File Name:		Sound.cpp
Description:	This file holds the functions for loading a wav file from file and playing a wav file. The only type of wav file supported
				by this project are 44.1kHz 16bit 2channel wav file formats.
Programmer:		Kyle Jensen
Date:			April 14, 2017
*/

#include "..\Include\Sound.h"


//Function: LoadWaveFile(char* fileName, IDirectSound8* directSound, SoundHandle* secondaryBuffer)
//Description: This method loads a wav file into the secondary buffer provided.
//Returns: bool : whether the wav file was loaded or not.
bool LoadWaveFile(char* fileName, IDirectSound8* directSound, SoundHandle* secondaryBuffer)
{
	FILE* file;
	WaveHeaderType waveFileHeader;
	WAVEFORMATEX waveFormat;
	DSBUFFERDESC bufferDesc;
	HRESULT result;
	IDirectSoundBuffer* tempBuffer;

	//Open the wave file in binary read mode
	if (fopen_s(&file, fileName, "rb") != 0)
		return false;

	//Read the wave file header
	unsigned int count = fread(&waveFileHeader, sizeof(waveFileHeader), 1, file);
	if (count != 1)
		return false;
	
	//Validate wave header before loading
	if ((waveFileHeader.chunkId[0] != 'R') || (waveFileHeader.chunkId[1] != 'I') ||
		(waveFileHeader.chunkId[2] != 'F') || (waveFileHeader.chunkId[3] != 'F'))
		return false;

	if ((waveFileHeader.format[0] != 'W') || (waveFileHeader.format[1] != 'A') ||
		(waveFileHeader.format[2] != 'V') || (waveFileHeader.format[3] != 'E'))
		return false;

	if ((waveFileHeader.subChunkId[0] != 'f') || (waveFileHeader.subChunkId[1] != 'm') ||
		(waveFileHeader.subChunkId[2] != 't') || (waveFileHeader.subChunkId[3] != ' '))
		return false;

	if (waveFileHeader.audioFormat != WAVE_FORMAT_PCM)
		return false;

	if (waveFileHeader.numChannels != 2)
		return false;

	if (waveFileHeader.sampleRate != 44100)
		return false;

	if (waveFileHeader.bitsPerSample != 16)
		return false;

	if ((waveFileHeader.dataChunkId[0] != 'd') || (waveFileHeader.dataChunkId[1] != 'a') ||
		(waveFileHeader.dataChunkId[2] != 't') || (waveFileHeader.dataChunkId[3] != 'a'))
		return false;

	//Set the wave format of secondary buffer.
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 2;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	//Set the buffer description of the secondary sound buffer
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME;
	bufferDesc.dwBufferBytes = waveFileHeader.dataSize;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = &waveFormat;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	//Create a temporary sound buffer with the specific buffer settings
	result = directSound->CreateSoundBuffer(&bufferDesc, &tempBuffer, NULL);
	if (FAILED(result))
		return false;

	//Test the buffer format against the direct sound 8 interface and create the secondary buffer
	result = tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&*secondaryBuffer);
	if (FAILED(result))
		return false;

	//Release the temporary buffer
	tempBuffer->Release();
	tempBuffer = NULL;

	//Move to the beginning of the wave data. (After header)
	fseek(file, sizeof(WaveHeaderType), SEEK_SET);

	//Create a temporary buffer to hold the wave file data
	unsigned char* waveData = new unsigned char[waveFileHeader.dataSize];
	if (!waveData)
		return false;

	//Read in the wave file data into the newly created buffer
	count = fread(waveData, 1, waveFileHeader.dataSize, file);
	if (count != waveFileHeader.dataSize)
		return false;

	if (fclose(file) != 0)
	{
		return false;
	}

	// Lock the secondary buffer to write wave data into it.
	unsigned char* bufferPtr;
	unsigned long bufferSize;
	result = (*secondaryBuffer)->Lock(0, waveFileHeader.dataSize, (void**)&bufferPtr, (DWORD*)&bufferSize, NULL, 0, 0);
	if (FAILED(result))
		return false;

	//Copy the wave data into the buffer
	memcpy(bufferPtr, waveData, waveFileHeader.dataSize);

	//Unlock the secondary buffer after the data has been written to it
	result = (*secondaryBuffer)->Unlock((void*)bufferPtr, bufferSize, NULL, 0);
	if (FAILED(result))
		return false;

	//Release the wave data since it was copied into the secondary buffer
	delete[] waveData;
	waveData = 0;

	return true;
}


//Function: PlayWaveFile(SoundHandle secondaryBuffer, long volume, bool looping)
//Description: This method sets the position of the secondary buffer to the start of the song, sets the volume, and plays.
//Returns: bool : whether the wav file was loaded or not.
bool PlayWaveFile(SoundHandle secondaryBuffer, long volume, bool looping)
{
	HRESULT result;

	result = secondaryBuffer->SetCurrentPosition(0);
	if (FAILED(result))
		return false;

	result = secondaryBuffer->SetVolume(volume);
	if (FAILED(result))
		return false;

	//If we want to loop, set the DSPPLAY_LOOPING flag
	DWORD flag = (looping) ? DSBPLAY_LOOPING : 0;
	result = secondaryBuffer->Play(0, 0, flag);
	if (FAILED(result))
		return false;

	return true;
}

