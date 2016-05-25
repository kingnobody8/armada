#include "Anime.h"

Anime::Anime()
{
	this->m_bIsLooping		= false;
	this->m_bIsPlaying		= false;
	this->m_bIsPaused		= false;
	this->m_dCurrFrameTime	= 0.0L;
	this->m_dSpeed			= 1.0L;
	this->m_schCurrFrame	= 0;
	this->m_slImgID		= -1;
}
Anime::~Anime()
{
}

void Anime::Init(slong slImgID, void* pBuffer, slong slSize)
{
	if (!pBuffer || slSize == 0)
		return;

	this->m_slImgID = slImgID;
}
void Anime::Play(bool bLoop, double dSpeed)
{
	this->m_bIsLooping			= bLoop;
	this->m_bIsPaused			= false;
	this->m_bIsPlaying			= true;
	this->m_dCurrFrameTime		= 0.0L;
	this->m_dSpeed				= dSpeed;
	this->m_schCurrFrame		= 0;
	this->m_schNumLoopsPlayed	= 0;
}
void Anime::Update(Time dDelta)
{
	//Return if we aren't updating
	if (!this->m_bIsPlaying || this->m_bIsPaused)
		return;

	//Adjust the timer
	this->m_dCurrFrameTime += dDelta * this->m_dSpeed;

	//If the timer runs over, then adjust the current frame
	if (this->m_dCurrFrameTime >= m_vFrames[this->m_schCurrFrame].m_dDuration || this->m_dCurrFrameTime < 0.0L)
	{
		//Move the current frame in the direction of the speed
		this->m_schCurrFrame += posneg(this->m_dSpeed);
		this->m_dCurrFrameTime = 0.0L;

		//If the current frame runs over or under, handle based on looping
		if (this->m_schCurrFrame >= (signed int)this->m_vFrames.size())
		{
			if (this->m_bIsLooping)
				this->m_schCurrFrame = 0;
			else
			{
				this->m_schCurrFrame = this->m_vFrames.size() - 1;
				this->m_bIsPlaying = false;
			}
		}
		else if (this->m_schCurrFrame < 0)
		{
			if (this->m_bIsLooping)
				this->m_schCurrFrame = this->m_vFrames.size() - 1;
			else
			{
				this->m_schCurrFrame = 0;
				this->m_bIsPlaying = false;
			}
		}
	}
	
}
void Anime::Render(Type2<slong> pos, Type2<float> fScale, _COLOR<uchar> color, SDL_BlendMode bMode, Sprite::TRotation* rot)
{
	Type2<slong> oPos = pos - this->m_vFrames[this->m_schCurrFrame].m_tAnchorPoint;
	_RECT<slong> rect = this->m_vFrames[this->m_schCurrFrame].m_tRenderRect;
	//Sprite::GetInstance()->DrawTex(this->m_slImgID, oPos, fScale, (SDL_Rect*)&rect, rot, bMode, color);
}

double Anime::GetCurrTime(void) const
{
	double time = 0.0L;
	for (schar i = 0; i < this->m_schCurrFrame; i++)
		time += this->m_vFrames[i].m_dDuration;
	time += this->m_dCurrFrameTime;
	return time;
}
double Anime::GetTotalTime(const bool bSpeedScale) const
{
	double time = 0.0L;
	ulong size = this->m_vFrames.size();
	for (ulong i = 0; i < size; i++)
		time += this->m_vFrames[i].m_dDuration;
	if (bSpeedScale)
		time *= this->m_dSpeed;
	return time;
}
