/*
Author:			Daniel Habig
Date:			2 - 11 - 14
File:			Server.h
Purpose:		Class to perform operations required of a server on a network
Sends Data to any number of connected clients
Recvs data from connected clients and places messages into a list
Handles connecting, accepting, and disconnecting clients
*/
#pragma once
#include "NetMsg.h"
#include <BitStream.h>
#include <PacketPriority.h>
#include <vector>
#include <queue>
#include <functional>

namespace RakNet{ class NatPunchthroughServer; }

namespace Network
{
	class CServer
	{
	private:	//Data
		RakNet::RakPeerInterface*			m_pRak;
		uchar								m_ucMaxClients;
		std::vector<TPeer>					m_vClients;
		std::priority_queue<uchar, std::vector<uchar>, std::greater<unsigned char>> m_qSlots;
		std::vector<CNetMsg>				m_vMsgs;

	private:	//Func
		bool	parse_msg(RakNet::Packet* packet);
		void	connect_client(RakNet::Packet* packet);
	public:		//Func
		CServer(void);
		~CServer(void);
		/*
		- starts the RakPeerInterface
		- creates a server at the specified port
		- begins listening for clients
		- returns true on success / false on failure
		*/
		bool		init		( const ushort port );
		/*
		- receives incoming packets
		- parses msgs from clients and broadcasts
		- pushes incoming messages onto the msg_list
		- returns true on success / false on failure
		*/
		bool		read_msg	( void );
		/*
		- sends outgoing messages
		- if destination is not connected, returns false
		- if destination's id == -1, duplicates msg and sends to all connected clients
		- returns true on success / false on failure
		*/
		bool		send_msg	( CNetMsg& msg, const TPeer& dest = TPeer(), const PacketPriority& prio = PacketPriority::HIGH_PRIORITY, const PacketReliability& reli = PacketReliability::RELIABLE_ORDERED);
		/*
		- disconnects all clients
		- shuts down the network
		*/
		void		stop		( void );
		/*
		- closes connection to client at index
		- gives back availible client id (only if the id is less than Max Clients)
		- removes client from Clients vector
		*/
		bool		disconnect	( const schar& clientID );
		/*
		- does not change the max clients
		- true - raknet's max clients = 0, false - raknet's max clients = m_ucMaxClients
		*/
		void		lock		( const bool b );


		/*
		- Outputs debug info to the console
		*/
		void OutputInfo(void);

		//Gets
		inline uchar				GetMaxClients		( void )					const	{ return this->m_ucMaxClients; }
		inline std::vector<TPeer>	GetClientVec		( void )					const	{ return this->m_vClients; }
		inline std::vector<CNetMsg> GetMsgs				( void )					const	{ return this->m_vMsgs; }
		CNetMsg						GetBroadcastResponse(void) const;
		//Sets
		void						SetMaxClients		( const uchar ucMaxClients );
		inline void					SetMsgsClear		( void )							{ this->m_vMsgs = std::vector<CNetMsg>(); }
		bool						SetBroadcastResponse(CNetMsg& msg);

	};



	//class CServer
	//{
	//private: //Data
	//	RakNet::RakPeerInterface*		m_pRak;
	//	RakNet::NatPunchthroughServer*	m_pNatPunch;
	//	std::vector<TPeer>				m_vClients;
	//	std::vector<CNetMsg>			m_vMsg_List;
	//	std::priority_queue<unsigned char, std::vector<unsigned char>, std::greater<unsigned char>>	m_qSlots;


	//private: //Methods
	//	bool	connect_client(const RakNet::Packet* packet);
	//	bool	disconnect_client(const RakNet::Packet* packet);
	//	bool	disconnect_client(const schar scID);
	//	void	parse_msg(const RakNet::Packet* packet, const unsigned char ID);
	//public:  //Methods
	//	CServer(void);
	//	~CServer(void);

	//	// - starts the RakPeerInterface
	//	// - creates a server at the specified port
	//	// - begins listening for clients
	//	// - returns true on success / false on failure
	//	bool		init(const unsigned short port);
	//	// - receives incoming packets
	//	// - handles connecting and disconnecting clients
	//	// - pushes incoming messages onto the msg_list
	//	// - returns true on success / false on failure
	//	bool		read_msg(void);
	//	// - sends outgoing messages to connected clients
	//	// - if destination is not connected, returns false
	//	// - if destination is NULL, duplicates msg and sends to all connected clients
	//	// - returns true on success / false on failure
	//	bool		send_msg(const RakNet::BitStream* msg, const RakNet::SystemAddress* dest, const PacketPriority prio, const PacketReliability reli);
	//	// - disconnects all clients
	//	// - shuts down the network
	//	void		stop(void);
	//	// - true  => no new clients may connect
	//	// - false => new clients may connect
	//	void		lock(bool b);


	//	inline bool	GetActive(void) const { return this->m_pRak != NULL; }
	//	inline uint	GetClientCount(void) const { return this->m_vClients.size(); }
	//	inline const char* GetRakGuid(void) const { return this->m_pRak->GetGuidFromSystemAddress(RakNet::UNASSIGNED_SYSTEM_ADDRESS).ToString(); }

	//};
}