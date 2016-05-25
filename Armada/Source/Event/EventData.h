#ifndef EVTDTA_H
#define EVTDTA_H

#include <iostream>
#include "Trackable.h"

using namespace std;

class EventData : public Trackable
{
private:
	int mData;

public:
	EventData(void) { mData = 0; }

	EventData(int data)
	{
		mData = data;
	}

	inline int GetData() { return mData; };
};

#endif