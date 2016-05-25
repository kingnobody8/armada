#ifndef EVTMGR_H
#define EVTMGR_H

#include <iostream>
#include <string>
#include <map>
#include "Delegate.h"
#include "EventData.h"
#include "Trackable.h"

using namespace std;

class EventManager : public Trackable
{
public:
	EventManager(void); // hide this guy because it's a singleton
	~EventManager(void);

	/*     Pre: A string type key, and a delegate pointer
	 *    Post: A handler is established for the type key
	 * Purpose: To create a handler under string type with a reference
	 *			to delegate
	 *****************************************************************/
	void addHandler(string type, Delegate* dele);

	/*     Pre: A type identifier, and a corresponding eventdata input
	 *    Post: The delegate is invoked that is related to the type
	 *			identifier
	 * Purpose: To fire the event associated with the type identifier
	 *****************************************************************/
	void fireEvent(string type, EventData* input);

private:
	int mNextID;
	map<string, Delegate*> eventData;

};

extern EventManager* gpEventManager;

#endif