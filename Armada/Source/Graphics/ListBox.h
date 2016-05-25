#pragma once
#include <string>
#include "../Util/Types.h"
using namespace Type;
#include "../Text/Text.h"

//Forward Declare
namespace Sprite { class CSprite; }

namespace GUI
{
	class CListBox
	{
	private:	//Data
		bool						m_bActive;
		_RECT<slong>				m_tRect;
		slong						m_slOffset;
		slong						m_slSelectedIndex;
		slong						m_slDisplayedIndices;
		slong						m_slOffsetIndex;

		//Text
		std::vector<std::string>	m_vList;
		Text::EFontAlignment		m_eAlignment;
		float						m_fTextScale;
		Type2<slong>				m_tTextOffset;

	private:	//Methods
	public:		//Methods
		CListBox(void);
		~CListBox(void);

		void Inc(void);
		void Dec(void);
		void Inc_Big(void);
		void Dec_Big(void);
		void Update(Time dDelta);
		void Render(Sprite::CSprite* pSprite, Text::CText* pText);


		//Gets
		inline bool						GetActive			( void )								const	{ return this->m_bActive; }
		inline _RECT<slong>				GetRect				( void )								const	{ return this->m_tRect; }
		inline std::vector<std::string>	GetList				( void )								const	{ return this->m_vList; }
		inline slong					GetSelectedIndex	( void )								const	{ return this->m_slSelectedIndex; }
		inline slong					GetDisplayedIndices	( void )								const	{ return this->m_slDisplayedIndices; }
		inline Text::EFontAlignment		GetTextAlignment	( void )								const	{ return this->m_eAlignment; }
		inline float					GetTextScale		( void )								const	{ return this->m_fTextScale; }
		inline Type2<slong>				GetTextOffset		( void )								const	{ return this->m_tTextOffset; }

		//Sets
		inline void						SetActive			( const bool bActive )							{ this->m_bActive = bActive; }
		inline void						SetRect				( const _RECT<slong> tRect )					{ this->m_tRect = tRect; }
		inline void						SetSelectedIndex	( const slong slSelectedIndex )					{ this->m_slSelectedIndex = slSelectedIndex; }
		inline void						SetDisplayedIndices	( const slong slDisplayedIndices )				{ this->m_slDisplayedIndices = slDisplayedIndices; }
		inline void						SetList				( const std::vector<std::string> vList )		{ this->m_vList = vList; }
		inline void						SetTextAlignment	( const Text::EFontAlignment eAlignment )		{ this->m_eAlignment = eAlignment; }
		inline void						SetTextScale		( const float fTextScale )						{ this->m_fTextScale = fTextScale; }
		inline void						SetTextOffset		( const Type2<slong> tOffset )					{ this->m_tTextOffset = tOffset; }
	};
}