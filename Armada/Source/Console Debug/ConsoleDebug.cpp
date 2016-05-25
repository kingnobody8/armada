#include "ConsoleDebug.h"
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <Windows.h>

CConsoleDebug::CConsoleDebug()
{
	this->m_bActive = false;	
}
CConsoleDebug::~CConsoleDebug()
{
}

void CConsoleDebug::Create(void)
{
	AllocConsole();

	HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
	int hCrt = _open_osfhandle((long)handle_out, _O_TEXT);
	FILE* hf_out = _fdopen(hCrt, "w");
	setvbuf(hf_out, NULL, _IONBF, 1);
	*stdout = *hf_out;

	HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
	hCrt = _open_osfhandle((long)handle_in, _O_TEXT);
	FILE* hf_in = _fdopen(hCrt, "r");
	setvbuf(hf_in, NULL, _IONBF, 128);
	*stdin = *hf_in;

	this->m_bActive = true;
}
void CConsoleDebug::Close(void)
{
	FreeConsole();
	this->m_bActive = false;
}

//Interface
void CConsoleDebug::PrintOut(const char* text)
{
	printf(text);
}
void CConsoleDebug::ClearConsole(void)
{
	system("cls");
}

//Mutators
void CConsoleDebug::SetActive(bool b)
{
	if (m_bActive)
		this->Close();
	else
		this->Create();
	this->m_bActive = b;
}
void CConsoleDebug::ToggleActive(void)
{
	this->SetActive(!this->m_bActive);
}
