/*
Author:			Daniel Habig
Date:			2 - 11 - 14
File:			Network.h
Purpose:		Includes for both clients and servers,
contains Network Message structure,
contains enum of message types,
as well as #defines needed by the network system
*/
#pragma once
#include "../Util/Types.h"
#include "../Input/Input.h"
#include <RakNetTypes.h>
using namespace Type;

//Network Defines
#define SERVER_PORT			24842
#define LOST_TIME			2000

//Debug Printing
#define DB_NET_MSG_CNT		1	//Connecting, Disconnecting, and Reconnecting
#define DB_NET_MSG_SR		1	//Message Sending and Receiving

namespace Network
{
	static const char* kstrRakNetServerAddress	= "natpunch.jenkinssoftware.com";
	static const ushort kusRakNetServerPort		= 61111;

	struct TPeer
	{
		RakNet::RakNetGUID		m_Guid;
		RakNet::SystemAddress	m_Addr;
		schar					m_scID;
		Input::_Key				m_tState;
		TPeer()
		{
			ZeroMemory(this, sizeof(TPeer));
			m_scID = -1;
		}
	};

	//For sorting by GUID
	inline bool operator==(const TPeer& lhs, const TPeer& rhs)
	{
		if (lhs.m_Guid != rhs.m_Guid)
			return false;
		if (lhs.m_Addr != rhs.m_Addr)
			return false;
		if (lhs.m_scID != rhs.m_scID)
			return false;
		return true;
	}
	inline bool operator!=(const TPeer& lhs, const TPeer& rhs){ return !operator==(lhs, rhs); }
	inline bool operator< (const TPeer& lhs, const TPeer& rhs){ return lhs.m_Guid < rhs.m_Guid; }
	inline bool operator> (const TPeer& lhs, const TPeer& rhs){ return  operator< (rhs, lhs); }
	inline bool operator<=(const TPeer& lhs, const TPeer& rhs){ return !operator> (lhs, rhs); }
	inline bool operator>=(const TPeer& lhs, const TPeer& rhs){ return !operator< (lhs, rhs); }

	//Special for ordering clients by slot id, rather than guid
	struct TPeerAltOrder
	{
		bool operator() (const TPeer& lhs, const TPeer& rhs)
		{
			return lhs.m_scID < rhs.m_scID;
		}
	};

}