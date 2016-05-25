#include "EntityManager.h"
#include "../Camera/Camera.h"
#include "../SDL/Sprite.h"
#include "Entity.h"

CEntityManager* CEntityManager::instance = nullptr;
CEntityManager* CEntityManager::GetInstance(void)
{
	if( !instance )
		instance = new CEntityManager();
	return instance;
}
void CEntityManager::DeleteInstance(void)
{
	if (instance)
	{
		delete instance;
		instance = nullptr;
	}
}
CEntityManager::CEntityManager(void)
{
}
CEntityManager::~CEntityManager(void)
{
	//Clear Moving List
	this->ClearList();
}

void CEntityManager::Update(double dDelta)
{
	//Remove
	unsigned int size = this->m_vRemovals.size();
	for( u32 i = 0; i < size; i++ )
	{
		for( unsigned int j = 0; j < this->m_vEntities.size(); j++ )
		{
			if( this->m_vRemovals[i] == this->m_vEntities[j] )
			{
				this->m_vEntities.erase( this->m_vEntities.begin() + j );
				break;
			}
		}
		delete this->m_vRemovals[i];
	}
	this->m_vRemovals.clear();

	//Update
	size = this->m_vEntities.size();
	for( u32 i = 0; i < size; i++ )
		this->m_vEntities[i]->Update(dDelta);
}
void CEntityManager::Render(void)
{
	//tAABB view = Camera::GetInstance()->GetView();
	//tAABB sect;
	//tAABB rect;
	//If moving list is on screen
	unsigned int size = this->m_vEntities.size();
	for( unsigned int i = 0; i < size; i++ )
	{
		//this->m_vEntities[i]->Render(view.low);
		//rect = GetAABBFromPoint( this->m_vMoving[i]->GetAABB(), this->m_vMoving[i]->;
		//if( Intersect_AABB_AABB( view, this->m_vMoving[i]->GetAABB(), sect ) )
		//this->m_vMoving[i]->Render(view.low);
	}
}

void CEntityManager::ClearList()
{
	unsigned int size = this->m_vEntities.size();
	for( unsigned int i = 0; i < size; i++ )
		delete this->m_vEntities[i];

	this->m_vEntities.clear(); 
}
void CEntityManager::RemoveEntity(CEntity* pEnt)
{
	m_vRemovals.push_back( pEnt );
}