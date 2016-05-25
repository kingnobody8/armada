/*
Author:			Daniel Habig
Date:			3-18-14
File:			SoundSystem.h
Purpose:		Setup and Managment of sounds and music.
*/
#pragma once
#include "../Util/Types.h"
#include <DirectXMath.h>
#include <vector>
using namespace Type;

const DirectX::XMFLOAT4X4 kxIdentity = DirectX::XMFLOAT4X4(1.0f,0.0f,0.0f,0.0f, 0.0f,1.0f,0.0f,0.0f, 0.0f,0.0f,1.0f,0.0f, 0.0f,0.0f,0.0f,1.0f);
#define MAX_AUDIO_CHANNELS		48

//Forward Declaration
namespace FMOD
{
	class System;
	class Sound;
	class Channel;
	class ChannelGroup;
};

namespace Audio
{
	enum EAudioType { SFX_2D, SFX_3D, MUS_2D, MUS_3D, VOX_2D, VOX_3D, NUM_AUDIO_TYPES };

	//Storage structure for sounds
	struct TSound
	{
		std::string		m_strFilePath;
		FMOD::Sound*	m_pSound;
		EAudioType		m_eAudioType;
		slong			m_slRefs;
		TSound()
		{
			m_pSound = NULL;
			m_eAudioType = NUM_AUDIO_TYPES;
			m_slRefs = 0;
		}
		TSound(const char* strFilePath, FMOD::Sound* pSound, EAudioType eAudioType)
		{
			m_strFilePath = strFilePath;
			m_pSound = pSound;
			m_eAudioType = eAudioType;
			m_slRefs = 0;
		}
	};
	struct TPlay
	{
		slong			m_slGUID;		//Unique ID
		TSound			m_pSound;		//What Sound we are playing
		FMOD::Channel*	m_pChannel;		//Our Channel
		TPlay()
		{
			m_slGUID = -1;
			m_pChannel = NULL;
		}
	};

	class CAudio
	{
	public:		//enum
	private:	//Data
		FMOD::System*				m_pFMod;								//Pointer to fmod system
		FMOD::ChannelGroup*			m_cgSfx;								//SFX channel group
		std::vector<TSound>			m_vAudio;								//Vector of sounds
		slong						m_vChannelGuid[MAX_AUDIO_CHANNELS];		//Vector of guids for each channel
		//std::vector<TPlay>			m_vPlaying;								//Vector of currently playing sounds

	private:	//Methods
		bool	LoadAudioFromFile(const slong slID, const char* _file_path, const EAudioType eAudioType);
		bool	LoadAudioFromPak(const slong slID, const char* _file_path, const EAudioType eAudioType);
	public:		//Methods
		CAudio(void);
		~CAudio(void);

		//Initialization
		bool Start(void);
		void Release();

		//Loading
		slong		LoadAudio(const char* _file_path, const EAudioType eAudioType = EAudioType::SFX_2D, const bool usePak = false);
		void		UnloadAudio(const slong slID);
		void		UnloadAll(void);

		bool		Play(const slong audioID, const bool looping = false, const bool check_for_prev = false, const slong uniqueID = -1, const DirectX::XMFLOAT4X4 xMatrix = kxIdentity);
		bool		Update(const Time dDelta);
		bool		UpdateSfx(const slong uniqueID, const Time dDelta, const DirectX::XMFLOAT4X4 xMatrix = kxIdentity);
		bool		UpdateCam(const Time dDelta, const DirectX::XMFLOAT4X4 xMatrix = kxIdentity);
		bool		Pause(const slong uniqueID = -1, const slong audioID = -1);
		bool		Stop(const slong uniqueID = -1, const slong audioID = -1);

		//Gets
		float		GetSfxVol(void) const;
		//Sets
		void		SetSfxVol(const float fVol);
	};
}