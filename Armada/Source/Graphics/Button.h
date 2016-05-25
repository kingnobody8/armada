#pragma once
#include <string>
#include "../Util/Types.h"
using namespace Type;
#include "../Text/Text.h"
#include "../Input/Input.h"

//Forward Declare
namespace Sprite { class CSprite; }
namespace Audio { class CAudio; }

namespace GUI
{
	class CButton
	{
	public:		//Enum
		enum EButtonType	{ BUTTON, OUTLINE, RADIO, NUM_BUTTON_TYPES };
		enum EButtonState	{ DEFAULT, HOVER, MDOWN, UNUSED_0, UNUSED_1, UNUSED_2, INACTIVE, NUM_STATES, ON = DEFAULT, ON_HOVER, ON_MDOWN, OFF, OFF_HOVER, OFF_MDOWN }; //All states after NUM_STATES are used for radio button
		enum EMoveType		{ PREVIOUS, CLOSEST, RESET, NUM_MOVE_TYPES };
		enum EButtonSfx		{ SFX_PUSH, SFX_PULL, SFX_HOVER, SFX_BAD, NUM_SFX };

	private:	//Data
	protected:	//Data
		bool					m_bActive;
		EButtonState			m_eState;
		_RECT<slong>			m_tRect;
		Input::_Key				m_tKey;

		//Color
		_COLOR<uchar>			m_vClrText[NUM_STATES];
		_COLOR<uchar>			m_vClrBG[NUM_STATES];
		_COLOR<uchar>			m_vClrOutline[NUM_STATES];
		slong					m_slOutlineSize[NUM_STATES];

		//Text
		std::string				m_strText;
		Text::EFontAlignment	m_eAlignment;
		float					m_fTextScale;
		Type2<slong>			m_tOffset;

		//Sfx
		slong					m_vSfx[NUM_SFX];
		EButtonState			m_ePrevState;
		bool					m_bMouseSelection;	//Used to tell whether or not to call hover w/ override

		//Button Group
		slong					m_vButtonGroupDir[NUM_CARDINAL_DIRECTIONS];


	private:	//Methods
	protected:	//Methods
	public:		//Methods
		CButton(void);
		~CButton(void);


		//Determines if the button is being 'hovered' over using a mouse
		virtual void Selection		(Type2<slong> tCursor);
		//Determine if the button is being 'hovered' over using a keyboard or controller
		virtual void Selection(bool bSelected);
		//Determines if the button is being pressed
		virtual Input::_Key Confirmation(bool bDown);
		virtual void Update(Time dDelta, Audio::CAudio* pAudio );
		virtual void Render(Sprite::CSprite* pSprite, Text::CText* pText);

		//Sets all colors in every state to this color
		virtual void DefaultColor(_COLOR<uchar> clr);

		//Gets
		inline bool						GetActive			( void )												const	{ return this->m_bActive; }
		inline EButtonState				GetState			( void )												const	{ return (this->m_bActive) ? this->m_eState : INACTIVE; }
		inline EButtonState				GetPrevState		( void )												const	{ return this->m_ePrevState; }
		inline _RECT<slong>				GetRect				( void )												const	{ return this->m_tRect; }
		inline Input::_Key				GetClick			( void )												const	{ return this->m_tKey; }
		inline _COLOR<uchar>			GetTextColor		( const EButtonState eState )							const	{ return this->m_vClrText[eState]; }
		inline _COLOR<uchar>			GetBGColor			( const EButtonState eState )							const	{ return this->m_vClrBG[eState]; }
		inline std::string				GetText				( void )												const	{ return this->m_strText; }
		inline Text::EFontAlignment		GetTextAlignment	( void )												const	{ return this->m_eAlignment; }
		inline float					GetTextScale		( void )												const	{ return this->m_fTextScale; }
		inline Type2<slong>				GetTextOffset		( void )												const	{ return this->m_tOffset; }
		inline slong					GetOutlineSize		( const EButtonState eState )							const	{ return this->m_slOutlineSize[eState]; }
		inline _COLOR<uchar>			GetOutlineColor		( const EButtonState eState )							const	{ return this->m_vClrOutline[eState]; }
		inline slong					GetSfx				( const EButtonSfx eSfx )								const	{ return this->m_vSfx[eSfx]; }
		inline slong					GetButtonGroupDir	( const eDIRECTION eDir )								const	{ return this->m_vButtonGroupDir[eDir]; }

		//Sets
		inline void						SetActive			( const bool bActive )											{ this->m_bActive = bActive; }
		inline void						SetState			( const EButtonState eState )									{ this->m_eState = eState; }
		inline void						SetPrevState		( const EButtonState eState )									{ this->m_ePrevState = eState; }
		inline void						SetRect				( const _RECT<slong> tRect )									{ this->m_tRect = tRect; }
		inline void						SetClick			( const Input::_Key tKey )										{ this->m_tKey = tKey; }
		inline void						SetTextColor		( const _COLOR<uchar> tClrText, const EButtonState eState )		{ this->m_vClrText[eState] = tClrText; }
		inline void						SetBGColor			( const _COLOR<uchar> clr, const EButtonState eState )			{ this->m_vClrBG[eState] = clr; }
		inline void						SetText				( const std::string strText )									{ this->m_strText = strText; }
		inline void						SetTextAlignment	( const Text::EFontAlignment eAlignment )						{ this->m_eAlignment = eAlignment; }
		inline void						SetTextScale		( const float fTextScale )										{ this->m_fTextScale = fTextScale; }
		inline void						SetTextOffset		( const Type2<slong> tOffset )									{ this->m_tOffset = tOffset; }
		inline void						SetOutlineSize		( const slong slOutlineSize, const EButtonState eState )		{ this->m_slOutlineSize[eState] = slOutlineSize; }
		inline void						SetOutlineColor		( const _COLOR<uchar> clr, const EButtonState eState )			{ this->m_vClrOutline[eState] = clr; }
		inline void						SetSfx				( const slong slSfx, const EButtonSfx eSfx )					{ this->m_vSfx[eSfx] = slSfx; }
		//slGroup == index of the group you wish to move to, -1 == don't override, -2 == don't move at all
		inline void						SetButtonGroupDir	( const slong slGroup, const eDIRECTION eDir )					{ this->m_vButtonGroupDir[eDir] = slGroup; }
	};
}