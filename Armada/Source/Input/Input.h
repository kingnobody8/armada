/*
Author:		Daniel Habig
Date:		11-2-13
File:		Input.h
Purpose:	These classes represent various forms of input. Each of these are to be used as intermediates between
base input functions and the application itself.
---------------------------------------------------------------------
Classes:	_Key, _Cursor, _DeadZone, _Trigger, _Analog, _Controller
Structs:	VecComp
Enums:		CTRL_KEYS
*/

#pragma once
#include "../Util/Types.h"
#include "xbox.h"
using namespace Type;

namespace Input
{
	//----------------------------------------------------------
	//                 Keys & Buttons
	//----------------------------------------------------------
	class _Key
	{
	private:
		double	m_dTimer;		//Current state timer
		bool	m_bPrev;		//Previous frame's key state
		bool	m_bCurr;		//Current frame's key state
		bool	m_bNext;		//Future frame's key state (Used with Flush)
		bool	m_bSet;			//Has next been called this frame (Used for event based input)

	public:
		_Key(void)
		{
			m_bPrev = m_bCurr = m_bNext = m_bSet = false;
			m_dTimer = 0.0f;
		}

		//Set the state the key will be in when it is flushed
		inline void		Next	(const bool bDown)
		{
			this->m_bNext	= bDown;
			this->m_bSet	= true;
		}
		//Flush all the data and update the timer
		inline void		Flush	(const double dDelta = 0.0f)
		{
			//Curr -> Prev
			this->m_bPrev = this->m_bCurr;
			//Next -> Curr
			if (this->m_bSet)
				this->m_bCurr = this->m_bNext;
			this->m_bSet = false;
			//Timer
			if (this->Diff())
				this->m_dTimer = 0.0f;
			else
				m_dTimer += dDelta;
		}
		//Has key changed from previous state
		inline bool		Diff	(void)	const	{ return this->m_bCurr != m_bPrev; }
		//How long has the key been in its current state (up or down)
		inline double	Time	(void)	const	{ return m_dTimer; }
		//Key Down
		inline bool		Held	(void)	const	{ return this->m_bCurr; }
		//Key Down (no repeats)	
		inline bool		Push	(void) const	{ return this->m_bCurr && !this->m_bPrev; }
		//Key Up (no repeats)
		inline bool		Pull	(void) const	{ return !this->m_bCurr && this->m_bPrev; }
	};


	//----------------------------------------------------------
	//                 Mouse Cursor
	//----------------------------------------------------------
	class _Cursor
	{
	private:
		POINT	curr;		//The cursor's current position
		POINT	prev;		//The cursor's previous position
		POINT	delta;		//The total distance moved since last delta query
		bool	visible;	//Is the cursor currently visible
		bool	clipped;	//Is the cursor currently clipped to the client rect
		bool	moveable;	//Is the cursor currently able to move
		//What is the cursor image (POINTER, TEXT, PRECISION, CUSTOM)

	public:
		_Cursor()
		{
			curr.x = curr.y = prev.x = prev.y = delta.x = delta.y = 0;
			visible = moveable = true;
			clipped = false;
		}
		_Cursor(_Cursor& that)
		{
			*this = that;
		}
		_Cursor& operator= (const _Cursor& that)
		{
			this->SetClipped(that.clipped);
			this->SetMoveable(that.moveable);
			this->SetVisible(that.visible);

			this->curr = that.curr;
			this->prev = that.prev;
			this->delta = that.delta;
			return *this;
		}
		~_Cursor()
		{
			SetClipped(false);
			SetMoveable(true);
			SetVisible(true);
		}

		_Cursor(POINT nPos)
		{
			delta.x = delta.y = 0;
			visible = moveable = true;
			clipped = false;
			curr = nPos;
			prev = curr;
		}
		_Cursor(long nX, long nY)
		{
			delta.x = delta.y = 0;
			visible = moveable = true;
			clipped = false;
			curr.x = nX;
			curr.y = nY;
			prev = curr;
		}

		inline void				Reset(void)
		{
			this->SetClipped(false);
			this->SetMoveable(true);
			this->SetVisible(true);
		}

		//Gets
		inline	POINT			GetCurrRaw(void)			const
		{
			return this->curr;
		}
		inline	POINT			GetCurrScreen(HWND* hWnd)	const
		{
			POINT sLoc = this->curr;
			ScreenToClient(*hWnd, &sLoc);
			return sLoc;
		}
		inline	Type2<float>	GetCurrScreenPercent(HWND* hWnd)	const
		{
			POINT	sLoc = GetCurrScreen(hWnd);
			RECT	client;
			GetClientRect(*hWnd, &client);
			float width = (float)(client.right - client.left);
			float height = (float)(client.bottom - client.top);

			return Type2<float>(sLoc.x / width, sLoc.y / height);
		}
		inline	POINT			GetDeltaRaw(void)
		{
			POINT ret = this->delta;
			this->delta.x = this->delta.y = 0;
			return ret;
		}
		inline	Type2<float>	GetDeltaScreenPercent(HWND* hWnd)
		{
			RECT client;
			GetClientRect(*hWnd, &client);
			Type2<float> ret = Type2<float>((float)(this->delta.x), (float)(this->delta.y));
			this->delta.x = this->delta.y = 0;
			Type2<float> size = Type2<float>((float)(client.right - client.left), (float)(client.bottom - client.top));
			ret = ret / size;
			return ret;
		}
		inline	bool			IsOnScreen(HWND* hWnd)	const
		{
			RECT client;
			GetClientRect(*hWnd, &client);
			POINT sLoc = GetCurrScreen(hWnd);
			if (sLoc.x < 0 || sLoc.x > client.right)
				return false;
			if (sLoc.y < 0 || sLoc.y > client.bottom)
				return false;
			return true;
		}
		inline	bool			GetVisible(void)			const	{ return this->visible; }
		inline	bool			GetClipped(void)			const	{ return this->clipped; }
		inline	bool			GetMoveable(void)			const	{ return this->moveable; }

		//Sets
		inline	void	SetCurrRaw(POINT nPoint)
		{
			this->prev = this->curr;
			this->curr = nPoint;

			this->delta.x += this->curr.x - this->prev.x;
			this->delta.y += this->curr.y - this->prev.y;

			if (!this->moveable)
			{
				SetPhysicalCursorPos(prev.x, prev.y);
				this->curr = this->prev;
			}
		}
		inline	void	SetCurrRaw(long nX, long nY)
		{
			POINT oPoint;
			oPoint.x = nX;
			oPoint.y = nY;
			SetCurrRaw(oPoint);
		}
		inline	void	SetVisible(bool isVisible = true)
		{
			if (this->visible == isVisible)
				return;
			this->visible = isVisible;
			ShowCursor(isVisible);
		}
		inline	void	SetClipped(bool isClipped = false, HWND* hWnd = nullptr)
		{
			if (!isClipped || hWnd == nullptr)
			{
				ClipCursor(nullptr);
				isClipped = false;
				return;
			}
			else
			{
				RECT client;
				GetClientRect(*hWnd, &client);
				ClipCursor(&client);
				isClipped = true;
			}

		}
		inline	void	SetMoveable(bool canMove = true)
		{
			this->moveable = canMove;
		}

		//These are for setting the position without being an update (NU == No Update)
		//Set the Mouse Cursor Position in Raw Space, NU == No Update
		inline	void	SetCurrRawNU(POINT nPoint)
		{
			this->curr = nPoint;
			this->prev = nPoint;
			this->delta.x = this->delta.y = 0;
			SetPhysicalCursorPos(prev.x, prev.y);
		}
		//Set the Mouse Cursor Position in Screen Space, NU == No Update
		inline	void	SetCurrScreenNU(POINT nPoint, HWND* hWnd = nullptr)
		{
			POINT temp = nPoint;
			ClientToScreen(*hWnd, &temp);
			SetCurrRawNU(temp);
		}
		//Set the Mouse Cursor Position in Screen Space with floats, NU == No Update
		inline	void	SetCurrScreenPercentNU(Type2<float> nPoint, HWND* hWnd = nullptr)
		{
			RECT client;
			GetClientRect(*hWnd, &client);
			POINT temp;
			temp.x = (long)(nPoint.x * (client.right - client.left));
			temp.y = (long)(nPoint.y * (client.bottom - client.top));
			SetCurrScreenNU(temp, hWnd);
		}

	};


	//----------------------------------------------------------
	//                 Joystick/Controller
	//----------------------------------------------------------
	template<typename DZONE_TYPE>
	class _DeadZone
	{
	private:
		DZONE_TYPE roof;	//Max value that can be filtered, used to filter the value between 0.0f and 1.0f
		DZONE_TYPE floor;	//Min value that will be considered valid, values below this will be ignored

	public:
		_DeadZone()
		{
			roof = 0;
			floor = 0;
		}
		_DeadZone(_DeadZone<DZONE_TYPE>& that)
		{
			*this = that;
		}
		_DeadZone<DZONE_TYPE>& operator= (const _DeadZone<DZONE_TYPE>& that)
		{
			this->roof = that.roof;
			this->floor = that.floor;
			return *this;
		}

		//nRoof will be clamped to be >= floor
		void SetRoof(DZONE_TYPE nRoof)
		{
			if (nRoof < this->floor)
				nRoof = this->floor;
			this->roof = nRoof;
		}
		//nFloor will be clamped between 0 and this->roof
		void SetFloor(DZONE_TYPE nFloor)
		{
			if (nFloor > this->roof)
				nFloor = this->roof;
			else if (nFloor < 0)
				nFloor = 0;
			this->floor = nFloor;
		}
		//nFloor should be less than or equal to nRoof
		void SetBoth(DZONE_TYPE nFloor, DZONE_TYPE nRoof)
		{
			this->floor = this->roof = 0;
			this->SetRoof(nRoof);
			this->SetFloor(nFloor);
		}
		//returns a float between 0.0f and 1.0f
		float GetFilteredValue(DZONE_TYPE nMag)
		{
			//If the mag is greater than the floor
			if (nMag > this->floor)
			{
				//If the mag is greater than the roof value, then set it to the roof
				if (nMag > this->roof)
					nMag = this->roof;

				//Adjust the magnitude
				nMag -= this->floor;

				//Normalize the magnitude
				float ret = nMag / float(this->roof - this->floor);

				//Return
				return ret;
			}
			else	//If mag <= floor return 0.0f
			{
				return 0.0f;
			}
		}

	};


	class _Trigger
	{
	private:
		_DeadZone<unsigned char>	dead;		//Filter for adjusting how much pressure is required to be a key press
		unsigned char				curr;		//Range of presicion for a trigger 0-255

	public:
		_Trigger()
		{
			curr = 0;
			dead.SetRoof(255);	//Range for xbox trigger
			dead.SetFloor(0);	//Default (no dead zone)
		}
		//Set the curr value
		void	SetCurr(const unsigned char nValue)
		{
			curr = nValue;
		}
		//Set the deadzone filter
		void	SetDead(unsigned char nDead)
		{
			this->dead.SetFloor(nDead);
		}
		//Get the filtered curr value
		float	GetPressure(void)
		{
			return dead.GetFilteredValue(curr);
		}

	};


	//The components of a vector - direction & magnitude
	struct VecComp
	{
		Type2<float>	dir;
		float			mag;
		VecComp()
		{
			mag = 0.0f;
		}
		VecComp(Type2<float> nDir, float nMag)
		{
			this->dir = nDir;
			this->mag = nMag;
		}
		Type2<float> GetVecComb()
		{
			return Type2<float>(this->dir.x * mag, this->dir.y * mag);
		}
	};


	class _Analog
	{
	private:
		POINT				curr;		//The x and y values along the x and y axis
		_DeadZone<float>	dead;		//Filter for the magnitude

	public:
		_Analog()
		{
			curr.x = curr.y = 0;
			//Xbox has point values between -32,768 and +32,768 (SHORT_MAX)
			dead.SetRoof(32768);			//Max analog stick magnitude
			dead.SetFloor(0.25f * 32768);	//Default (25% deadzone)
		}

		//Sets
		void SetCurr(POINT nPoint)
		{
			this->curr = nPoint;
		}
		void SetCurr(long nX, long nY)
		{
			this->curr.x = nX;
			this->curr.y = nY;
		}
		void SetDead(float nDead)
		{
			this->dead.SetFloor(nDead);
		}
		//Gets
		VecComp GetCurr(void)
		{
			VecComp ret;

			//determine magnitude
			ret.mag = (float)sqrt(this->curr.x*this->curr.x + this->curr.y*this->curr.y);

			//If 0 vector
			if (ret.mag == 0.0f)
				return ret;

			//determine direction
			ret.dir.x = this->curr.x / ret.mag;
			ret.dir.y = this->curr.y / ret.mag;

			//filter magnitude through deadzone
			ret.mag = this->dead.GetFilteredValue(ret.mag);

			//return the VecComp
			return ret;
		}

	};


	//Key array enums
	enum CTRL_KEYS {
		XB_A, XB_B, XB_X, XB_Y, XB_LBUMP, XB_RBUMP, XB_LTRIG, XB_RTRIG, XB_LTHUMB, XB_RTHUMB, XB_BACK,				/*These are XBOX 360 key versions*/
		START, d_UP, d_DOWN, d_LEFT, d_RIGHT, PLUGIN, CTRL_KEY_COUNT,												/*These are shared between XBOX and PS3 keys*/
		PS_CROSS = XB_A, PS_CIRCLE, PS_SQUARE, PS_TRIANGLE, PS_L1, PS_R1, PS_L2, PS_R2, PS_L3, PS_R3, PS_SELECT
	};	/*These are PS3 key versions*/


	//Controller
	class _Controller
	{
	private:
		_Key		key[CTRL_KEY_COUNT];	//Array of keys
		_Analog		lLog;					//Left Analog Stick
		_Analog		rLog;					//Right Analog Stick
		_Trigger	lTrig;					//Left Trigger
		_Trigger	rTrig;					//Right Trigger

	public:
		_Controller()
		{
			this->lTrig.SetDead(XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
			this->rTrig.SetDead(XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
			this->lLog.SetDead(XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
			this->rLog.SetDead(XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
		}
		_Controller(_Controller& that)
		{
			*this = that;
		}
		_Controller& operator= (const _Controller& that)
		{
			for (int i = 0; i < CTRL_KEY_COUNT; i++)
				this->key[i] = that.key[i];
			this->lLog = that.lLog;
			this->rLog = that.rLog;
			this->lTrig = that.lTrig;
			this->rTrig = that.rTrig;
			return *this;
		}
		void Reset(void)
		{
			for (int i = 0; i < CTRL_KEY_COUNT; i++)
				key[i] = _Key();
			lLog = _Analog();
			rLog = _Analog();
			lTrig = _Trigger();
			rTrig = _Trigger();
		}

		//Updates
		void Next(XINPUT_STATE xState)
		{
			//D-pad
			this->key[d_UP].Next((xState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) != 0);
			this->key[d_DOWN].Next((xState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) != 0);
			this->key[d_LEFT].Next((xState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) != 0);
			this->key[d_RIGHT].Next((xState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) != 0);
			//Face						
			this->key[XB_A].Next((xState.Gamepad.wButtons & XINPUT_GAMEPAD_A) != 0);
			this->key[XB_B].Next((xState.Gamepad.wButtons & XINPUT_GAMEPAD_B) != 0);
			this->key[XB_X].Next((xState.Gamepad.wButtons & XINPUT_GAMEPAD_X) != 0);
			this->key[XB_Y].Next((xState.Gamepad.wButtons & XINPUT_GAMEPAD_Y) != 0);
			//Start & Select			
			this->key[START].Next((xState.Gamepad.wButtons & XINPUT_GAMEPAD_START) != 0);
			this->key[XB_BACK].Next((xState.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) != 0);
			//Left & Right Bumper		
			this->key[XB_LBUMP].Next((xState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) != 0);
			this->key[XB_RBUMP].Next((xState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0);
			//Left & Right Thumb		
			this->key[XB_LTHUMB].Next((xState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) != 0);
			this->key[XB_RTHUMB].Next((xState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) != 0);

			//Plugged In
			this->key[PLUGIN].Next((xState.dwPacketNumber != 0));

			//Triggers
			this->lTrig.SetCurr(xState.Gamepad.bLeftTrigger);
			this->key[XB_LTRIG].Next(this->lTrig.GetPressure() != 0.0f);
			this->rTrig.SetCurr(xState.Gamepad.bRightTrigger);
			this->key[XB_RTRIG].Next(this->rTrig.GetPressure() != 0.0f);

			//Analog Sticks
			this->lLog.SetCurr(xState.Gamepad.sThumbLX, xState.Gamepad.sThumbLY);
			this->rLog.SetCurr(xState.Gamepad.sThumbRX, xState.Gamepad.sThumbRY);

		}
		void Flush(double delta)
		{
			for (int i = 0; i < CTRL_KEY_COUNT; i++)
				key[i].Flush(delta);
		}

		//Sets
		void SetTrigDeadZone(const unsigned char c /*CTRL_KEYS left or right trigger*/, float nDead = 0.0f)
		{
			if (nDead < 0.0f)
				nDead = 0.0f;
			else if (nDead > 1.0f)
				nDead = 1.0f;

			switch (c)
			{
			case CTRL_KEYS::XB_LTRIG:
				this->lTrig.SetDead((unsigned char)(nDead * 255));
			case CTRL_KEYS::XB_RTRIG:
				this->rTrig.SetDead((unsigned char)(nDead * 255));
			}
		}
		void SetStickDeadZone(const unsigned char c /*CTRL_KEYS left or right stick*/, float nDead = 0.0f)
		{
			if (nDead < 0.0f)
				nDead = 0.0f;
			else if (nDead > 1.0f)
				nDead = 1.0f;
			switch (c)
			{
			case CTRL_KEYS::XB_LTHUMB:
				this->lLog.SetDead(nDead);
			case CTRL_KEYS::XB_RTHUMB:
				this->rLog.SetDead(nDead);
			}
		}

		//Gets
		_Key*		GetKey(const unsigned char c)	{ return &this->key[c]; }
		float		GetTrigger(const unsigned char c /*CTRL_KEYS left or right trigger*/)
		{
			switch (c)
			{
			case XB_LTRIG:
				return this->lTrig.GetPressure();
			case XB_RTRIG:
				return this->rTrig.GetPressure();
			default:
				return -1.0f;
			}
		}
		VecComp		GetStick(const unsigned char c /*CTRL_KEYS left or right stick*/)
		{
			switch (c)
			{
			case CTRL_KEYS::XB_LTHUMB:
				return this->lLog.GetCurr();
			case CTRL_KEYS::XB_RTHUMB:
				return this->rLog.GetCurr();
			default:
				return VecComp();
			}
		}

	};
}