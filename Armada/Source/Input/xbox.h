/*
Author:			Daniel Habig
Date:			11-9-13
File:			xbox.h
Purpose:		Used to interface with wired xbox controllers
*/
#pragma once
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <XInput.h>

//#pragma comment(lib, "XInput.lib")		//Win8
#pragma comment(lib, "XInput9_1_0.lib")		//Win7

namespace Input
{
	// XBOX Controller
	class Xbox
	{
	private:
		XINPUT_STATE		_controllerState;
		int					_controllerNum;
		unsigned short int _vibeLeft;
		unsigned short int _vibeRight;

	public:
		Xbox();
		Xbox(int playerNumber);
		XINPUT_STATE	GetState(void);
		bool			IsConnected(void);
		void			Vibrate(int leftVal = 0, int rightVal = 0);
		void			Vibrate(float leftVal = 0.0f, float rightVal = 0.0f);
		inline float	GetLeftVibe(void)	const	{ return this->_vibeLeft / 65535.0f; }
		inline float	GetRightVibe(void)	const	{ return this->_vibeRight / 65535.0f; }

	};
}