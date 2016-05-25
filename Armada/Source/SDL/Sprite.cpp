#include "Sprite.h"
#include <SDL_image.h>
#include <SDL_syswm.h>
#include "../Paker/Paker.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <assert.h>	

namespace Sprite
{
	CSprite::CSprite(void)
	{
		this->m_bFullScreen = false;
		this->m_eCoordinateSystem = CARTESIAN;
		this->m_pRen = NULL;
		this->m_pWin = NULL;
		this->m_tBackColor = CLR::SLATE;
		this->m_fGamma = 0.5f;
		this->m_bShowRect = false;
	}
	CSprite::~CSprite(void)
	{
		//Cleanup
		UnloadAll();
	}

	/*------------------------------------------------------------------*/
	/*					Initialization									*/
	/*------------------------------------------------------------------*/
	// - Init SDL
	// - Create Window
	// - Create Renderer
	// - Init Sprite
	bool CSprite::Init(const char* winTitle, slong x, slong y, slong w, slong h)
	{
		//Initialize SDL
		if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
		{
			MessageBox(NULL, L"SDL ERROR", L"SDL_Init", MB_OK);
			return false;
		}

		//Create Window
		this->m_pWin = SDL_CreateWindow(winTitle, x, y, w, h, SDL_WINDOW_RESIZABLE);
		if (this->m_pWin == NULL)
		{
			const char* c = SDL_GetError();
			MessageBox(NULL, L"SDL ERROR", L"SDL_CreateWindow", MB_OK);
			return false;
		}
		this->m_tScreenSize = Type2<sint>(w, h);

		//Bind Renderer to Window
		this->m_pRen = SDL_CreateRenderer(this->m_pWin, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		if (this->m_pRen == nullptr)
		{
			MessageBox(NULL, L"SDL ERROR", L"SDL_CreateRenderer", MB_OK);
			return false;
		}

		//Make White Rect
		if (!this->MakeSpecialRects("white.sdl", _COLOR<uchar>(255, 255, 255, 255)))
			return false;
		//Make Black Rect
		if (!this->MakeSpecialRects("black.sdl", _COLOR<uchar>(0, 0, 0, 255)))
			return false;

		return true;

	}
	bool CSprite::MakeSpecialRects(const char* name, _COLOR<uchar> clr)
	{
		SDL_Surface *surface = NULL;
		ulong rmask, gmask, bmask, amask;

		/* SDL interprets each pixel as a 32-bit number, so our masks must depend
		on the endianness (byte order) of the machine */
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		rmask = 0xff000000;
		gmask = 0x00ff0000;
		bmask = 0x0000ff00;
		amask = 0x000000ff;
#else
		rmask = 0x000000ff;
		gmask = 0x0000ff00;
		bmask = 0x00ff0000;
		amask = 0xff000000;
#endif

		surface = SDL_CreateRGBSurface(0, 1, 1, 32, rmask, gmask, bmask, amask);
		if (surface == NULL)
		{
			MessageBox(NULL, L"SDL ERROR", L"SDL_CreateRGBSurface", MB_OK);
			return false;
		}

		uchar* vec = (uchar*)(surface->pixels);
		for (ulong i = 0; i < 4; i++)
			*(vec + i) = clr.ary[i];

		//Make the texture
		TTexture texture;
		texture.m_strFilePath = name;

		//Set width and height
		texture.m_tSize = Type2<slong>(surface->w, surface->h);

		//Load the texture from the surface
		texture.m_pTex = SDL_CreateTextureFromSurface(this->m_pRen, surface);

		//If the texture loading failed return -1
		if (texture.m_pTex == NULL)
		{
			const char* c = SDL_GetError();
			MessageBox(NULL, L"SDL ERROR", L"IMG_LoadTexture", MB_OK);
			return false;
		}

		//Add ref
		texture.m_slRefs++;

		//Free the surface
		SDL_FreeSurface(surface);

		//Push texture onto list
		this->m_vTextures.push_back(texture);

		return true;
	}
	bool CSprite::Poll(void)
	{
		SDL_Event mEvent;
		while (SDL_PollEvent(&mEvent))
		{
			switch (mEvent.type)
			{
			case SDL_QUIT:{
							  return false;
							  break; }
			case SDL_WINDOWEVENT: {
									  this->Resize();
									  break; }
			}
		}
		return true;
	}


	/*------------------------------------------------------------------*/
	/*					Loading & Unloading								*/
	/*------------------------------------------------------------------*/
	bool CSprite::LoadTexFromFile(const slong slID, const char* _file_path)
	{
		//Easy reference to the texture
		TTexture& texture = this->m_vTextures[slID];

		// Copy the filename of the texture.
		texture.m_strFilePath = _file_path;

		//Load a surface to get width and height
		SDL_Surface* surface = IMG_Load(_file_path);
		if (surface == nullptr)
		{
			const char* c = SDL_GetError();
			MessageBox(NULL, L"SDL ERROR", L"IMG_Load", MB_OK);
			return false;
		}

		//Set width and height
		texture.m_tSize = Type2<slong>(surface->w, surface->h);

		//Load the texture from the file
		texture.m_pTex = IMG_LoadTexture(this->m_pRen, _file_path);

		//If the texture loading failed return -1
		if (texture.m_pTex == NULL)
		{
			const char* c = SDL_GetError();
			MessageBox(NULL, L"SDL ERROR", L"IMG_LoadTexture", MB_OK);
			return false;
		}

		//Add ref
		texture.m_slRefs++;

		//Free the surface
		SDL_FreeSurface(surface);

		//Return Success
		return true;
	}
	bool CSprite::LoadTexFromPak(slong slID, const char* _file_name)
	{
		//Easy reference to the texture
		TTexture& texture = m_vTextures[slID];

		// Copy the filename of the texture.
		texture.m_strFilePath = _file_name;

		//Get the image
		char* file_buffer = NULL;
		ulong file_size = 0;
		Pak::GetAsset(_file_name, &file_buffer, file_size);

		//Load WOPS
		SDL_RWops* wops = nullptr;
		wops = SDL_RWFromMem(file_buffer, file_size);

		//Load a surface to get width and height
		SDL_Surface* surface = IMG_Load_RW(wops, 0);
		if (surface == nullptr)
		{
			//SDL_Log(SDL_GetError());
			const char* c = SDL_GetError();
			MessageBox(NULL, L"SDL ERROR", L"IMG_Load_RW", MB_OK);
			return false;
		}
		//Set width and height
		texture.m_tSize = Type2<slong>(surface->w, surface->h);

		//Load the texture from the file
		texture.m_pTex = SDL_CreateTextureFromSurface(this->m_pRen, surface);//IMG_LoadTexture_RW(ren, wops, 1 );

		if (texture.m_pTex == nullptr)
		{
			//SDL_Log(SDL_GetError());
			const char* c = SDL_GetError();
			MessageBox(NULL, L"SDL ERROR", L"SDL_CreateTextureFromSurface", MB_OK);
			return false;
		}

		//Add ref
		texture.m_slRefs++;

		//Free the surface
		SDL_FreeSurface(surface);

		//Free the buffer
		delete[] file_buffer;

		return true;
	}
	slong CSprite::LoadTex(const char* _file_path, bool usePak)
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
		for (ulong i = 0; i < this->m_vTextures.size(); i++)
		{
			// compare strings without caring about upper or lowercase
			if (strcmp(szFileName.c_str(), this->m_vTextures[i].m_strFilePath.c_str()) == 0)	// 0 means their equal
			{
				this->m_vTextures[i].m_slRefs++;
				return i;
			}
		}


		//Look for an open slot
		slong slID = -1;
		for (ulong i = 0; i < this->m_vTextures.size(); i++)
		{
			if (this->m_vTextures[i].m_slRefs == 0)
			{
				slID = i;
				break;
			}
		}

		// if we didn't find an open spot, load it in a new one
		if (slID == -1)
		{
			slID = this->m_vTextures.size();
			this->m_vTextures.push_back(TTexture());
		}
		else //Otherwise remove the texture with 0 references
		{
			// Make sure the texture has been released.
			UnloadTex(slID);
		}

		//Load Texure into the slot
		bool success = false;
		if (usePak)
			success = this->LoadTexFromPak(slID, _file_path);
		else
			success = this->LoadTexFromFile(slID, _file_path);

		//Return slot index if successful
		if (success)
			return slID;
		else
			return -1;
	}
	void CSprite::UnloadTex(slong nID)
	{
		// Make sure the nID is in range.
		assert(nID > -1 && nID < (slong)this->m_vTextures.size() && "nID is out of range");

		// Remove ref.
		this->m_vTextures[nID].m_slRefs--;

		// Release the texture if it's not being used.
		if (this->m_vTextures[nID].m_slRefs <= 0)
		{
			// Do a delete and leave this spot empty
			SDL_DestroyTexture(this->m_vTextures[nID].m_pTex);
			this->m_vTextures[nID].m_pTex = nullptr;
			this->m_vTextures[nID].m_strFilePath = "";
			this->m_vTextures[nID].m_slRefs = 0;
		}
	}
	void CSprite::UnloadAll(void)
	{
		ulong size = this->m_vTextures.size();
		for (ulong i = 0; i < size; i++)
		{
			// Do a delete and leave this spot empty
			SDL_DestroyTexture(this->m_vTextures[i].m_pTex);
			this->m_vTextures[i].m_pTex = nullptr;
			this->m_vTextures[i].m_strFilePath = "";
			this->m_vTextures[i].m_slRefs = 0;
		}
	}


	/*------------------------------------------------------------------*/
	/*					Rendering										*/
	/*------------------------------------------------------------------*/
	void CSprite::Primer(void)
	{
		_COLOR<uchar> fin = this->m_tBackColor;

		SDL_SetRenderDrawColor(this->m_pRen, fin.r, fin.g, fin.b, 255);
		SDL_RenderClear(this->m_pRen);
	}
	void CSprite::Present(void)
	{
		//Perform gamma augmentation
		if (this->m_fGamma > 0.5f)	//Lighten
		{
			slong tex = this->LoadTex("white.sdl");
			uchar alpha = uchar(clamp<ulong>(ulong((this->m_fGamma * 2.0f - 1.0f) * 0.9f * 255.0f), 0, 255));
			this->DrawTex(tex, Type2<slong>(0, 0), Type2<float>(float(SCREEN_WIDTH), float(SCREEN_HEIGHT)), NULL, NULL, SDL_BLENDMODE_ADD, _COLOR<uchar>(255, 255, 255, alpha));
		}
		else if (this->m_fGamma < 0.5f) //Darken
		{
			slong tex = this->LoadTex("black.sdl");
			uchar alpha = uchar(clamp<ulong>(ulong(((1.0f - this->m_fGamma) * 2.0f - 1.0f) * 0.9f * 255.0f), 0, 255));
			this->DrawTex(tex, Type2<slong>(0, 0), Type2<float>(float(SCREEN_WIDTH), float(SCREEN_HEIGHT)), NULL, NULL, SDL_BLENDMODE_BLEND, _COLOR<uchar>(255, 255, 255, alpha));
		}

		SDL_RenderPresent(this->m_pRen);
	}
	void CSprite::SetBGColor(uchar red, uchar green, uchar blue, uchar alpha)
	{
		this->m_tBackColor = _COLOR<uchar>(red, green, blue, alpha);
	}
	void CSprite::SetBGColor(_COLOR<uchar> color)
	{
		this->m_tBackColor = color;
	}
	void CSprite::Resize(void)
	{
		Type2<float> scale = GetScreenScale();
		SDL_RenderSetScale(this->m_pRen, scale.x, scale.y);
	}
	Type2<float> CSprite::GetScreenScale(void)
	{
		Type2<float> real = Type2<float>((float)SCREEN_WIDTH, (float)SCREEN_HEIGHT);
		Type2<sint>	curr;
		SDL_GetWindowSize(this->m_pWin, &curr.x, &curr.y);
		Type2<float> scale = Type2<float>(curr.x / real.x, curr.y / real.y);
		return scale;
	}
	void CSprite::SetFullScreen(const bool bFull)
	{
		//Invert bool
		this->m_bFullScreen = bFull;

		//Fullscreen
		if (this->m_bFullScreen)
		{
			//Resize (First)
			SDL_GetWindowSize(this->m_pWin, &this->m_tScreenSize.w, &this->m_tScreenSize.h);
			SDL_SetWindowSize(this->m_pWin, SCREEN_WIDTH, SCREEN_HEIGHT);
			//Goto Windowed/FullScreen
			SDL_SetWindowFullscreen(this->m_pWin, this->m_bFullScreen);
		}
		else// Windowed
		{
			//Goto Windowed/FullScreen
			SDL_SetWindowFullscreen(this->m_pWin, this->m_bFullScreen);
			//Resize
			SDL_SetWindowSize(this->m_pWin, this->m_tScreenSize.w, this->m_tScreenSize.h);
		}
	}
	void CSprite::FullScreenToggle(void)
	{
		this->SetFullScreen(!this->m_bFullScreen);
	}
	CSprite::ECoordinateSystem CSprite::GetCoordSys(void)
	{
		return this->m_eCoordinateSystem;
	}
	
	/*------------------------------------------------------------------*/
	/*					Drawing											*/
	/*------------------------------------------------------------------*/
	//Draw Points
	void CSprite::DrawPoint(Type2<slong> pos, _COLOR<uchar> color)
	{
		if (color.a == 0)
			return;

		if (this->m_eCoordinateSystem == CARTESIAN)
			pos.y = SCREEN_HEIGHT - pos.y;

		SDL_SetRenderDrawColor(this->m_pRen, color.r, color.g, color.b, color.a);
		SDL_RenderDrawPoint(this->m_pRen, pos.x, pos.y);
	}
	void CSprite::DrawPoint(slong nX, slong nY, _COLOR<uchar> color)
	{
		DrawPoint(Type2<slong>(nX, nY), color);
	}
	//Draw Lines
	void CSprite::DrawLine(Type2<slong> alpha, Type2<slong> omega, _COLOR<uchar> color, const ulong size)
	{
		if (color.a == 0)
			return;

		if (this->m_eCoordinateSystem == CARTESIAN)
		{
			alpha.y = SCREEN_HEIGHT - alpha.y;
			omega.y = SCREEN_HEIGHT - omega.y;
		}

		SDL_SetRenderDrawColor(this->m_pRen, color.r, color.g, color.b, color.a);

		SDL_RenderDrawLine(this->m_pRen, alpha.x, alpha.y, omega.x, omega.y);

		if (size == 1)
			return;

		//Find the shallow axis
		Type2<slong> diff = alpha - omega;

		if (abs(diff.x) < abs(diff.y)) //X is the shallow axis
		{
			diff.x = 1;
			diff.y = 0;
		}
		else //Y is the shallow axis
		{
			diff.x = 0;
			diff.y = 1;
		}

		Type2<slong> tempA = alpha;
		Type2<slong> tempO = omega;
		for (ulong i = 1; i < size; i++)
		{
			tempA += diff;
			tempO += diff;
			SDL_RenderDrawLine(this->m_pRen, tempA.x, tempA.y, tempO.x, tempO.y);
		}
		tempA = alpha;
		tempO = omega;
		for (ulong i = 1; i < size; i++)
		{
			tempA -= diff;
			tempO -= diff;
			SDL_RenderDrawLine(this->m_pRen, tempA.x, tempA.y, tempO.x, tempO.y);
		}

		//Type2<slong> diff;
		//if ((alpha - omega).x != 0)
		//	diff.y = 1;
		//if ((alpha - omega).y != 0)
		//	diff.x = 1;
		//Type2<slong> tempA = alpha;
		//Type2<slong> tempO = omega;
		//for (ulong i = 1; i < size; i++)
		//{
		//	tempA += diff;
		//	tempO += diff;
		//	SDL_RenderDrawLine(this->m_pRen, tempA.x, tempA.y, tempO.x, tempO.y);
		//}
		//tempA = alpha;
		//tempO = omega;
		//for (ulong i = 1; i < size; i++)
		//{
		//	tempA -= diff;
		//	tempO -= diff;
		//	SDL_RenderDrawLine(this->m_pRen, tempA.x, tempA.y, tempO.x, tempO.y);
		//}
	}
	void CSprite::DrawLine(slong x1, slong y1, slong x2, slong y2, _COLOR<uchar> color, const ulong size)
	{
		DrawLine(Type2<slong>(x1, y1), Type2<slong>(x2, y2), color, size);
	}
	//Draw Rectangles
	void CSprite::DrawRect(_RECT<slong> rect, _COLOR<uchar> color, bool bFill, ulong size)
	{
		if (color.a == 0)
			return;

		if (this->m_eCoordinateSystem == CARTESIAN)
			rect.y = SCREEN_HEIGHT - rect.y - rect.h;

		SDL_Rect sRect = *(SDL_Rect*)&rect;

		SDL_SetRenderDrawColor(this->m_pRen, color.r, color.g, color.b, color.a);
		if (bFill)
			SDL_RenderFillRect(this->m_pRen, &sRect);
		else
		{
			for (ulong i = 0; i < size; i++)
			{
				SDL_RenderDrawRect(this->m_pRen, &sRect);
				sRect.x++;
				sRect.y++;
				sRect.w -= 2;
				sRect.h -= 2;
			}
		}
	}
	void CSprite::DrawRect(slong nX, slong nY, slong nW, slong nH, _COLOR<uchar> color, bool bFill, ulong size)
	{
		DrawRect(_RECT<slong>(nX, nY, nW, nH), color, bFill, size);
	}
	//Render Textures
	void CSprite::Rotate(const slong nID, TRotation* rot, const SDL_Rect* area, const Type2<float>* fScale)
	{
		if (nID < 0 || nID >= (slong)m_vTextures.size())
			return;
		if (this->m_vTextures[nID].m_pTex == nullptr)
			return;

		//Get point of rotation
		if (!rot->m_bTexSpace)
		{
			rot->m_tCenterPoint.x -= area->x;
			rot->m_tCenterPoint.y -= area->y;
			//Coordinates
			if (this->m_eCoordinateSystem == ECoordinateSystem::CARTESIAN)
			{
				rot->m_tCenterPoint.y -= area->h;
				rot->m_tCenterPoint.y *= -1;
			}
		}
		if (rot->m_bInPlace)
		{
			rot->m_tCenterPoint.x = (slong)(this->m_vTextures[nID].m_tSize.w / 2.0f * fScale->x);
			rot->m_tCenterPoint.y = (slong)(this->m_vTextures[nID].m_tSize.h / 2.0f * fScale->y);
		}
	}
	void CSprite::DrawTex(slong nID, Type2<slong> nPos, Type2<float> fScale, SDL_Rect* pSection, TRotation* rot, SDL_BlendMode bMode, _COLOR<uchar> color)
	{
		if (color.a == 0)
			return;

		//Make sure the nID is in range
		assert(nID > -1 && nID < (slong)this->m_vTextures.size() && "nID is out of range");

		//Make sure the texture is valid
		assert(this->m_vTextures[nID].m_pTex != nullptr && "Attempting to draw release texture id");

		//Turn on blending
		SDL_SetTextureBlendMode(this->m_vTextures[nID].m_pTex, bMode);
		//Set Alpha mod
		SDL_SetTextureAlphaMod(this->m_vTextures[nID].m_pTex, color.a);
		//Set Color mod
		SDL_SetTextureColorMod(this->m_vTextures[nID].m_pTex, color.r, color.g, color.b);

		//Set Src and Dst rect
		SDL_Rect srcRect;
		SDL_Rect dstRect;
		srcRect.x = srcRect.y = 0;
		dstRect.x = nPos.x;
		dstRect.y = nPos.y;
		if (pSection != nullptr)
		{
			srcRect = *pSection;
			dstRect.w = slong(pSection->w * fScale.x);
			dstRect.h = slong(pSection->h * fScale.y);
		}
		else
		{
			srcRect.w = this->m_vTextures[nID].m_tSize.w;
			srcRect.h = this->m_vTextures[nID].m_tSize.h;
			dstRect.w = slong(this->m_vTextures[nID].m_tSize.w * fScale.x);
			dstRect.h = slong(this->m_vTextures[nID].m_tSize.h * fScale.y);
		}

		//Rotation
		SDL_Point center;
		center.x = center.y = 0;
		float angle = 0;
		SDL_RendererFlip flip = SDL_FLIP_NONE;
		if (rot != nullptr)
		{
			Rotate(nID, rot, &dstRect, &fScale);
			center = *(SDL_Point*)&rot->m_tCenterPoint;
			angle = rot->m_fAngle;
			flip = rot->m_eFlipType;
		}

		//Coordinates
		if (this->m_eCoordinateSystem == CARTESIAN)
			dstRect.y = SCREEN_HEIGHT - dstRect.y - dstRect.h;

		//Draw
		SDL_RenderCopyEx(this->m_pRen, this->m_vTextures[nID].m_pTex, &srcRect, &dstRect, angle, &center, flip);

		if (this->m_bShowRect)
			this->DrawRect(_RECT<slong>(nPos.x, nPos.y, dstRect.w, dstRect.h), CLR::WHITE, false);
	}
}