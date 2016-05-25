#ifndef DELEGATE_H
#define DELEGATE_H

#include <iostream>
#include <string>
#include <cstring>
#include "EventData.h"

using namespace std;

class Delegate : public Trackable
{
	//Create some typedefs to manage references easier
	typedef void *InstancePtr;
	typedef void (*InternalFunction)(InstancePtr, EventData*); //This holds an internal function reference

public:

	/*     Pre: A function and/or a reference to the object
	 *    Post: A delegate binded to function is returned
	 * Purpose: To create a delegate bound to a function
	 ***********************************************************/
	template <void (*Function)(EventData*)>
	static Delegate* create(void)
    {
		Delegate* del = new Delegate();
		del->Bind<Function>();
        return del;
    }
	template <class C, void (C::*Function)(EventData*)>
	static Delegate* create(C* obj)
    {
		Delegate* del = new Delegate();
		del->Bind<C, Function>(obj);
        return del;
    }

	/*     Pre: A function and/or a reference to the object
	 *    Post: A delegate binded to function is returned
	 * Purpose: To create a delegate bound to a function
	 ***********************************************************/
	template <void (*Function)(EventData*)>
	void Bind(void)
	{
		mInst = NULL;
		mInternal = &Method<Function>;
	}
	template <class C, void (C::*Function)(EventData*)>
	void Bind(C* obj)
	{
		mInst = obj;
		mInternal = &ClassMethod<C, Function>;
	}

	/*     Pre: This object exists and is bound to a function
	 *    Post: The function is fired
	 * Purpose: To create a delegate bound to a function
	 ***********************************************************/
	void Invoke(EventData* ARG0)
	{
		return mInternal(mInst, ARG0);
	}

	/*     Pre: Functions, event data, or pointers
	 *    Post: The function is executed
	 * Purpose: To correctly fire a function based on whether
	 *			it's a void* function of a member function
	 ***********************************************************/
	template <void (*Function)(EventData*)>
	static void Method(InstancePtr, EventData* ARG0)
	{
		return (Function)(ARG0);
	}
	template <class C, void (C::*Function)(EventData*)>
	static void ClassMethod(InstancePtr ptr, EventData* ARG0)
	{
		return (static_cast<C*>(ptr)->*Function)(ARG0);
	}

private:

	Delegate(void) {}

	InternalFunction mInternal;
	InstancePtr mInst;

};

#endif