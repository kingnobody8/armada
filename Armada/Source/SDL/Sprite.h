#pragma once
#include "../Util/Types.h"
#include <vector>
#include <string>
#include <SDL.h>
using namespace Type;

namespace Sprite
{
	//Consts
	const ulong SCREEN_WIDTH = 1920;
	const ulong SCREEN_HEIGHT = 1080;

	//Texture storage
	struct TTexture
	{
		std::string		m_strFilePath;
		SDL_Texture*	m_pTex;
		Type2<slong>	m_tSize;
		slong			m_slRefs;

		//Ctor
		TTexture()
		{
			m_strFilePath = "";
			m_pTex = NULL;
			m_slRefs = 0;
		}
	};

	//Rotation Struct
	struct TRotation
	{
		Type2<slong>		m_tCenterPoint;
		float				m_fAngle;
		bool				m_bInPlace;
		bool				m_bTexSpace;
		SDL_RendererFlip	m_eFlipType;

		//Ctor
		TRotation()
		{
			m_fAngle = 0.0f;
			m_bInPlace = m_bTexSpace = false;
			m_eFlipType = SDL_RendererFlip::SDL_FLIP_NONE;
		}
	};

	//Rendering Interface
	class CSprite
	{
	public:			//Enums
		enum ECoordinateSystem { SCREEN, CARTESIAN, NUM_COORDINATE_TYPES };

	private:		//Data
		//Texture array
		std::vector<TTexture>	m_vTextures;
		//Window
		SDL_Window*				m_pWin;
		//Renderer
		SDL_Renderer*			m_pRen;
		//Background color
		_COLOR<uchar>			m_tBackColor;
		//Screen
		bool					m_bFullScreen;
		//Coordinate System
		ECoordinateSystem		m_eCoordinateSystem;
		//Screen Scale
		Type2<sint>				m_tScreenSize;
		//Gamma
		float					m_fGamma;
		//Render Rects
		bool					m_bShowRect;
	public:			//Data


	private:		//Methods
		void			Resize(void);
		void			Rotate(const slong slID, TRotation* pRot, const SDL_Rect* pArea, const Type2<float>* fScale);
		bool			LoadTexFromFile(const slong slID, const char* _file_path);
		bool			LoadTexFromPak(const slong slID, const char* _file_name);
		bool			MakeSpecialRects(const char* name, _COLOR<uchar> clr);
	public:			//Methods
		CSprite(void);
		~CSprite(void);

		//Initialization:
		bool			Init(const char* pWindowTitle, slong x, slong y, slong w, slong h);
		bool			Poll(void);

		//Screen 
		void			SetFullScreen(const bool bFull);
		void			FullScreenToggle(void);
		inline bool		GetFullScreen(void) const { return this->m_bFullScreen; }
		Type2<float>	GetScreenScale(void);
		ECoordinateSystem GetCoordSys(void);

		//Load Textures
		slong			LoadTex(const char* _file_path, bool usePak = false);
		void			UnloadTex(slong nID);
		void			UnloadAll(void);

		//Render
		void			Primer(void);
		void			Present(void);
		void			SetBGColor(uchar red, uchar green, uchar blue, uchar alpha);
		void			SetBGColor(_COLOR<uchar> color);

		//Gamma
		inline void		SetGamma(const float fGamma) { this->m_fGamma = clamp<float>(fGamma, 0.0f, 1.0f); }
		inline float	GetGamma(void) { return this->m_fGamma; }

		//Render Rects
		inline void		SetShowRect(const bool bShowRect) { this->m_bShowRect = bShowRect; }
		inline bool		GetShowRect(void) const { return this->m_bShowRect; }

		//Draw Point
		void			DrawPoint(Type2<slong> pos, _COLOR<uchar> color = CLR::WHITE);
		void			DrawPoint(slong x, slong y, _COLOR<uchar> color = CLR::WHITE);
		//Draw Line
		void			DrawLine(Type2<slong> alpha, Type2<slong> omega, _COLOR<uchar> color = CLR::WHITE, const ulong size = 1);
		void			DrawLine(slong x1, slong y1, slong x2, slong y2, _COLOR<uchar> color = CLR::WHITE, const ulong size = 1);
		//Draw Rectangle
		void			DrawRect(_RECT<slong> rect, _COLOR<uchar> color = CLR::WHITE, bool bFill = true, ulong size = 1);
		void			DrawRect(slong x, slong y, slong w, slong h, _COLOR<uchar> color = CLR::WHITE, bool bFill = true, ulong size = 1);
		//Draw Texture
		void			DrawTex(slong slID, Type2<slong> tPos, Type2<float> fScale = Type2<float>(1.0f, 1.0f),
			SDL_Rect* pSection = NULL, TRotation * pRot = NULL, SDL_BlendMode bMode = SDL_BlendMode::SDL_BLENDMODE_NONE,
			_COLOR<uchar> tColor = CLR::WHITE);
	};
}