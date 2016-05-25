#pragma once
#include <string>
#include "RadioButton.h"

namespace GUI
{
	class CSlider : public CButton
	{
	public:
		enum EOrientation { VERT, HORZ, NUM_SLIDER_TYPES };
	private:	//Data
		_COLOR<uchar>	m_vClrSlider[NUM_STATES];
		_COLOR<uchar>	m_vClrLine[NUM_STATES];
		_COLOR<uchar>	m_vClrBar[NUM_STATES];
		slong			m_vSliderSize[NUM_STATES];
		slong			m_vLineSize[NUM_STATES];
		EOrientation	m_eOrientation;
		float			m_fPercent;
		Type2<slong>	m_tCursor;
		bool			m_bUsedMouse;

	protected:	//Data

	private:	//Methods
	protected:	//Methods
	public:		//Methods
		CSlider(void);
		~CSlider(void);


		//Determines if the button is being 'hovered' over using a mouse
		virtual void Selection(Type2<slong> tCursor);
		//Determine if the button is being 'hovered' over using a keyboard or controller
		virtual void Selection(bool bSelected);
		//Determines if the button is being pressed
		virtual Input::_Key Confirmation(bool bDown);
		virtual void Update(Time dDelta, Audio::CAudio* pAudio);
		virtual void Render(Sprite::CSprite* pSprite, Text::CText* pText);
		virtual void DefaultColor(_COLOR<uchar> clr);


		//Gets
		inline _COLOR<uchar>	GetSliderColor	( const EButtonState eState )					const	{ return this->m_vClrSlider[eState]; }
		inline _COLOR<uchar>	GetLineColor	( const EButtonState eState )					const	{ return this->m_vClrLine[eState]; }
		inline _COLOR<uchar>	GetBarColor		( const EButtonState eState )					const	{ return this->m_vClrBar[eState]; }
		inline slong			GetSliderSize	( const EButtonState eState )					const	{ return this->m_vSliderSize[eState]; }
		inline slong			GetLineSize		( const EButtonState eState )					const	{ return this->m_vLineSize[eState]; }
		inline EOrientation		GetOrientation	( void )										const	{ return this->m_eOrientation; }
		inline float			GetPercent		( void )										const	{ return this->m_fPercent; }
		//Sets
		inline void				SetSliderColor	( const _COLOR<uchar> clr, const EButtonState eState)	{ this->m_vClrSlider[eState] = clr; }
		inline void				SetLineColor	( const _COLOR<uchar> clr, const EButtonState eState)	{ this->m_vClrLine[eState] = clr; }
		inline void				SetBarColor		( const _COLOR<uchar> clr, const EButtonState eState)	{ this->m_vClrBar[eState] = clr; }
		inline void				SetSliderSize	( const EButtonState eState, const slong slSize )		{ this->m_vSliderSize[eState] = slSize; }
		inline void				SetLineSize		( const EButtonState eState, const slong slSize )		{ this->m_vLineSize[eState] = slSize; }
		inline void				SetOrientation	( const EOrientation eType )							{ this->m_eOrientation = eType; }
		inline void				SetPercent		( const float fPercent )								{ this->m_fPercent = fPercent; }
	};
}