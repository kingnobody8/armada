#pragma once
#include "../Util/Types.h"
using namespace Type;
#include <vector>

//Forward Declarations
namespace Sprite { class CSprite; }

class CStar
{
private:
	Type2<float> m_tPos;
	Type2<float> m_tVel;
	float		m_fSpeed;
	uchar		m_ucSize;
public:
	CStar(void)
	{
		m_fSpeed = 0.0f;
		m_ucSize = 1;
	}
	~CStar(void) {}
	void Update(Time dDelta, Type2<float> tDir,Type2<ulong> tMax)
	{
		this->m_tPos += tDir * m_fSpeed * float(dDelta);

		if ((tDir.x > 0.0f && m_tPos.x > tMax.x) || (tDir.x < 0.0f && m_tPos.x < 0.0f))
			this->m_tPos.x = tMax.x - this->m_tPos.x;
		if ((tDir.y > 0.0f && m_tPos.y > tMax.y) || (tDir.y < 0.0f && m_tPos.y < 0.0f))
			this->m_tPos.y = tMax.y - this->m_tPos.y;
	}

	//Mutators
	inline void				SetPos		( const Type2<float> tPos )	{ this->m_tPos = tPos; }
	inline void				SetVel		( const Type2<float> tVel ) { this->m_tVel = tVel; }
	inline void				SetSpeed	( const float fSpeed )		{ this->m_fSpeed = fSpeed; }
	inline void				SetSize		( const uchar ucSize )		{ this->m_ucSize = ucSize; }
	//Accessors
	inline Type2<float>		GetPos		( void )	const	{ return this->m_tPos; }
	inline Type2<float>		GetVel		( void )	const	{ return this->m_tVel; }
	inline float			GetSpeed	( void )	const	{ return this->m_fSpeed; }
	inline uchar			GetSize		( void )	const	{ return this->m_ucSize; }
};

class CStarField
{
private:
	Sprite::CSprite*	m_pSprite;
	std::vector<CStar>	m_vField;
	Type2<float>		m_tDir;
	Time		m_tTotalTime;

public:
	CStarField(void);
	~CStarField(void);
	void Enter(Sprite::CSprite* pSprite);

	void Update(Time dDelta);
	void Render(void);

};