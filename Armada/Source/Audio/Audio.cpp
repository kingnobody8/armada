/*
Author:			Daniel Habig
Date:			3-18-14
File:			SoundSystem.cpp
Purpose:		Setup and Managment of sounds and music.
*/
#define WIN32_LEAN_AND_MEAN
#include "Audio.h"
#include <fmod.h>
#include <fmod.hpp>
#include "../Paker/Paker.h"
#include <Windows.h>
#include <assert.h>	

#define DOPPLER_SCALE			1.0f
#define	DISTANCE_FACTOR			1.0f
#define ROLLOFF_SCALE			1.0f

namespace Audio
{
	CAudio::CAudio()
	{
		//this->m_vPlaying.reserve(MAX_AUDIO_CHANNELS);
	}

	CAudio::~CAudio()
	{
	}

	bool CAudio::Start(void)
	{
		// Create the main fmod system object
		if (FMOD::System_Create(&this->m_pFMod) != FMOD_RESULT::FMOD_OK)
			return false;

		// Initialize FMOD
		if (this->m_pFMod->init(MAX_AUDIO_CHANNELS, FMOD_INIT_NORMAL, 0) != FMOD_RESULT::FMOD_OK)
			return false;

		//Setup 3D settings
		if (this->m_pFMod->set3DSettings(DOPPLER_SCALE, DISTANCE_FACTOR, ROLLOFF_SCALE) != FMOD_RESULT::FMOD_OK)
			return false;

		// Initialize the SFX channel group
		if (this->m_pFMod->createChannelGroup("SFX", &this->m_cgSfx) != FMOD_RESULT::FMOD_OK)
			return false;
		if (this->m_cgSfx->setVolume(1.0f) != FMOD_RESULT::FMOD_OK)
			return false;

		//Initialize the Channel Guids
		for (ulong i = 0; i < MAX_AUDIO_CHANNELS; ++i)
		{
			this->m_vChannelGuid[i] = -1;
			FMOD::Channel* pChannel = NULL;
			if (this->m_pFMod->getChannel(i, &pChannel) != FMOD_OK || pChannel == NULL)
				return false;
			pChannel->setUserData(&this->m_vChannelGuid[i]);
		}

		//Intialize Camera Position
		Type3<float> tPos, tVel, tFor, tUp;
		tPos = Type3<float>(0.0f, 0.0f, 0.0f);
		tVel = Type3<float>(0.0f, 0.0f, 0.0f);
		tFor = Type3<float>(0.0f, 0.0f, 1.0f);
		tUp = Type3<float>(0.0f, 1.0f, 0.0f);
		if (this->m_pFMod->set3DListenerAttributes(0, (FMOD_VECTOR*)&tPos, (FMOD_VECTOR*)&tVel, (FMOD_VECTOR*)&tFor, (FMOD_VECTOR*)&tUp) != FMOD_RESULT::FMOD_OK)
			return false;

		return true;
	}
	void CAudio::Release()
	{
		this->UnloadAll();
		this->m_pFMod->release();
		this->m_pFMod = NULL;
	}

	//Loading and Unloading
	bool CAudio::LoadAudioFromFile(const slong slID, const char* _file_path, const EAudioType eAudioType)
	{
		FMOD::Sound* pSound = NULL;
		switch (eAudioType)
		{
		case SFX_2D:
			if (this->m_pFMod->createSound(_file_path, FMOD_2D, 0, &pSound) != FMOD_RESULT::FMOD_OK)
				return false;
			break;
		case SFX_3D:
			if (this->m_pFMod->createSound(_file_path, FMOD_3D, 0, &pSound) != FMOD_RESULT::FMOD_OK)
				return false;
			break;
		case MUS_2D:
			if (this->m_pFMod->createStream(_file_path, FMOD_2D, 0, &pSound) != FMOD_RESULT::FMOD_OK)
				return false;
			break;
		case MUS_3D:
			if (this->m_pFMod->createStream(_file_path, FMOD_3D, 0, &pSound) != FMOD_RESULT::FMOD_OK)
				return false;
			break;
		}

		this->m_vAudio[slID] = TSound(_file_path, pSound, eAudioType);
		this->m_vAudio[slID].m_slRefs++;
		return true;
	}
	bool CAudio::LoadAudioFromPak(const slong slID, const char* _file_path, const EAudioType eAudioType)
	{
		char * pBuffer = NULL;
		ulong slSize = 0;
		if (!Pak::GetAsset(_file_path, &pBuffer, slSize))
			return false;

		FMOD_CREATESOUNDEXINFO info;
		ZeroMemory(&info, sizeof(info));
		info.length = slSize;

		FMOD::Sound* pSound = NULL;
		switch (eAudioType)
		{
		case SFX_2D:
		{
			FMOD_RESULT fr = this->m_pFMod->createSound(pBuffer, FMOD_2D | FMOD_OPENMEMORY, &info, &pSound);
			if ( fr != FMOD_RESULT::FMOD_OK)
				return false;
			break;
		}
		case SFX_3D:
			if (this->m_pFMod->createSound(pBuffer, FMOD_3D | FMOD_OPENMEMORY, &info, &pSound) != FMOD_RESULT::FMOD_OK)
				return false;
			break;
		case MUS_2D:
		case MUS_3D:
			break;
		//case MUS_2D:
		//	if (this->m_pFMod->createStream(pBuffer, FMOD_2D | FMOD_OPENMEMORY, &info, &pSound) != FMOD_RESULT::FMOD_OK)
		//		return false;
		//	break;
		//case MUS_3D:
		//	if (this->m_pFMod->createStream(pBuffer, FMOD_3D | FMOD_OPENMEMORY, &info, &pSound) != FMOD_RESULT::FMOD_OK)
		//		return false;
		//	break;
		}

		//Cleanup 
		delete[]pBuffer;
		pBuffer = NULL;

		this->m_vAudio[slID] = TSound(_file_path, pSound, eAudioType);
		this->m_vAudio[slID].m_slRefs++;
		return true;
	}
	slong CAudio::LoadAudio(const char* _file_path, const EAudioType eAudioType, const bool usePak)
	{
		//Make sure filename is valid
		if (!_file_path)
			return false;

		//Get all lowercase version
		std::string szFileName = "";
		size_t size = strlen(_file_path);
		for (size_t i = 0; i < size; i++)
			szFileName.push_back(tolower(_file_path[i]));

		//Make sure the texture isn't already loaded
		for (ulong i = 0; i < this->m_vAudio.size(); i++)
		{
			// compare strings without caring about upper or lowercase
			if (strcmp(szFileName.c_str(), this->m_vAudio[i].m_strFilePath.c_str()) == 0 && this->m_vAudio[i].m_eAudioType == eAudioType)	// 0 means their equal
			{
				this->m_vAudio[i].m_slRefs++;
				return i;
			}
		}

		//Look for an open slot
		slong slID = -1;
		for (ulong i = 0; i < this->m_vAudio.size(); i++)
		{
			if (this->m_vAudio[i].m_slRefs == 0)
			{
				slID = i;
				break;
			}
		}

		// if we didn't find an open spot, load it in a new one
		if (slID == -1)
		{
			slID = this->m_vAudio.size();
			this->m_vAudio.push_back(TSound());
		}
		else //Otherwise remove the texture with 0 references
		{
			// Make sure the texture has been released.
			UnloadAudio(slID);
		}

		//Load Texure into the slot
		bool success = false;
		if (usePak)
			success = this->LoadAudioFromPak(slID, szFileName.c_str(), eAudioType);
		else
			success = this->LoadAudioFromFile(slID, szFileName.c_str(), eAudioType);

		//Return slot index if successful
		if (success)
			return slID;
		else
			return -1;
	}
	void CAudio::UnloadAudio(const slong slID)
	{
		// Make sure the nID is in range.
		assert(slID > -1 && slID < (slong)this->m_vAudio.size() && "nID is out of range");

		// Remove ref.
		this->m_vAudio[slID].m_slRefs--;

		// Release the texture if it's not being used.
		if (this->m_vAudio[slID].m_slRefs <= 0)
		{
			// Do a delete and leave this spot empty
			this->m_vAudio[slID].m_pSound->release();
			this->m_vAudio[slID].m_pSound = NULL;
			this->m_vAudio[slID].m_strFilePath = "";
			this->m_vAudio[slID].m_slRefs = 0;
		}
	}
	void CAudio::UnloadAll(void)
	{
		ulong size = this->m_vAudio.size();
		for (ulong i = 0; i < size; i++)
		{
			if (this->m_vAudio[i].m_pSound == NULL)
				continue;
			// Do a delete and leave this spot empty
			this->m_vAudio[i].m_pSound->release();
			this->m_vAudio[i].m_pSound = NULL;
			this->m_vAudio[i].m_strFilePath = "";
			this->m_vAudio[i].m_slRefs = 0;
		}
	}

	//Playing
	bool CAudio::Play(const slong audioID, const bool looping,  const bool check_for_prev, const slong uniqueID, const DirectX::XMFLOAT4X4 xMatrix)
	{
		//Early Out
		if (audioID == -1)
			return false;

		// Make sure the nID is in range.
		assert(audioID > -1 && audioID < (slong)this->m_vAudio.size() && "nID is out of range");

		//Check for prev
		if (check_for_prev)
		{
			for (ulong i = 0; i < MAX_AUDIO_CHANNELS; ++i)
			{
				FMOD::Channel* pChannel = NULL;
				if (this->m_pFMod->getChannel(i, &pChannel) == FMOD_OK && pChannel != NULL)
				{
					bool isPlaying = false;
					if (pChannel->isPlaying(&isPlaying) == FMOD_OK && isPlaying)
					{
						FMOD::Sound* pSound = NULL;
						if (pChannel->getCurrentSound(&pSound) == FMOD_OK && pSound == this->m_vAudio[audioID].m_pSound)
							return false;
					}
				}
			}
		}

		bool ThreeD = false;
		FMOD::ChannelGroup* pGroup = NULL;
		switch (this->m_vAudio[audioID].m_eAudioType)
		{
		case EAudioType::SFX_2D:
			pGroup = this->m_cgSfx;
			break;
		case EAudioType::SFX_3D:
			ThreeD = true;
			pGroup = this->m_cgSfx;
			break;
		case EAudioType::MUS_2D:
		case EAudioType::MUS_3D:
		case EAudioType::VOX_2D:
		case EAudioType::VOX_3D:
			break;
		}

		TPlay play;
		play.m_slGUID = uniqueID;
		play.m_pSound = this->m_vAudio[audioID];
		if (this->m_pFMod->playSound(this->m_vAudio[audioID].m_pSound, pGroup, true, &play.m_pChannel) != FMOD_OK)
			return false;

		//If 3D, set the positions
		if (ThreeD)
		{
			Type3<float> pos, vel;
			pos = Type3<float>(xMatrix._41, xMatrix._42, xMatrix._43);
			play.m_pChannel->set3DAttributes((FMOD_VECTOR*)(&pos), (FMOD_VECTOR*)(&vel), NULL);
		}

		//Start the sound
		if (looping)
			play.m_pChannel->setMode(FMOD_LOOP_NORMAL);
		play.m_pChannel->setPaused(false);

		return true;
	}
	bool CAudio::Update(const Time dDelta)
	{
		if(this->m_pFMod->update() != FMOD_OK)
			return false;
		return true;
	}
	bool CAudio::UpdateSfx(const slong uniqueID, const Time dDelta, const DirectX::XMFLOAT4X4 xMatrix)
	{
		//Early Out
		if (uniqueID == -1)
			return false;

		//Find Currently Playing Sfx
		for (ulong i = 0; i < MAX_AUDIO_CHANNELS; ++i)
		{
			FMOD::Channel* pChannel = NULL;
			if (this->m_pFMod->getChannel(i, &pChannel) == FMOD_OK && pChannel != NULL)
			{
				bool isPlaying = false;
				if (pChannel->isPlaying(&isPlaying) == FMOD_OK && isPlaying && this->m_vChannelGuid[i] == uniqueID)
				{
					//Get prev 3D data
					Type3<float> oldPos, oldVel, newPos, newVel;
					pChannel->get3DAttributes((FMOD_VECTOR*)(&oldPos), (FMOD_VECTOR*)(&oldVel));
					//Get curr 3D data
					newPos = Type3<float>(xMatrix._41, xMatrix._42, xMatrix._43);
					newVel = newPos - oldPos;
					newVel *= float(dDelta);
					//Set the 3D data
					pChannel->get3DAttributes((FMOD_VECTOR*)(&newPos), (FMOD_VECTOR*)(&newVel), NULL);
				}
			}
		}

		return true;
	}
	bool CAudio::UpdateCam(const Time dDelta, const DirectX::XMFLOAT4X4 xMatrix)
	{
		return false;
	}

	bool CAudio::Pause(const slong uniqueID, const slong audioID)
	{
		//if (audioID != -1)
		//{
		//	// Make sure the nID is in range.
		//	assert(audioID > -1 && audioID < (slong)this->m_vAudio.size() && "nID is out of range");
		//	this->m_vAudio[audioID];
		//}
		//
		//
		//if (uniqueID != -1)
		//{
		//	ulong size = this->m_vPlaying.size();
		//	for (ulong i = 0; i < size; ++i)
		//	{
		//		if (this->m_vPlaying[i].m_slGUID == uniqueID)
		//		{
		//			bool paused = false;
		//			this->m_vPlaying[i].m_pChannel->getPaused(&paused);
		//			if (audioID == -1)
		//				this->m_vPlaying[i].m_pChannel->setPaused(!paused);
		//			else if (audioID == this->m_vPlaying[i].m_pSound.)
		//		}
		//	}
		//}
		//else
		//{
		//
		//}
		return true;
	}
	bool CAudio::Stop(const slong uniqueID, const slong audioID)
	{
		return true;
	}

	//Gets
	float CAudio::GetSfxVol(void) const
	{
		float ret = -1.0f;
		this->m_cgSfx->getVolume(&ret);
		return ret;
	}
	
	//Sets
	void CAudio::SetSfxVol(const float fVol)
	{
		this->m_cgSfx->setVolume(fVol);
	}
	
}