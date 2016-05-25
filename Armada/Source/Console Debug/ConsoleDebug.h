#pragma once

class CConsoleDebug
{
private:	//Data
	bool	m_bActive;


private:	//Methods
	void	Create			( void );
	void	Close			( void );
public:
	CConsoleDebug			( void );				//Creates a Console Window
	~CConsoleDebug			( void );				//Closes the Console Window

	//Interface
	void	PrintOut		( const char* text );	//Prints the text to the console screen (adds '\n')
	void	ClearConsole	( void );

	//Mutators
	void	SetActive		( bool b );
	void	ToggleActive	( void );

	//Accessors
	bool	GetActive		( void )	const	{ return this->m_bActive; }
};
