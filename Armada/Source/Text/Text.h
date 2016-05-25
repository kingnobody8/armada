#pragma once
#include <string>
#include <vector>
#include <list>
#include "Rune.h"
#include "Word.h"
#include "../Util/Types.h"
#include "../SDL/Sprite.h"
using namespace Type;

namespace Text
{
#define KERN	0.1f
#define SPACE	0.5f
#define WORD_QUEUE 32

	enum EFontAlignment { F_LEFT, F_RIGHT, F_CENTER };

	struct TTextPair
	{
		std::vector<Word> lines;
		std::string str;
	};

	class CText
	{
	private:	//Data
		std::vector<Rune>		m_vRunes;
		std::list<TTextPair>	m_vTextQueue;
		Sprite::CSprite*		m_pSprite;

	private:	//Methods
		Word				GetWord(const char* szText, bool bSpace, slong slCounter);
		std::vector<Word>	ParseStringIntoWords	(const char* szText);
		std::vector<Word>	ParseWordsIntoLines		(std::vector<Word> tWords, slong slWidth, float fScale);
		unsigned int		GetWordPixelLength		(const char* word);
		void				AddToQueue				(std::vector<Word> vWords, std::string nStr);
		void				WriteToScreen(std::vector<Word> tLines, _RECT<slong> tBox, float fScale, _COLOR<uchar> clr, EFontAlignment eAlign, SDL_BlendMode bMode);
		void				WriteLeftAlign(std::vector<Word> tLines, _RECT<slong> tBox, float fScale, _COLOR<uchar> clr, SDL_BlendMode bMode);
		void				WriteCenterAlign(std::vector<Word> tLines, _RECT<slong> tBox, float fScale, _COLOR<uchar> clr, SDL_BlendMode bMode);
		void				WriteRightAlign(std::vector<Word> tLines, _RECT<slong> tBox, float fScale, _COLOR<uchar> clr, SDL_BlendMode bMode);

	public:		//Methods
		CText(void);						// ctor
		~CText(void);						// dtor

		void	Start(Sprite::CSprite* pSprite);				// load resources
		void	Stop(void);										// unload resources
		void	LoadBinData(const char* fontFile, const char* imgFile);
		void	Write(	const char* strText, _RECT<slong> tBox, _COLOR<uchar> clr = CLR::WHITE, float fScale = 1.0f,
						bool format = true, EFontAlignment eAlign = F_LEFT, SDL_BlendMode bMode = SDL_BLENDMODE_BLEND);
		Type2<ulong>	GetSize(const char* strText, float fScale, bool bWithFinalKern = false);

		//void	Draw(const char* szText, _RECT box, bool formated = true, float fScale = 1.0f, _COLOR color = c_WHITE,
		//	FONT_TYPE font = F_MENU, FONT_ALIGN align = F_LEFT, SDL_BlendMode bMode = SDL_BLENDMODE_BLEND);
		//
		//void	DrawTest(const char* szText, _RECT box, bool formated = true, float fScale = 1.0f, _COLOR color = c_WHITE,
		//	FONT_TYPE font = F_MENU, FONT_ALIGN align = F_LEFT, SDL_BlendMode bMode = SDL_BLENDMODE_BLEND);
	};
}