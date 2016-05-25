#include "Camera.h"
#include "../SDL/Sprite.h"


Camera* Camera::instance = nullptr;
Camera* Camera::GetInstance(void)
{
	if( !instance )
		instance = new Camera();
	return instance;
}
void Camera::DeleteInstance(void)
{
	if (instance)
	{
		delete instance;
		instance = nullptr;
	}
}
Camera::Camera(void)
{
	this->Origin();
}
Camera::~Camera(void)
{
}

void Camera::Center(Type2<float> pos)
{
	//view.low = pos - Type2<float>(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f);
	//view.high = view.low + Type2<float>((float)SCREEN_WIDTH, (float)SCREEN_HEIGHT);
}
void Camera::Origin(void)
{
	//view.low = Type2<float>(0, 0);
	//view.high = Type2<float>((float)SCREEN_WIDTH, (float)SCREEN_HEIGHT);
}
