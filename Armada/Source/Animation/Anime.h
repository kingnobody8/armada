#pragma once
#include "../Util/Types.h"
#include "../SDL/Sprite.h"
#include <vector>

class Anime
{
private:	//Data
	struct Frame
	{
		Type2<slong>	m_tAnchorPoint;	//Offset of the image
		_RECT<slong>	m_tRenderRect;	//Rect that contains the image in the texture
		Time			m_dDuration;	//How long does the frame last
		Frame()
		{
			m_dDuration = Time();
		}
	};

	//Animation
	std::vector<Frame>		m_vFrames;
	schar					m_schCurrFrame;
	Time					m_dCurrFrameTime;

	bool					m_bIsPlaying;
	bool					m_bIsPaused;
	bool					m_bIsLooping;
	double					m_dSpeed;
	slong					m_slImgID;

	//Extra Data
	ulong					m_schNumLoopsPlayed;

public:		//Data

private:	//Methods
public:		//Methods
	Anime();
	~Anime();

	/*
	Sets the image ID
	fills out the frames using the buffer
	*/
	void			Init(slong slImgID, void* pBuffer = NULL, slong slSize = 0);
	/*
	allows the animation to play when update is called
	always starts at the first frame
	*/
	void			Play(bool bLoop = false, double dSpeed = 1.0f);
	/*
	pauses or unpauses the animation
	*/
	inline void		Pause(bool bPause = true) { this->m_bIsPaused = bPause; }
	/*
	updates the curr frame and moves to the next frame if it's time is up
	*/
	void			Update(Time dDelta);
	/*
	renders the animation at the specificed point
	*/
	void			Render(Type2<slong> pos, Type2<float> fScale = Type2<float>(1.0f, 1.0f), _COLOR<uchar> color = CLR::WHITE, SDL_BlendMode bMode = SDL_BLENDMODE_NONE, Sprite::TRotation* rot = NULL);

	//Accessors
	inline void		SetImgID	(slong slImgID) { this->m_slImgID = slImgID; }
	inline void		SetSpeed	(float fSpeed)	{ this->m_dSpeed = fSpeed; }
	//Mutators
	inline double	GetSpeed	(void) { return this->m_dSpeed; }
	inline bool		GetPlaying	(void) { return this->m_bIsPlaying; }
	inline bool		GetPaused	(void) { return this->m_bIsPaused; }
	inline bool		GetLooping	(void) { return this->m_bIsLooping; }
	inline ulong	GetLoopCount(void) { return this->m_schNumLoopsPlayed; }

	//Other
	Time			GetCurrTime	(void) const;
	Time			GetTotalTime(const bool bSpeedScale = false) const;
};

