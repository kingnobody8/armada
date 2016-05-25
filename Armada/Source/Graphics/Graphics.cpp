#include "Graphics.h"
#include "../SDL/Sprite.h"
using namespace Type;

CStarField::CStarField(void)
{
	this->m_pSprite = NULL;
	m_tTotalTime = 0.0l;
}
CStarField::~CStarField(void)
{

}
void CStarField::Enter(Sprite::CSprite* pSprite)
{
	this->m_pSprite = pSprite;

	m_tDir = Type2<float>(1.0f, 1.0f);
	m_tDir.normalize();

	ulong count = 1001;
	this->m_vField.reserve(count);
	for (ulong i = 0; i < count; i++)
	{
		CStar star;
		star.SetSize(2 + rand() % 5);
		star.SetPos(Type2<float>( (float)(rand() % Sprite::SCREEN_WIDTH), (float)(rand() % Sprite::SCREEN_HEIGHT)));
		star.SetSpeed(200.0f + 400.0f * pow( (rand() % 1000 / 1000.0f), 4));
		this->m_vField.push_back(star);
	}
}
void CStarField::Update(Time dDelta)
{
	this->m_tTotalTime += dDelta;
	this->m_tDir.x = float(sin(this->m_tTotalTime * 0.01l + 0.5l));
	this->m_tDir.y = float(cos(this->m_tTotalTime * 0.01l + 0.5l));
	this->m_tDir.normalize();

	ulong size = this->m_vField.size();
	for (ulong i = 0; i < size; i++)
		this->m_vField[i].Update(dDelta, this->m_tDir, Type2<ulong>(Sprite::SCREEN_WIDTH, Sprite::SCREEN_HEIGHT));
}
void CStarField::Render(void)
{
	ulong size = this->m_vField.size();
	for (ulong i = 0; i < size; i++)
		this->m_pSprite->DrawRect((slong)this->m_vField[i].GetPos().x, (slong)this->m_vField[i].GetPos().y, (slong)this->m_vField[i].GetSize(), (slong)this->m_vField[i].GetSize(), CLR::WHITE);
}