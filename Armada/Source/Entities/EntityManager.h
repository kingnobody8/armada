#pragma once
#include "../Util/Types.h"
#include <vector>
using namespace std;

class CEntity;

class CEntityManager
{
private:
	//Instance
	static CEntityManager* instance;
	CEntityManager(void);
	~CEntityManager(void);

	vector<CEntity*>	m_vEntities;
	vector<CEntity*>	m_vRemovals;

public:
	//Instance
	static CEntityManager* GetInstance();
	static void DeleteInstance();

	void		Update(double dDelta);
	void		Render(void);

	inline void	ClearList(void);

	void		RemoveEntity(CEntity* pEnt);
};

