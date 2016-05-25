#pragma once
#include "../Util/Defines.h"
#include "../Util/Types.h"
#include "../Util/Collision.h"
#include "../Camera/Camera.h"
#include "../SDL/Sprite.h"

enum eENTITYTYPE { ENTITY, JUMPER, PLAYER, PROJECTILE, LARK };
class CTile;
class CEntityManager;

class CEntity
{
private:		//Data
protected:		//Data
	//tAABB		m_tRect;
	eENTITYTYPE	m_eEntityType;
	Camera*		cam;
	//Sprite*		sprite;
	CEntityManager* m_pEM;
public:			//Data
	Type2<float>		next_pos;
	Type2<float>		pos;
	Type2<float>		vel;
	Type2<float>		acc;

private:		//Methods
protected:		//Methods
public:			//Methods
	CEntity(void) { m_eEntityType = ENTITY; cam = Camera::GetInstance(); /*sprite = Sprite::GetInstance();*/ m_pEM = NULL; }
	~CEntity(void) {};

	virtual void Update(double dDelta) = 0;
	virtual void Render(Type2<float> pixel_offset) = 0;

	inline void Flush(void) { this->pos = this->next_pos; }
	//inline tAABB GetAABB(void) { return this->m_tRect; }
	inline eENTITYTYPE GetType(void) { return this->m_eEntityType; }
};
