#pragma once
#include "../Util/Types.h"
#include "../Util/Collision.h"
using namespace Collision;

class Camera
{
private:	//Data
	//Instance
	static Camera* instance;
	Camera(void);
	~Camera(void);
	tAABB<float>			view;

public:		//Methods
	//Instance
	static Camera* GetInstance(void);
	static void DeleteInstance(void);

	void Center(Type2<float> pos);
	void Origin(void);
	inline tAABB<float> GetView(void) { return this->view; }
	inline Type2<float> GetOffset(void) { return this->view.low; }
};