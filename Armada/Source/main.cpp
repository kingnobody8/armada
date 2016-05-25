/*
Author:			Daniel Habig
Date:			11-9-13
File:			main.cpp
Purpose:		Setup for an SDL application.
*/
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "State_Machine\Machine.h"
#include <time.h>
//#include <vld.h>

//Debug
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

/*------------------------------------------------------------------*/
/*					Global Data										*/
/*------------------------------------------------------------------*/
const char*	win_title				= "Armada";
const LPCWSTR win_name				= L"Armada";


/*------------------------------------------------------------------*/
/*					Functions										*/
/*------------------------------------------------------------------*/
BOOL PrevApp( void )
{
	//	Find a window of the same window class name and window title
	/*HWND hApp = FindWindow(NULL, "Window Title");*/
	HWND hOtherWnd = FindWindow( NULL, win_name );

	//	If one was found
	if( hOtherWnd )
	{
		//	If it was minimized
		if( IsIconic( hOtherWnd ) == TRUE )
			//	restore it
				ShowWindow( hOtherWnd, SW_RESTORE );

		//	Bring it to the front
		SetForegroundWindow( hOtherWnd );

		return TRUE;
	}

	//	No other copies found running
	return FALSE;
}
int main(int argc, char** argv)
{
	//Seed the random
	time_t curr_time;
	time(&curr_time);
	srand((unsigned int)(curr_time));

	//Leak Detection
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(153);

	//Check for another currently running app
	//if (PrevApp() == TRUE)
		//return -1;

	//Create
	CMachine mach;

	//Initialize
	mach.Start( win_title, win_name );

	//Loop
	while( mach.Loop() );

	//Cleanup
	mach.End();

	//End
	return 0;
}
