#include "ListBox.h"

namespace GUI
{
	CListBox::CListBox(void)
	{
		this->m_bActive = true;
		this->m_slSelectedIndex = this->m_slDisplayedIndices = this->m_slOffsetIndex = -1;
		this->m_eAlignment = Text::EFontAlignment::F_LEFT;
		this->m_fTextScale = 1.0f;
	}
	CListBox::~CListBox(void)
	{

	}

	void CListBox::Inc(void)
	{

	}
	void CListBox::Dec(void)
	{

	}
	void CListBox::Inc_Big(void)
	{

	}
	void CListBox::Dec_Big(void)
	{
		
	}
	void CListBox::Update(Time dDelta)
	{

	}
	void CListBox::Render(Sprite::CSprite* pSprite, Text::CText* pText)
	{

	}

}