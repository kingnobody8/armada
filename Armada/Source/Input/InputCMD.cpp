#include "InputCMD.h"
#include "../SDL/Sprite.h"

namespace Input
{
	CInput::CInput(void)
	{
		//Window
		this->m_pHWND = NULL;
		//Keys
		this->Reset();
		//Text
		this->m_strText.clear();
		this->m_bTextMode = false;
		this->m_slTextMarker = 0;
		this->m_dTextTimer = TEXT_RATE;
		this->m_dMarkerTimer = MARKER_RATE;
		//SDL
		this->m_pSprite = NULL;
		//Input Type
		this->m_eInputType = eInputType::DEFAULT;
		//Cursor Lock
		this->m_bCursorLockOverride = true;
	}
	CInput::~CInput(void)
	{
	}


	/*------------------------------------------------------------------*/
	/*					Helper Funcs									*/
	/*------------------------------------------------------------------*/
	void CInput::SetFocus(const bool bFocus, const Time dDelta)
	{
		this->m_yFocus.lock();
		this->m_tFocus.Next(bFocus);
		this->m_tFocus.Flush(dDelta);
		this->m_yFocus.unlock();
	}
	void CInput::SetKeyState(const Time dDelta)
	{
		unsigned char	keys[256];
		bool anykey = false;

		//NOTE: Loop is in an optimized format
		/*
		Pointer arithmetic is slightly faster than array indexing
		Loop unrolling decreases dependencies and allows for more throughput
		*/
		for (int i = 0; i < 256; i += 4)
		{
			*(keys + i) = GetAsyncKeyState(i) >> 8;
			*(keys + i + 1) = GetAsyncKeyState(i + 1) >> 8;
			*(keys + i + 2) = GetAsyncKeyState(i + 2) >> 8;
			*(keys + i + 3) = GetAsyncKeyState(i + 3) >> 8;
		}
		//Toggle keys are special (don't bit shift and use 'GetKeyState' instead of 'Async')
		*(keys + VK_CAPITAL) = (char)GetKeyState(VK_CAPITAL);
		*(keys + VK_NUMLOCK) = (char)GetKeyState(VK_NUMLOCK);

		//AnyKey (255 doesn't count)
		for (unsigned int i = 0; i < 255; i++)
		{
			if (*(keys + i) != 0 && i != VK_CAPITAL && i != VK_NUMLOCK)
				keys[VK_ANYKEY] = true;
		}

		this->m_yKeyState.lock();
		//NOTE: Loop is in an optimized format
		for (unsigned int i = 0; i < 256; i += 4)
		{
			//Next
			this->m_vKeyState[i + 0].Next(keys[i + 0] != 0);
			this->m_vKeyState[i + 1].Next(keys[i + 1] != 0);
			this->m_vKeyState[i + 2].Next(keys[i + 2] != 0);
			this->m_vKeyState[i + 3].Next(keys[i + 3] != 0);

			//Flush
			this->m_vKeyState[i + 0].Flush(dDelta);
			this->m_vKeyState[i + 1].Flush(dDelta);
			this->m_vKeyState[i + 2].Flush(dDelta);
			this->m_vKeyState[i + 3].Flush(dDelta);
		}
		this->m_yKeyState.unlock();
	}
	void CInput::SetCursor(void)
	{
		POINT mLoc;
		GetPhysicalCursorPos(&mLoc);
		this->m_yCursor.lock();
		this->m_cCursor.SetCurrRaw(mLoc);
		this->m_yCursor.unlock();
	}
	void CInput::SetCtrl(const Time dDelta)
	{
		//Get states
		XINPUT_STATE	xState[MAX_CTRL];
		for (int i = 0; i < MAX_CTRL; i++)
			xState[i] = this->m_vXbox[i].GetState();

		//Update states
		this->m_yCtrl.lock();
		for (int i = 0; i < eCTRL::MAX_CTRL; i++)
		{
			this->m_vCtrl[i].Next(xState[i]);
			this->m_vCtrl[i].Flush(dDelta);
		}
		this->m_yCtrl.unlock();
	}
	void CInput::SetText(double dDelta)
	{
		if (!this->m_bTextMode)
			return;

		//Get the shift state
		bool k_shift = this->m_vKeyState[VK_SHIFT].Held();
		bool k_caps = this->m_vKeyState[VK_CAPITAL].Held();
		bool shift = ((!k_caps && k_shift) || (k_caps && !k_shift));

		//Variables to use
		std::string		next;
		unsigned char	let = 0;

#pragma region PUSH
		if (this->m_vKeyState[VK_ANYKEY].Push())
			this->m_dTextTimer = WAIT_TIME;

		//Move the Text Marker
		m_yText.lock();
		if (this->m_vKeyState[VK_LEFT].Push())
		{
			this->m_dMarkerTimer = MARKER_RATE;
			this->m_slTextMarker--;
		}
		if (this->m_vKeyState[VK_RIGHT].Push())
		{
			this->m_dMarkerTimer = MARKER_RATE;
			this->m_slTextMarker++;
		}
		m_slTextMarker = clamp<slong>(m_slTextMarker, 0, this->m_strText.length());
		m_yText.unlock();

		//Letters
		for (int i = 'A'; i <= 'Z'; i++)
		{
			let = i;
			if (!shift)
				let = tolower(let);
			if (this->m_vKeyState[i].Push())
				next.push_back(let);
		}

		//Number Line
		for (int i = '0'; i <= '9'; i++)
		{
			let = i;
			if (shift)
			{
				switch (i - '0')
				{
				case 0:
					let = ')';
					break;
				case 1:
					let = '!';
					break;
				case 2:
					let = '@';
					break;
				case 3:
					let = '#';
					break;
				case 4:
					let = '$';
					break;
				case 5:
					let = '%';
					break;
				case 6:
					let = '^';
					break;
				case 7:
					let = '&';
					break;
				case 8:
					let = '*';
					break;
				case 9:
					let = '(';
					break;
				}
			}
			if (this->m_vKeyState[i].Push())
				next.push_back(let);
		}
		//Numpad Numbers
		for (int i = VK_NUMPAD0; i <= VK_NUMPAD9; i++)
		{
			let = *std::to_string(i - VK_NUMPAD0).c_str();
			if (this->m_vKeyState[i].Push())
				next.push_back(let);
		}
		//Numpad Operators
		if (this->m_vKeyState[VK_MULTIPLY].Push())
			next.push_back('*');
		if (this->m_vKeyState[VK_ADD].Push())
			next.push_back('+');
		if (this->m_vKeyState[VK_SUBTRACT].Push())
			next.push_back('-');
		if (this->m_vKeyState[VK_DECIMAL].Push())
			next.push_back('.');
		if (this->m_vKeyState[VK_DIVIDE].Push())
			next.push_back('/');

		//Special Characters
		if (!shift)
		{
			if (this->m_vKeyState[VK_OEM_1].Push())
				next.push_back(';');
			if (this->m_vKeyState[VK_OEM_2].Push())
				next.push_back('/');
			if (this->m_vKeyState[VK_OEM_3].Push())
				next.push_back('`');
			if (this->m_vKeyState[VK_OEM_4].Push())
				next.push_back('[');
			if (this->m_vKeyState[VK_OEM_5].Push())
				next.push_back('\\');
			if (this->m_vKeyState[VK_OEM_6].Push())
				next.push_back(']');
			if (this->m_vKeyState[VK_OEM_7].Push())
				next.push_back('\'');
			if (this->m_vKeyState[VK_OEM_COMMA].Push())
				next.push_back(',');
			if (this->m_vKeyState[VK_OEM_PERIOD].Push())
				next.push_back('.');
			if (this->m_vKeyState[VK_OEM_PLUS].Push())
				next.push_back('=');
			if (this->m_vKeyState[VK_OEM_MINUS].Push())
				next.push_back('-');
		}
		else
		{
			if (this->m_vKeyState[VK_OEM_1].Push())
				next.push_back(':');
			if (this->m_vKeyState[VK_OEM_2].Push())
				next.push_back('?');
			if (this->m_vKeyState[VK_OEM_3].Push())
				next.push_back('~');
			if (this->m_vKeyState[VK_OEM_4].Push())
				next.push_back('{');
			if (this->m_vKeyState[VK_OEM_5].Push())
				next.push_back('|');
			if (this->m_vKeyState[VK_OEM_6].Push())
				next.push_back('}');
			if (this->m_vKeyState[VK_OEM_7].Push())
				next.push_back('"');
			if (this->m_vKeyState[VK_OEM_COMMA].Push())
				next.push_back('<');
			if (this->m_vKeyState[VK_OEM_PERIOD].Push())
				next.push_back('>');
			if (this->m_vKeyState[VK_OEM_PLUS].Push())
				next.push_back('+');
			if (this->m_vKeyState[VK_OEM_MINUS].Push())
				next.push_back('_');
		}

		//Space
		if (this->m_vKeyState[VK_SPACE].Push())
			next.push_back(' ');
		//Tab
		if (this->m_vKeyState[VK_TAB].Push())
		{
			unsigned int tabsize = 4 - (this->m_strText.length() + next.length()) % 4;
			for (unsigned int i = 0; i < tabsize; i++)
				next.push_back(' ');
		}

		m_yText.lock();
		//Backspace
		if (this->m_vKeyState[VK_BACK].Push() && this->m_slTextMarker > 0)
		{
			this->m_strText.erase(this->m_slTextMarker - 1, 1);
			this->m_slTextMarker -= 1;
		}
		//Delete
		if (this->m_vKeyState[VK_DELETE].Push() && this->m_slTextMarker < (int)this->m_strText.length())
			this->m_strText.erase(this->m_slTextMarker, 1);
		m_yText.unlock();

#pragma endregion
#pragma region HELD
		if (this->m_vKeyState[VK_ANYKEY].Held())
			this->m_dTextTimer -= dDelta;
		bool confirm = this->m_dTextTimer < 0.0f;
		if (confirm)
		{
			this->m_dTextTimer = TEXT_RATE;

			//Move the textMarker
			m_yText.lock();
			if (this->m_vKeyState[VK_LEFT].Held())
			{
				this->m_dMarkerTimer = MARKER_RATE;
				this->m_slTextMarker--;
			}
			if (this->m_vKeyState[VK_RIGHT].Held())
			{
				this->m_dMarkerTimer = MARKER_RATE;
				this->m_slTextMarker++;
			}
			m_slTextMarker = clamp<slong>(m_slTextMarker, 0, this->m_strText.length());
			m_yText.unlock();


			////Letters
			//for (int i = 'A'; i <= 'Z'; i++)
			//{
			//	let = i;
			//	if (!shift)
			//		let = tolower(let);
			//	if (this->m_vKeyState[i].Held())
			//		next.push_back(let);
			//}

			////Number Line
			//for (int i = '0'; i <= '9'; i++)
			//{
			//	let = i;
			//	if (shift)
			//	{
			//		switch (i - '0')
			//		{
			//		case 0:
			//			let = ')';
			//			break;
			//		case 1:
			//			let = '!';
			//			break;
			//		case 2:
			//			let = '@';
			//			break;
			//		case 3:
			//			let = '#';
			//			break;
			//		case 4:
			//			let = '$';
			//			break;
			//		case 5:
			//			let = '%';
			//			break;
			//		case 6:
			//			let = '^';
			//			break;
			//		case 7:
			//			let = '&';
			//			break;
			//		case 8:
			//			let = '*';
			//			break;
			//		case 9:
			//			let = '(';
			//			break;
			//		}
			//	}
			//	if (this->m_vKeyState[i].Held())
			//		next.push_back(let);
			//}
			////Numpad Numbers
			//for (int i = VK_NUMPAD0; i <= VK_NUMPAD9; i++)
			//{
			//	let = *std::to_string(i - VK_NUMPAD0).c_str();
			//	if (this->m_vKeyState[i].Held())
			//		next.push_back(let);
			//}
			////Numpad Operators
			//if (this->m_vKeyState[VK_MULTIPLY].Held())
			//	next.push_back('*');
			//if (this->m_vKeyState[VK_ADD].Held())
			//	next.push_back('+');
			//if (this->m_vKeyState[VK_SUBTRACT].Held())
			//	next.push_back('-');
			//if (this->m_vKeyState[VK_DECIMAL].Held())
			//	next.push_back('.');
			//if (this->m_vKeyState[VK_DIVIDE].Held())
			//	next.push_back('/');

			////Special Characters
			//if (!shift)
			//{
			//	if (this->m_vKeyState[VK_OEM_1].Held())
			//		next.push_back(';');
			//	if (this->m_vKeyState[VK_OEM_2].Held())
			//		next.push_back('/');
			//	if (this->m_vKeyState[VK_OEM_3].Held())
			//		next.push_back('`');
			//	if (this->m_vKeyState[VK_OEM_4].Held())
			//		next.push_back('[');
			//	if (this->m_vKeyState[VK_OEM_5].Held())
			//		next.push_back('\\');
			//	if (this->m_vKeyState[VK_OEM_6].Held())
			//		next.push_back(']');
			//	if (this->m_vKeyState[VK_OEM_7].Held())
			//		next.push_back('\'');
			//	if (this->m_vKeyState[VK_OEM_COMMA].Held())
			//		next.push_back(',');
			//	if (this->m_vKeyState[VK_OEM_PERIOD].Held())
			//		next.push_back('.');
			//	if (this->m_vKeyState[VK_OEM_PLUS].Held())
			//		next.push_back('=');
			//	if (this->m_vKeyState[VK_OEM_MINUS].Held())
			//		next.push_back('-');
			//}
			//else
			//{
			//	if (this->m_vKeyState[VK_OEM_1].Held())
			//		next.push_back(':');
			//	if (this->m_vKeyState[VK_OEM_2].Held())
			//		next.push_back('?');
			//	if (this->m_vKeyState[VK_OEM_3].Held())
			//		next.push_back('~');
			//	if (this->m_vKeyState[VK_OEM_4].Held())
			//		next.push_back('{');
			//	if (this->m_vKeyState[VK_OEM_5].Held())
			//		next.push_back('|');
			//	if (this->m_vKeyState[VK_OEM_6].Held())
			//		next.push_back('}');
			//	if (this->m_vKeyState[VK_OEM_7].Held())
			//		next.push_back('"');
			//	if (this->m_vKeyState[VK_OEM_COMMA].Held())
			//		next.push_back('<');
			//	if (this->m_vKeyState[VK_OEM_PERIOD].Held())
			//		next.push_back('>');
			//	if (this->m_vKeyState[VK_OEM_PLUS].Held())
			//		next.push_back('+');
			//	if (this->m_vKeyState[VK_OEM_MINUS].Held())
			//		next.push_back('_');
			//}

			////Space
			//if (this->m_vKeyState[VK_SPACE].Held())
			//	next.push_back(' ');
			////Tab
			//if (this->m_vKeyState[VK_TAB].Held())
			//{
			//	unsigned int tabsize = 4 - (this->m_strText.length() + next.length()) % 4;
			//	for (unsigned int i = 0; i < tabsize; i++)
			//		next.push_back(' ');
			//}
			m_yText.lock();
			//Backspace
			if (this->m_vKeyState[VK_BACK].Held() && this->m_slTextMarker > 0)
			{
				this->m_strText.erase(this->m_slTextMarker - 1, 1);
				this->m_slTextMarker -= 1;
			}
			//Delete
			if (this->m_vKeyState[VK_DELETE].Held() && this->m_slTextMarker < (int)this->m_strText.length())
				this->m_strText.erase(this->m_slTextMarker, 1);
			m_yText.unlock();
		}
#pragma endregion

		if (!next.empty())
			next = next[0];
		//Add all new text to actual string
		m_yText.lock();
		this->m_strText.insert(this->m_slTextMarker, next);
		this->m_slTextMarker += next.length();
		m_yText.unlock();
	}


	/*------------------------------------------------------------------*/
	/*					Important Funcs									*/
	/*------------------------------------------------------------------*/
	//Start
	/*
	Allows the inputCMD class to begin, call this once at the begginning of the application.
	Needs a pointer to the window's hWnd.
	*/
	void CInput::Start(HWND* const pHWND, Sprite::CSprite* const pSprite)
	{
		this->m_pHWND = pHWND;
		this->m_pSprite = pSprite;
		for (int i = 0; i < MAX_CTRL; i++)
			m_vXbox[i] = Xbox(i + 1);
	}
	//Update
	/*
	Polls all the device states and updates our device instances
	*/
	void CInput::Update(const double dDelta)
	{
		//If no window, then don't do update
		if (this->m_pHWND == NULL)
			return;

		//Sync Active Window
		HWND fore = GetForegroundWindow();
		this->SetFocus(fore == *this->m_pHWND, dDelta);


		//If Focus Push
		if (this->GetFocus().Push())
		{
			if (this->GetFinalInputType() != MOUSE)
				this->Cursor_Set_Override(false);
		}
		//If Focus Pull
		if (this->GetFocus().Pull())
		{
			if (this->GetFinalInputType() != MOUSE)
			{
				this->Cursor_Set_Override(true);
				this->Cursor_Lock(false);
			}
		}
		//If Focus Not Held
		if (!this->GetFocus().Held())
		{
			this->Reset(true,true);
			return;
		}

		//Polls and Sets
		this->SetKeyState(dDelta);
		this->SetCursor();
		this->SetCtrl(dDelta);
		this->SetText(dDelta);
	}
	//Reset
	/*
	Restores all data members to a defualt state.
	Call this when switching game states.
	*/
	void CInput::Reset(bool bKeepOldCursor, bool bKeepTextMode)
	{
		//Reset Keys
		this->m_yKeyState.lock();
		for (int i = 0; i < 256; i++)
			this->m_vKeyState[i] = _Key();
		this->m_yKeyState.unlock();

		//Reset Cursor
		if (!bKeepOldCursor)
		{
			this->m_yCursor.lock();
			this->m_cCursor = _Cursor();
			this->m_yCursor.unlock();
		}

		//Reset Mouse Wheel
		m_yWheel.lock();
		this->m_scWheel = 0;
		m_yWheel.unlock();

		//Reset Controllers
		if (!bKeepOldCursor)
		{
			this->m_yCtrl.lock();
			for (int i = 0; i < MAX_CTRL; i++)
				this->m_vCtrl[i] = _Controller();
			this->m_yCtrl.unlock();
		}

		//Text
		if (!bKeepTextMode)
		{
			this->m_yText.lock();
			this->m_strText.clear();
			this->m_bTextMode = false;
			this->m_slTextMarker = 0;
			this->m_dTextTimer = TEXT_RATE;
			this->m_dMarkerTimer = MARKER_RATE;
			this->m_yText.unlock();
		}
	}


	/*------------------------------------------------------------------*/
	/*					Keyboard										*/
	/*------------------------------------------------------------------*/
	//Key_Held
	/*
	Returns if the specified key is held down
	Uses the Virtual Keys #defines
	*/
	bool CInput::Key_Held(const uchar ucKey)
	{
		this->m_yKeyState.lock();
		bool ret = this->m_vKeyState[ucKey].Held();
		this->m_yKeyState.unlock();
		return ret;
	}
	//Key_Push
	/*
	Returns if the specified key is PUSHED down and this is the first time the key has been queried since it has been held down.
	Uses the Virtual Keys #defines
	*/
	bool CInput::Key_Push(const uchar ucKey)
	{
		this->m_yKeyState.lock();
		bool ret = this->m_vKeyState[ucKey].Push();
		this->m_yKeyState.unlock();
		return ret;
	}
	//Key_Pull
	/*
	Returns if the specified key is PULLED up and this is the first time the key has been queried since it has been up.
	Uses the Virtual Keys #defines
	*/
	bool CInput::Key_Pull(const uchar ucKey)
	{
		this->m_yKeyState.lock();
		bool ret = this->m_vKeyState[ucKey].Pull();
		this->m_yKeyState.unlock();
		return ret;
	}
	//Key_Time
	/*
	Returns how long the specified key has been in it's current state (UP or DOWN).
	Uses the Virtual Keys #defines
	*/
	double CInput::Key_Time(const uchar ucKey)
	{
		this->m_yKeyState.lock();
		double ret = this->m_vKeyState[ucKey].Time();
		this->m_yKeyState.unlock();
		return ret;
	}


	/*------------------------------------------------------------------*/
	/*					Mouse Wheel										*/
	/*------------------------------------------------------------------*/
	//Wheel_Set
	/*
	adds a scroll direction to the mouse wheel
	*/
	void CInput::Wheel_Set(const schar scDir)
	{
		this->m_yWheel.lock();
		this->m_scWheel += scDir;
		this->m_yWheel.unlock();
	}
	//GetWheel
	/*
	Returns how far the mouse wheel has been scrolled since the last time this function has been called
	Resets the wheel afterwards
	*/
	schar CInput::Wheel_Get(void)
	{
		m_yWheel.lock();
		char ret = this->m_scWheel;
		this->m_scWheel = 0;
		m_yWheel.unlock();
		return ret;
	}


	/*------------------------------------------------------------------*/
	/*					Mouse Cursor									*/
	/*------------------------------------------------------------------*/
	//Cursor_Set_PosRaw
	/*
	Moves the mouse cursor to the specified raw coordinantes without adjusting the delta value.
	*/
	void CInput::Cursor_Set_PosRaw(const Type2<slong> tPt)
	{
		POINT p;
		p.x = tPt.x;
		p.y = tPt.y;
		this->m_yCursor.lock();
		this->m_cCursor.SetCurrRawNU(p);
		this->m_yCursor.unlock();
	}
	//Cursor_Set_PosScreen
	/*
	Moves the mouse cursor to the specified screen coordinantes without adjusting the delta value.
	*/
	void CInput::Cursor_Set_PosScreen(const Type2<slong> tPt)
	{
		POINT p;
		p.x = tPt.x;
		p.y = tPt.y;
		this->m_yCursor.lock();
		this->m_cCursor.SetCurrScreenNU(p, this->m_pHWND);
		this->m_yCursor.unlock();
	}
	//Cursor_Set_PosFloat
	/*
	Moves the mouse cursor to the specified floating point screen coordinantes without adjusting the delta value.
	*/
	void CInput::Cursor_Set_PosFloat(const Type2<float> tPt)
	{
		this->m_yCursor.lock();
		this->m_cCursor.SetCurrScreenPercentNU(tPt, this->m_pHWND);
		this->m_yCursor.unlock();
	}
	//Cursor_Set_Visible
	/*
	Sets the visibility of the the cursor
	*/
	void CInput::Cursor_Set_Visible(const bool isVisible)
	{
		this->m_yCursor.lock();
		this->m_cCursor.SetVisible(isVisible);
		this->m_yCursor.unlock();
	}
	//Cursor_Set_Clipped
	/*
	If TRUE		- cursor will be unable to move outside the bounds of the client rect
	If FALSE	- cursor will move freely
	*/
	void CInput::Cursor_Set_Clipped(const bool isClipped)
	{
		this->m_yCursor.lock();
		this->m_cCursor.SetClipped(isClipped);
		this->m_yCursor.unlock();
	}
	//Cursor_Set_Moveable
	/*
	If TRUE		- cursor will move freely
	If FALSE	- cursor will be locked in place
	NOTE:	- All other functions still work, including SetCursor_Pos & GetCursor_Delta
	*/
	void CInput::Cursor_Set_Moveable(const bool canMove)
	{
		this->m_yCursor.lock();
		this->m_cCursor.SetMoveable(canMove);
		this->m_yCursor.unlock();
	}
	//Cursor_Get_PosRaw
	/*
	returns raw cursor position
	*/
	Type2<slong> CInput::Cursor_Get_PosRaw(void)
	{
		this->m_yCursor.lock();
		POINT ret = this->m_cCursor.GetCurrRaw();
		this->m_yCursor.unlock();

		Type2<slong> tPt = Type2<slong>(ret.x, ret.y);
		return tPt;
	}
	//Cursor_Get_PosScreen
	/*
	returns raw cursor position in screen space
	*/
	Type2<slong> CInput::Cursor_Get_PosScreen(void)
	{
		this->m_yCursor.lock();
		POINT ret = this->m_cCursor.GetCurrScreen(this->m_pHWND);
		this->m_yCursor.unlock();

		Type2<slong> tPt = Type2<slong>(ret.x, ret.y);
		return tPt;
	}
	//Cursor_Get_PosFloat
	/*
	returns floating point cursor position in screen space
	*/
	Type2<float> CInput::Cursor_Get_PosFloat(void)
	{
		this->m_yCursor.lock();
		Type2<float> ret = this->m_cCursor.GetCurrScreenPercent(this->m_pHWND);
		this->m_yCursor.unlock();
		return ret;
	}
	//Cursor_Get_DeltaRaw
	/*
	returns how far cursor has moved since last query in raw POINT
	*/
	Type2<slong> CInput::Cursor_Get_DeltaRaw(void)
	{
		this->m_yCursor.lock();
		POINT ret = this->m_cCursor.GetDeltaRaw();
		this->m_yCursor.unlock();

		Type2<slong> tPt = Type2<slong>(ret.x, ret.y);
		return tPt;
	}
	//Cursor_Get_DeltaFloat
	/*
	returns how far cursor has moved since last query in floating point screen space
	*/
	Type2<float> CInput::Cursor_Get_DeltaFloat(void)
	{
		this->m_yCursor.lock();
		Type2<float> ret = this->m_cCursor.GetDeltaScreenPercent(this->m_pHWND);
		this->m_yCursor.unlock();
		return ret;
	}
	//Cursor_Get_Visible
	/*
	returns is cursor is visible
	*/
	bool CInput::Cursor_Get_Visible(void)
	{
		this->m_yCursor.lock();
		bool ret = this->m_cCursor.GetVisible();
		this->m_yCursor.unlock();
		return ret;
	}
	//Cursor_Get_Clipped
	/*
	returns if the cursor is clipped to client rect
	*/
	bool CInput::Cursor_Get_Clipped(void)
	{
		this->m_yCursor.lock();
		bool ret = this->m_cCursor.GetClipped();
		this->m_yCursor.unlock();
		return ret;
	}
	//Cursor_Get_Moveable
	/*
	returns if the cursor is moveable
	*/
	bool CInput::Cursor_Get_Moveable(void)
	{
		this->m_yCursor.lock();
		bool ret = this->m_cCursor.GetMoveable();
		this->m_yCursor.unlock();
		return ret;
	}
	//Cursor_SDL
	/*
		filters the screen pos value through SDL
		*/
	Type2<slong> CInput::Cursor_SDL(void)
	{
		Type2<slong> ret = this->Cursor_Get_PosScreen();
		Type2<float> scale = this->m_pSprite->GetScreenScale();
		Sprite::CSprite::ECoordinateSystem eCSys = this->m_pSprite->GetCoordSys();

		//Scale
		ret.x = (slong)(ret.x / scale.x);
		ret.y = (slong)(ret.y / scale.y);

		//Invert for coord system
		if (eCSys == Sprite::CSprite::ECoordinateSystem::CARTESIAN)
			ret.y = Sprite::SCREEN_HEIGHT - ret.y;
		return ret;
	}
	//Cursor_Lock
	/*
		centers the mouse, turns it invisible, and locks it in place
		*/
	void CInput::Cursor_Lock(const bool b)
	{
		if (b && !m_bCursorLockOverride)
		{
			this->Cursor_Set_Moveable(false);
			this->Cursor_Set_PosFloat(Type2<float>(0.5f, 0.5f));
			this->Cursor_Set_Visible(false);
		}
		else
		{
			this->Cursor_Set_Moveable(true);
			this->Cursor_Set_Visible(true);
		}
	}
	//Cursor_Set_Override
	/*
		override determines if Cursor_Lock performs normally, true == cursor never is locked, false = lock behaves normally
		*/
	void CInput::Cursor_Set_Override(const bool bOverride)
	{
		this->m_bCursorLockOverride = bOverride;
	}
	//Cursor_Set_Override
	/*
		override determines if Cursor_Lock performs normally, true == cursor never is locked, false = lock behaves normally
		*/
	bool CInput::Cursor_Get_Override(void)
	{
		return this->m_bCursorLockOverride;
	}


	/*------------------------------------------------------------------*/
	/*					Controllers										*/
	/*------------------------------------------------------------------*/
	//Ctrl_Set_Trig_Dead
	/*
	Sets the DeadZone of the triggers
	nDead		= percent 0.0f to 1.0f (any trigger value below this nDead value will be ignored)
	nButton		= CTRL_KEYS enum ( XB_LTRIG or XB_RTRIG )
	nCtrl_Num	= the player number ( 0 based )
	*/
	void CInput::Ctrl_Set_Trig_Dead(const float nDead, const unsigned char nButton, const unsigned char nCtrl_Num)
	{
		this->m_yCtrl.lock();
		this->m_vCtrl[nCtrl_Num].SetTrigDeadZone(nButton, nDead);
		this->m_yCtrl.unlock();
	}
	//Ctrl_Set_Stick_Dead
	/*
	Sets the DeadZone of the Thumb Sticks
	nDead		= percent 0.0f to 1.0f (any stick magnitude value below this nDead value will be ignored)
	nButton		= CTRL_KEYS enum ( XB_LTHUMB or XB_RTHUMB )
	nCtrl_Num	= the player number ( 0 based )
	*/
	void CInput::Ctrl_Set_Stick_Dead(const float nDead, const uchar nButton, const uchar nCtrl_Num)
	{
		this->m_yCtrl.lock();
		this->m_vCtrl[nCtrl_Num].SetStickDeadZone(nButton, nDead);
		this->m_yCtrl.unlock();
	}
	//Ctrl_Set_Vibe
	/*
		Sets the vibration amounts in each of the controllers vibrators
		set to -1.0f to keep it the same as previous
		*/
	void CInput::Ctrl_Set_Vibe(const Type2<float> nVibe, const uchar nCtrl_Num)
	{
		this->m_yCtrl.lock();
		this->m_vXbox[nCtrl_Num].Vibrate(nVibe.x, nVibe.y);
		this->m_yCtrl.unlock();
	}
	//Ctrl_Vibe
	/*
	returns the vibration amount in each of the controllers vibrators
	*/
	Type2<float> CInput::Ctrl_Vibe(const uchar nCtrl_Num)
	{
		this->m_yCtrl.lock();
		Type2<float> ret = Type2<float>(this->m_vXbox[nCtrl_Num].GetLeftVibe(), this->m_vXbox[nCtrl_Num].GetRightVibe());
		this->m_yCtrl.unlock();
		return ret;
	}
	//Ctrl_Held
	/*
	returns if the button is HELD down
	nButton		= CTRL_KEYS enum
	nCtrl_Num	= the player number ( 0 based )
	*/
	bool CInput::Ctrl_Held(const uchar nButton, const uchar nCtrl_Num)
	{
		this->m_yCtrl.lock();
		bool ret = this->m_vCtrl[nCtrl_Num].GetKey(nButton)->Held();
		this->m_yCtrl.unlock();
		return ret;
	}
	//Ctrl_Push
	/*
	Returns if the specified button is PUSHED down and this is the first time the key has been queried since it has been held down.
	nButton		= CTRL_KEYS enum
	nCtrl_Num	= the player number ( 0 based )
	*/
	bool CInput::Ctrl_Push(const uchar nButton, const uchar nCtrl_Num)
	{
		this->m_yCtrl.lock();
		bool ret = this->m_vCtrl[nCtrl_Num].GetKey(nButton)->Push();
		this->m_yCtrl.unlock();
		return ret;
	}
	//Ctrl_Pull
	/*
	Returns if the specified button is PULLED up and this is the first time the key has been queried since it has been up.
	nButton		= CTRL_KEYS enum
	nCtrl_Num	= the player number ( 0 based )
	*/
	bool CInput::Ctrl_Pull(const uchar nButton, const uchar nCtrl_Num)
	{
		this->m_yCtrl.lock();
		bool ret = this->m_vCtrl[nCtrl_Num].GetKey(nButton)->Pull();
		this->m_yCtrl.unlock();
		return ret;
	}
	//Ctrl_Time
	/*
	Returns how long the specified key has been in it's current state (UP or DOWN)
	nButton		= CTRL_KEYS enum
	nCtrl_Num	= the player number ( 0 based )
	*/
	double CInput::Ctrl_Time(const uchar nButton, const uchar nCtrl_Num)
	{
		this->m_yCtrl.lock();
		double ret = this->m_vCtrl[nCtrl_Num].GetKey(nButton)->Time();
		this->m_yCtrl.unlock();
		return ret;
	}
	//Ctrl_Trig
	/*
	Returns a float from 0.0f to 1.0f specifiying how pressed the trigger is
	nButton		= CTRL_KEYS enum ( XB_LTRIG or XB_RTRIG )
	nCtrl_Num	= the player number ( 0 based )
	*/
	float CInput::Ctrl_Trig(const uchar nButton, const uchar nCtrl_Num)
	{
		this->m_yCtrl.lock();
		float ret = this->m_vCtrl[nCtrl_Num].GetTrigger(nButton);
		this->m_yCtrl.unlock();
		return ret;
	}
	//Ctrl_Stick
	/*
	Returns a VecComp which contains info about the stick ( Direction and Magnitude )
	nButton		= CTRL_KEYS enum ( XB_LTHUMB or XB_RTHUMB )
	nCtrl_Num	= the player number ( 0 based )
	*/
	VecComp CInput::Ctrl_Stick(const uchar nButton, const uchar nCtrl_Num)
	{
		this->m_yCtrl.lock();
		VecComp ret = this->m_vCtrl[nCtrl_Num].GetStick(nButton);
		this->m_yCtrl.unlock();
		return ret;
	}


	/*------------------------------------------------------------------*/
	/*					Text											*/
	/*------------------------------------------------------------------*/
	//Text_Set_Mode
	/*
	If TRUE		- All Key Pushes will be registered as chars and added to the Text data member
	If FALSE	- No chars will be added to the Text data member
	NOTE:		- Key_Push will not work when in TextMode
	*/
	void CInput::Text_Set_Mode(const bool bMode)
	{
		this->m_yText.lock();
		this->m_strText.clear();
		this->m_bTextMode = bMode;
		this->m_yText.unlock();
	}
	bool CInput::Text_Get_Mode(void)
	{
		this->m_yText.lock();
		bool ret = this->m_bTextMode;
		this->m_yText.unlock();
		return ret;
	}
	void CInput::Text_Set_String(const char* strText)
	{
		this->m_yText.lock();
		this->m_strText = strText;
		this->m_slTextMarker = this->m_strText.length();
		this->m_yText.unlock();
	}
	std::string	 CInput::Text_Get_String(void)
	{
		this->m_yText.lock();
		std::string ret = this->m_strText;
		this->m_yText.unlock();
		return ret;
	}
	void CInput::Text_Set_Marker(const slong slMarker)
	{
		this->m_yText.lock();
		this->m_slTextMarker = clamp<slong>(slMarker, 0, this->m_strText.length());
		this->m_yText.unlock();
	}
	slong CInput::Text_Get_Marker(void)
	{
		this->m_yText.lock();
		slong ret = this->m_slTextMarker;
		this->m_yText.unlock();
		return ret;
	}


	/*------------------------------------------------------------------*/
	/*					Special											*/
	/*------------------------------------------------------------------*/
	//IsActive
	/*
	Returns if the program's window is currently in focus
	Dont perform the Input part of IGameState when window has lost focus
	*/
	_Key CInput::GetFocus(void)
	{
		this->m_yFocus.lock();
		_Key ret = this->m_tFocus;
		this->m_yFocus.unlock();
		return ret;
	}
	void CInput::SetInputType(const eInputType eType)
	{
		this->m_eInputType = eType;
	}
	eInputType CInput::GetInputType(void)
	{
		return this->m_eInputType;
	}
	eInputType CInput::GetFinalInputType(void)
	{
		eInputType ret = this->m_eInputType;

		if ((ret == Input::eInputType::DEFAULT && !this->Ctrl_Held(PLUGIN)) || ret == Input::eInputType::MOUSE)
			ret = MOUSE;
		else
			ret = CTRL_KEYBOARD;
		return ret;
	}

}