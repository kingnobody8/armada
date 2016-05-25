/*
Author:			Daniel Habig
Date:			11-9-13
File:			xbox.cpp
Purpose:		Used to interface with wired xbox controllers
*/
#include "xbox.h"

namespace Input
{
	Xbox::Xbox()
	{
		this->_controllerNum = -1;
		this->_vibeLeft = this->_vibeRight = 0;
	}
	Xbox::Xbox(int playerNumber)
	{
		// Set the Controller Number
		_controllerNum = playerNumber - 1;
		// Set the vibration levels
		_vibeLeft = _vibeRight = 0;
	}

	XINPUT_STATE Xbox::GetState()
	{
		// Zeroise the state
		ZeroMemory(&_controllerState, sizeof(XINPUT_STATE));

		// Get the state
		DWORD Result = XInputGetState(_controllerNum, &_controllerState);

		//HACK:
		//the dwPacketNumber member of XINPUT_STATE is not used for my purposes, so I will use it to set a bool whether the controller is connected
		_controllerState.dwPacketNumber = 0;
		if (Result == ERROR_SUCCESS)
			_controllerState.dwPacketNumber = 1;

		return _controllerState;
	}

	bool Xbox::IsConnected()
	{
		// Zeroise the state
		ZeroMemory(&_controllerState, sizeof(XINPUT_STATE));

		// Get the state
		DWORD Result = XInputGetState(_controllerNum, &_controllerState);

		if (Result == ERROR_SUCCESS)
			return true;
		else
			return false;
	}

	//If value < 0, the value will be left alone
	void Xbox::Vibrate(int leftVal, int rightVal)
	{
		//Clamp the value to unsigned short int max
		if (leftVal > 65535)
			leftVal = 65535;
		if (rightVal > 65535)
			rightVal = 65535;

		if (leftVal >= 0)
			_vibeLeft = leftVal;
		if (rightVal >= 0)
			_vibeRight = rightVal;

		// Create a Vibraton State
		XINPUT_VIBRATION Vibration;

		// Zeroise the Vibration
		ZeroMemory(&Vibration, sizeof(XINPUT_VIBRATION));

		// Set the Vibration Values
		Vibration.wLeftMotorSpeed = _vibeLeft;
		Vibration.wRightMotorSpeed = _vibeRight;

		// Vibrate the controller
		XInputSetState(_controllerNum, &Vibration);

	}
	void Xbox::Vibrate(float leftVal, float rightVal)
	{
		int left = (int)(leftVal * 65535);
		int right = (int)(rightVal * 65535);
		this->Vibrate(left, right);
	}
}