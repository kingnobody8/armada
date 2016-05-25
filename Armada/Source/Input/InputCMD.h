/*
Author:			Daniel Habig
Date:			11-9-13
File:			InputCMD.h
Purpose:		Input interface
*/
#pragma once
#include "Input.h"
#include <mutex>
#include <string>

//Forward Declaration
namespace Sprite{ class CSprite; }

namespace Input
{
	//Moving 'MAX_CTRL' in the enum will speed up the controller update if you know for sure the max number of controllers that will be used
	enum eCTRL		{ CTRL_1, MAX_CTRL, CTRL_2, CTRL_3, CTRL_4 };
	enum eInputType	{ DEFAULT, MOUSE, CTRL_KEYBOARD, NUM_INPUT_TYPES};
#define MARKER_RATE	0.1f	//How fast will the text marker move
#define TEXT_RATE	0.15f	//How fast will the text held keys make letters
#define WAIT_TIME   0.3f	//How long until text rate kicks in
#define VK_ANYKEY	0xE8	//Unassigned Virtual Key Code
const Time kCtrlStickRate = 0.25L; //How fast does using a stick for selecting a button move


	class CInput
	{
	private:	//Data
		//Window state
		HWND*				m_pHWND;
		_Key				m_tFocus;

		//Input Devices
		_Key				m_vKeyState[256];	//Keyboard State		(Uses Virtual Key Codes)
		schar				m_scWheel;			//Mouse Wheel			(Value is magnitude in direction of scroll)
		_Cursor				m_cCursor;			//Mouse Cursor			(Has various functionality with cursor position, movement, and visibility)
		_Controller			m_vCtrl[MAX_CTRL];	//Controllers			(Access to all buttons, triggers and analog sticks with deadzone filters. Also vibration[Type2<float>, x == left, y == right])
		Xbox				m_vXbox[MAX_CTRL];	//xbox controllers		(Used for updating ctrl)

		//Keyboard to Text
		std::string			m_strText;
		Time				m_dTextTimer;
		Time				m_dMarkerTimer;
		slong				m_slTextMarker;
		bool				m_bTextMode;

		//Cursor Locking
		bool				m_bCursorLockOverride;

		//Input Type
		eInputType			m_eInputType;

		//Locks
		std::mutex			m_yFocus;
		std::mutex			m_yKeyState;
		std::mutex			m_yWheel;
		std::mutex			m_yCursor;
		std::mutex			m_yCtrl;
		std::mutex			m_yText;

		//SDL
		Sprite::CSprite*	m_pSprite;

	private:	//Methods
		void	SetFocus	( const bool bFocus, const Time dDelta );
		void	SetKeyState	( const Time dDelta );
		void	SetCursor	( void );
		void	SetCtrl		( const Time dDelta );
		void	SetText		( const Time dDelta );

	public:
		CInput(void);
		~CInput(void);

		//Important Funcs
		void	Start		( HWND* const  pHWND, Sprite::CSprite* const pSprite);
		void	Update		( const Time dDelta );
		void	Reset		( bool bKeepOldCursor = false, bool bKeepTextMode = false );

#pragma region KEY
		bool	Key_Held	( const uchar ucKey );
		bool	Key_Push	( const uchar ucKey );
		bool	Key_Pull	( const uchar ucKey );
		Time	Key_Time	( const uchar ucKey );
#pragma endregion
#pragma region WHEEL
		void	Wheel_Set	( const schar scDir );
		schar	Wheel_Get	( void );
#pragma endregion
#pragma region CURSOR
		void 			Cursor_Set_PosRaw		( const Type2<slong> tPoint );
		void 			Cursor_Set_PosScreen	( const Type2<slong> tPoint );
		void 			Cursor_Set_PosFloat		( const Type2<float> tPoint );
		void 			Cursor_Set_Visible		( const bool bVisible );
		void 			Cursor_Set_Clipped		( const bool bClipped );
		void 			Cursor_Set_Moveable		( const bool bMoveable );
		Type2<slong>	Cursor_Get_PosRaw		( void );
		Type2<slong>	Cursor_Get_PosScreen	( void );
		Type2<float>	Cursor_Get_PosFloat		( void );
		Type2<slong>	Cursor_Get_DeltaRaw		( void );
		Type2<float>	Cursor_Get_DeltaFloat	( void );
		bool 			Cursor_Get_Visible		( void );
		bool 			Cursor_Get_Clipped		( void );
		bool 			Cursor_Get_Moveable		( void );

		//Special
		Type2<slong>	Cursor_SDL				( void );
		void			Cursor_Lock				( const bool bLock );
		void			Cursor_Set_Override		( const bool bOverride );
		bool			Cursor_Get_Override		( void );
#pragma endregion
#pragma region CONTROLLER
		void			Ctrl_Set_Trig_Dead		( const float nDead, const uchar nButton, const uchar nCtrl_Num = eCTRL::CTRL_1 );
		void			Ctrl_Set_Stick_Dead		( const float nDead, const uchar nButton, const uchar nCtrl_Num = eCTRL::CTRL_1 );
		void			Ctrl_Set_Vibe			( const Type2<float> nVibe, const uchar nCtrl_Num = eCTRL::CTRL_1 );
		Type2<float>	Ctrl_Vibe				( const uchar nCtrl_Num = eCTRL::CTRL_1 );
		bool			Ctrl_Held				( const uchar nButton, const uchar nCtrl_Num = eCTRL::CTRL_1);
		bool			Ctrl_Push				( const uchar nButton, const uchar nCtrl_Num = eCTRL::CTRL_1);
		bool			Ctrl_Pull				( const uchar nButton, const uchar nCtrl_Num = eCTRL::CTRL_1);
		double			Ctrl_Time				( const uchar nButton, const uchar nCtrl_Num = eCTRL::CTRL_1);
		float			Ctrl_Trig				( const uchar nButton, const uchar nCtrl_Num = eCTRL::CTRL_1);
		VecComp			Ctrl_Stick				( const uchar nButton, const uchar nCtrl_Num = eCTRL::CTRL_1);
#pragma endregion
#pragma region TEXT
		void			Text_Set_Mode			( const bool bMode );
		bool			Text_Get_Mode			( void );
		void			Text_Set_String			( const char* strText );
		std::string		Text_Get_String			( void );
		void			Text_Set_Marker			( const slong slMarker );
		slong			Text_Get_Marker			( void );
#pragma endregion
#pragma region SPECIAL
		//Window
		_Key			GetFocus				( void );
		//Input Type
		void			SetInputType			( const eInputType eType );
		eInputType		GetInputType			( void );
		eInputType		GetFinalInputType		( void );
#pragma endregion
	};
}