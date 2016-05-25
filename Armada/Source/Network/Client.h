/*
Author:			Daniel Habig
Date:			2 - 11 - 14
File:			Client.h
Purpose:		Class to perform operations required of a client on a network
Sends Data to a connected server
Recvs data from the connected server
*/
#pragma once
#include "NetMsg.h"
#include <BitStream.h>
#include <PacketPriority.h>
#include <vector>

//Forward Declaration
namespace RakNet{ class NatPunchthroughClient; }

namespace Network
{

	class CClient
	{
	private:	//Data
		RakNet::RakPeerInterface*			m_pRak;
		TPeer								m_tPeer;
		TPeer								m_tServer;
		std::vector<CNetMsg>				m_vBroadcasts;
		//std::vector<TPeer>					m_vClients;		//In case the host drops, we can reconnect
		std::vector<CNetMsg>				m_vMsgs;

	private:	//Func
		bool	parse_msg(RakNet::Packet* packet);
	public:		//Func
		CClient(void);
		~CClient(void);
		
		/*
		- starts the RakPeerInterface
		- creates a client at an unspecified port
		*/
		bool	init(void);
		/*
		- sends a broadcast msg on the lan
		*/
		bool	broadcast(ushort port);
		/*
		- receives incoming packets
		- parses msgs from server and broadcasts
		- pushes incoming messages onto the msg_list
		- returns true on success / false on failure
		*/
		bool	read_msg(void);
		/*
		- sends outgoing messages
		- if server is not connected, returns false
		- returns true on success / false on failure
		*/
		bool		send_msg(CNetMsg& msg, const PacketPriority& prio = PacketPriority::HIGH_PRIORITY, const PacketReliability& reli = PacketReliability::RELIABLE_ORDERED);

		/*
		- attempts a connection to the specified local ip address and port
		- returns true on success / false on failure
		- returning true doesn't mean we are connected, just that the msg asking for a connection has been sent
		- read_msg will hold a msg detailing if the connection succeeded or not
		*/
		bool	connect_lan(const char* ip_addr, const ushort port);

		/*
		- disconnects from server
		- shuts down the network
		*/
		void	stop(void);

		bool	disconnect(void);


		/*
		- Outputs debug info to the console
		*/
		void OutputInfo(void);


		//Gets
		inline bool GetActive(void) const { return this->m_pRak != NULL; }
		inline std::vector<CNetMsg>	GetBroadcast(void)					const	{ return this->m_vBroadcasts; }
		inline std::vector<CNetMsg> GetMsgs(void)					const	{ return this->m_vMsgs; }
		inline TPeer	GetPeer(void) const { return this->m_tPeer; }
		inline bool		GetConnected(void) const { return this->m_tPeer.m_scID > -1; }
		//Sets
		inline void	SetMsgsClear(void) { this->m_vMsgs = std::vector<CNetMsg>(); }
		inline void SetBroadcastClear(void) { this->m_vBroadcasts = std::vector<CNetMsg>(); }

	};

	//class CClient
	//{
	//private: //Data
	//	bool								m_bConnected;
	//	RakNet::RakPeerInterface*			m_pRak;
	//	RakNet::NatPunchthroughClient*		m_pNatPunch;
	//	RakNet::SystemAddress				m_Server;
	//	std::vector<TPeer>					m_vClients;
	//	std::vector<RakNet::SystemAddress>	m_vPotentialServers;
	//	std::vector<CNetMsg>				m_vMsg_List;
	//	schar								m_scID;
	//
	//private: //Methods
	//	void	parse_msg(const RakNet::Packet* packet);
	//	void	handle_pong(const RakNet::Packet* packet);
	//	void	handle_connection_accepted(const RakNet::Packet* packet);
	//public:  //Methods
	//	CClient(void);
	//	~CClient(void);
	//
	//	// - starts the RakPeerInterface
	//	// - creates a client at an unspecified port
	//	bool	init();
	//	// - broadcasts to any local area network servers at the specified port
	//	// - returns true if startup was successful (doesn't mean we are connected)(read_msg handles that)
	//	bool	broadcast(const unsigned short port);
	//	// - receives incoming packets
	//	// - handles connecting and disconnecting from server
	//	// - pushes incoming messages onto the msg_list
	//	// - returns true on success / false on failure
	//	bool	read_msg(void);
	//	// - sends outgoing messages to the connected server
	//	// - returns true on success / false on failure
	//	bool	send_msg(const RakNet::BitStream* msg, const PacketPriority prio, const PacketReliability reli);
	//	// - disconnects from server
	//	// - shuts down the network
	//	void	stop(void);
	//
	//	bool	lan(const char* ip_address, const unsigned short port);
	//	bool	wan(const char* ip_address, const unsigned short port);
	//
	//	void	punch(const char* guid);
	//
	//	inline bool	GetActive				( void )	const	{ return this->m_bActive; }
	//	inline bool GetConnected			( void )	const	{ return this->m_bConnected; }
	//	inline bool GetConnectOnBroadcast	( void )	const	{ return this->m_bConnectOnBroadcast; }
	//	inline char GetID					( void )	const	{ return this->m_scID; }
	//	inline std::vector<RakNet::SystemAddress> GetPotentialServers ( void )	const	{ return this->m_vPotentialServers; }
	//};
}