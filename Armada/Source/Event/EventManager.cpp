#include "EventManager.h"

EventManager::EventManager(void)
{
}

EventManager::~EventManager(void)
{
	for ( map<string, Delegate*>::iterator i = eventData.begin(); i != eventData.end(); ++i )
	{
		delete i->second;
		i->second = NULL;
	}
	eventData.clear();
}

void EventManager::addHandler(string type, Delegate* dele)
{
	eventData[type] = dele;
}

void EventManager::fireEvent(string type, EventData* input)
{
	map<string, Delegate*>::iterator iter = eventData.find( type );

	if( iter != eventData.end() )
	{
		iter->second->Invoke(input); // fire off the event function
		delete input;                // delete the inputed event data to save memory
	}
}