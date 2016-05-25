/*
Author:			Daniel Habig
Date:			2 - 11 - 14
File:			Server.cpp
Purpose:		Class to perform operations required of a server on a network
Sends Data to any number of connected clients
Recvs data from connected clients and places messages into a list
Handles connecting, accepting, and disconnecting clients
*/
#include "Server.h"
#include <RakPeerInterface.h>
#include <NatPunchthroughServer.h>
#include <BitStream.h>
#include <GetTime.h>
#include <algorithm>


namespace Network
{
	CServer::CServer(void)
	{
		this->m_pRak = NULL;
		this->m_ucMaxClients = 0;
	}
	CServer::~CServer(void)
	{

	}

	bool CServer::init(const ushort port)
	{
		//Early Out
		if (this->m_pRak != NULL)
			return false;

		//Clear out any 'old' data
		this->m_vMsgs.clear();

		//Initialization
		this->m_pRak = RakNet::RakPeerInterface::GetInstance();
		RakNet::SocketDescriptor sd(port, 0);
		sd.socketFamily = AF_INET;
		if (m_pRak->Startup(this->m_ucMaxClients, &sd, 1) != RakNet::RAKNET_STARTED)
		{
			this->stop();
			return false;
		}
		m_pRak->SetMaximumIncomingConnections(this->m_ucMaxClients);

		//Debug Output
#if DB_NET_MSG_CNT
		printf("S : Server Initialized\n");
		printf("S : Port: \t\t%d\n", port);
		this->OutputInfo();
		printf("S : Listening For Clients\n");
#endif
		return true;
	}
	bool CServer::read_msg(void)
	{
		//Early Out
		if (this->m_pRak == NULL)
			return false;

		RakNet::Packet* packet = NULL;
		for (packet = m_pRak->Receive(); packet; m_pRak->DeallocatePacket(packet), packet = m_pRak->Receive())
		{
			if (!this->parse_msg(packet))
				return false;
		}
		return true;
	}
	bool CServer::send_msg(CNetMsg& msg, const TPeer& dest, const PacketPriority& prio, const PacketReliability& reli)
	{
		//Early Out
		if (this->m_pRak == NULL)
			return false;

		//Duplicate to all clients
		if (dest.m_scID == -1)
		{
			ulong size = this->m_vClients.size();
			for (ulong i = 0; i < size; ++i)
			{
				if (!this->send_msg(msg, this->m_vClients[i], prio, reli))
					return false;
			}
			return true;
		}

		//Parse msg to bitstream
		RakNet::BitStream stream;
		RakNet::Time timeStamp = RakNet::GetTime();
		stream.Write(RakNet::MessageID(ID_TIMESTAMP));
		stream.Write(timeStamp);
		stream.Write(RakNet::MessageID(msg.GetType()));
		stream.Write(msg.GetData(), msg.GetWrite());

#if DB_NET_MSG_SR
		printf("S : Sending Msg to: \t\t\t%s\n", dest.m_Addr.ToString());
		printf("  : Client #%d\n", dest.m_scID);
		printf("  : Msg Type:\t%s\n", ENetMsgStr(msg.GetType()));
		printf("  : Msg Size:\t%d\n", msg.GetWrite());
		printf("  : Msg Time:\t%d\n", timeStamp);
#endif

		return this->m_pRak->Send(&stream, prio, reli, 0, dest.m_Guid, false) != 0;
	}
	void CServer::stop(void)
	{
		if (this->m_pRak != NULL)
		{
			//Disconnect all clients
			for (ulong i = 0; i < this->m_vClients.size(); ++i) /* size needs to be re-calculated each iteration */
				this->disconnect(this->m_vClients[i].m_scID);

			//Shutdown RakNet
			RakNet::RakPeerInterface::DestroyInstance(this->m_pRak);
			this->m_pRak = NULL;
#if DB_NET_MSG_CNT
			printf("S : Shuting Down.\n");
#endif
		}
	}
	bool CServer::disconnect(const schar& clientID)
	{
		//Early Out
		if (this->m_pRak == NULL)
			return false;
		if (clientID < 0 || clientID >= slong(this->m_vClients.size()))
			return false;

		//Get the client guid
		TPeer peer;
		ulong size = this->m_vClients.size();
		for (ulong i = 0; i < size; ++i)
		{
			if (this->m_vClients[i].m_scID == clientID)
			{
				peer = this->m_vClients[i];
				break;
			}
		}

		//Disconnect
		this->m_pRak->CloseConnection(peer.m_Guid, true, 0, HIGH_PRIORITY);

		//Set the slots
		if (peer.m_scID < this->m_ucMaxClients)
			this->m_qSlots.push(peer.m_scID);
		//Set the client vector
		this->m_vClients.erase(this->m_vClients.begin() + peer.m_scID);

#if DB_NET_MSG_CNT
		printf("S : Disconnected Client #%d from: \t%s\n", peer.m_scID, peer.m_Addr.ToString());
#endif

		return true;
	}

	bool CServer::parse_msg(RakNet::Packet* packet)
	{
#pragma region DEBUG_OUTPUT
#if DB_NET_MSG_CNT
		switch (packet->data[0])
		{
		case ID_NEW_INCOMING_CONNECTION:{
											printf("S : Incoming Connection from: \t\t%s\n", packet->systemAddress.ToString());
											break; }
		case ID_DISCONNECTION_NOTIFICATION:{
											   printf("S : Disconnection from: \t\t%s\n", packet->systemAddress.ToString());
											   break; }
		case ID_CONNECTION_LOST:{
									printf("S : Lost Connection from: \t\t%s\n", packet->systemAddress.ToString());
									break; }
		case ID_UNCONNECTED_PING:{
									 printf("S : Received Ping Broadcast from: \t%s\n", packet->systemAddress.ToString());
									 break; }
		case ID_TIMESTAMP:{
							  printf("S : Received Msg from: \t\t\t%s\n", packet->systemAddress.ToString());
							  break;	}
		default: {
					 printf("S : Received Identifier %i from: \t\t%s\n", packet->data[0], packet->systemAddress.ToString());
					 break; }
		}
#endif
#pragma endregion
		
		//Set Msg Type
		CNetMsg msg = CNetMsg(ENetMsg(packet->data[0]));

		//If connecting client then give an id
		if (packet->data[0] == ID_NEW_INCOMING_CONNECTION)
		{
			this->connect_client(packet);
		}

		//Get Peer Data
		TPeer peer;
		peer.m_Addr = packet->systemAddress;
		peer.m_Guid = packet->guid;
		ulong size = this->m_vClients.size();
		for (ulong i = 0; i < size; ++i)
		{
			if (this->m_vClients[i].m_Guid == peer.m_Guid)
			{
				peer.m_scID = schar(i);
				break;
			}
		}
		msg.SetPeer(peer);

		//If user type, then get data
		if (packet->data[0] == ID_TIMESTAMP)
		{
			//Get the header parts of the packet
			RakNet::BitStream		bsIn(packet->data, packet->length, false);
			uchar					useTimeStamp;
			RakNet::Time			timeStamp;
			uchar					typeID;
			//Read them in
			bsIn.Read((RakNet::MessageID)useTimeStamp);
			bsIn.Read(timeStamp);
			bsIn.Read((RakNet::MessageID)typeID);
			ulong offset = bsIn.GetReadOffset() / 8;
			//Parse into a CNetMsg
			msg.SetType(ENetMsg(typeID));
			msg.WriteRaw((const char*)(packet->data + offset), ushort(packet->length - offset));
			msg.SetTime(timeStamp);
#if DB_NET_MSG_SR
			printf("  : Client #%d\n", peer.m_scID);
			printf("  : Msg Type:\t%s\n", ENetMsgStr(msg.GetType()));
			printf("  : Msg Size:\t%d\n", msg.GetWrite());
			printf("  : Msg Time:\t%d\n", msg.GetTime());
#endif
		}

		//Add msg to vector
		this->m_vMsgs.push_back(msg);
		return true;
	}
	void CServer::connect_client(RakNet::Packet* packet)
	{
		TPeer peer;
		//Get an open slot
		peer.m_scID = this->m_qSlots.top();
		this->m_qSlots.pop();
		//Get the address
		peer.m_Addr = packet->systemAddress;
		peer.m_Guid = packet->guid;
		//Add to list
		this->m_vClients.push_back(peer);
		std::sort(this->m_vClients.begin(), this->m_vClients.end(), TPeerAltOrder());
		RakNet::TimeMS timeOut = LOST_TIME;
		m_pRak->SetTimeoutTime(timeOut, RakNet::UNASSIGNED_SYSTEM_ADDRESS);
		//Send Client Their ID
		{
			CNetMsg msg = CNetMsg(NM_CLIENT_ID);
			msg.WriteByte(peer.m_scID);
			this->send_msg(msg, peer, HIGH_PRIORITY, RELIABLE_ORDERED);
		}
#if DB_NET_MSG_CNT
		printf("S : Connected With Client #%i at \t%s\n", peer.m_scID, peer.m_Addr.ToString());
#endif
	}
	void CServer::OutputInfo(void)
	{
		printf("S : Guid: \t\t%s\n", this->m_pRak->GetGuidFromSystemAddress(RakNet::UNASSIGNED_SYSTEM_ADDRESS).ToString());
		ulong numAddr = this->m_pRak->GetNumberOfAddresses();
		printf("S : IP Count: \t\t%d\n", numAddr);
		for (ulong i = 0; i < numAddr; ++i)
			printf("S : IP Addr[%d]: \t%s\n", i, this->m_pRak->GetLocalIP(i));
		printf("S : Max Clients: \t%d\n", this->m_ucMaxClients);
	}
	void CServer::lock(const bool b)
	{
		//Early Out
		if (this->m_pRak == NULL)
			return;

		if (b)
			this->m_pRak->SetMaximumIncomingConnections(0);
		else
			this->m_pRak->SetMaximumIncomingConnections(this->m_ucMaxClients);
	}


	//Gets
	CNetMsg CServer::GetBroadcastResponse(void) const
	{
		//NOTE: This is not tested, I am unsure if this memory will need to be deleted
		char* pBuffer = NULL;
		uint uiLen = 0;
		this->m_pRak->GetOfflinePingResponse(&pBuffer, &uiLen);
		CNetMsg ret;
		ret.WriteRaw(pBuffer, uiLen);
		return ret;
	}
	//Sets
	void CServer::SetMaxClients(const uchar ucMaxClients)
	{
		this->m_ucMaxClients = ucMaxClients;
		if (this->m_pRak != NULL)
			this->m_pRak->SetMaximumIncomingConnections(this->m_ucMaxClients);

		//Set the slots
		ulong size = this->m_vClients.size();
		for (uchar i = 0; i < ucMaxClients; ++i)
		{
			bool push = true;
			for (ulong j = 0; j < size; ++j)
			{
				if (this->m_vClients[j].m_scID == i)
				{
					push = false;
					break;
				}
			}
			if (push)
				this->m_qSlots.push(i);
		}

	}
	bool CServer::SetBroadcastResponse(CNetMsg& msg)
	{
		//Early Out
		if (this->m_pRak == NULL)
			return false;
		//Response can't be over 400 bytes
		if (msg.GetWrite() > 400)
			return false;

		this->m_pRak->SetOfflinePingResponse(msg.GetData(), msg.GetWrite());
		return true;
	}
}


//----------------------------
//	CServer::CServer(void)
//	{
//		this->m_bActive = false;
//		this->m_pRak = nullptr;
//		for (unsigned int i = 0; i < MAX_CLIENTS; i++)
//			m_qSlots.push(i);
//	}
//	CServer::~CServer(void)
//	{
//		this->stop();
//	}
//
//	bool CServer::connect_client(const RakNet::Packet* packet)
//	{
//		TPeer peer;
//		//Get an open slot
//		peer.m_scID = this->m_qSlots.top();
//		this->m_qSlots.pop();
//		//Get the address
//		peer.m_Addr = packet->systemAddress;
//		//Add to list
//		this->m_vClients.push_back(peer);
//		RakNet::TimeMS timeOut = LOST_TIME;
//		m_pRak->SetTimeoutTime(timeOut, RakNet::UNASSIGNED_SYSTEM_ADDRESS);
//#if DB_NET_MSG_CNT
//		printf("S : Connected With Client # %i.\n", peer.m_scID);
//#endif
//		//Send Client Their ID
//		{
//			RakNet::BitStream stream;
//			stream.Write((RakNet::MessageID)NM_CLIENT_ID);
//			stream.Write((const char*)&peer.m_scID, sizeof(unsigned char));
//			this->send_msg(&stream, &peer.m_Addr, HIGH_PRIORITY, RELIABLE_ORDERED);
//		}
//		//Add New Client Msg
//		{
//			CNetMsg msg((ENetMsg)ID_NEW_INCOMING_CONNECTION);
//			msg.WriteByte(peer.m_scID);
//			this->m_vMsg_List.push_back(msg);
//		}
//		return true;
//	}
//	bool CServer::disconnect_client(const RakNet::Packet* packet)
//	{
//		//Remove the Peer from the list
//		TPeer peer;
//		unsigned int size = this->m_vClients.size();
//		for (unsigned int i = 0; i < size; i++)
//		{
//			if (m_vClients[i].m_Addr == packet->systemAddress)
//			{
//				peer = m_vClients[i];
//				m_vClients.erase(m_vClients.begin() + i);
//				break;
//			}
//		}
//		//Close the connection
//		m_pRak->CloseConnection(packet->systemAddress, true, 0, HIGH_PRIORITY);
//		//Push the available id onto the slot
//		this->m_qSlots.push(peer.m_scID);
//#if DB_NET_MSG_CNT
//		printf("S : Disconnected With Client # %i.\n", peer.m_scID);
//#endif
//		//Add Removed Client Msg
//		CNetMsg msg((ENetMsg)ID_DISCONNECTION_NOTIFICATION);
//		msg.SetID(peer.m_scID);
//		this->m_vMsg_List.push_back(msg);
//		return true;
//	}
//	bool CServer::disconnect_client(const schar scID)
//	{
//		//Remove Peer From List
//		TPeer peer;
//		unsigned int size = this->m_vClients.size();
//		for (unsigned int i = 0; i < size; i++)
//		{
//			if (m_vClients[i].m_scID == scID)
//			{
//				peer = m_vClients[i];
//				m_vClients.erase(m_vClients.begin() + i);
//				break;
//			}
//		}
//		//Close the connection
//		m_pRak->CloseConnection(peer.m_Addr, true, 0, HIGH_PRIORITY);
//		//Push the available id onto the slot
//		this->m_qSlots.push(peer.m_scID);
//#if DB_NET_MSG_CNT
//		printf("S : Disconnected With Client # %i.\n", peer.m_scID);
//#endif
//		//Add Removed Client Msg
//		CNetMsg msg((ENetMsg)ID_DISCONNECTION_NOTIFICATION);
//		msg.SetID(peer.m_scID);
//		this->m_vMsg_List.push_back(msg);
//		return true;
//	}
//	void CServer::parse_msg(const RakNet::Packet* packet, const unsigned char ID)
//	{
//		//Get the header parts of the packet
//		RakNet::BitStream		bsIn(packet->data, packet->length, false);
//		unsigned char	useTimeStamp;
//		RakNet::Time			timeStamp;
//		unsigned char	typeID;
//		//Read them in
//		bsIn.Read((RakNet::MessageID)useTimeStamp);
//		bsIn.Read(timeStamp);
//		bsIn.Read((RakNet::MessageID)typeID);
//		unsigned int offset = bsIn.GetReadOffset() / 8;
//		//Parse into a CNetMsg
//		CNetMsg msg = CNetMsg((char*)(packet->data + offset), (unsigned short)packet->length - offset, ID, (ulong)timeStamp, (ENetMsg)typeID);
//		this->m_vMsg_List.push_back(msg);
//#if DB_NET_MSG_BLD
//		printf("S : Parsing Msg of Type: %s & Size: %i.\n", ENetMsgStr(msg.GetType()), msg.GetWrite());
//#endif
//	}
//
//	/* Init */
//	bool CServer::init(const unsigned short port)
//	{
//		this->m_pRak = RakNet::RakPeerInterface::GetInstance();
//		this->m_pNatPunch = RakNet::NatPunchthroughServer::GetInstance();
//		m_pRak->AttachPlugin(this->m_pNatPunch);
//		RakNet::SocketDescriptor sd(port, 0);
//		sd.socketFamily = AF_INET;
//		if (m_pRak->Startup(MAX_CLIENTS, &sd, 1) != RakNet::RAKNET_STARTED)
//		{
//			RakNet::RakPeerInterface::DestroyInstance(this->m_pRak);
//			RakNet::NatPunchthroughServer::DestroyInstance(this->m_pNatPunch);
//			this->m_pRak = nullptr;
//			this->m_pNatPunch = nullptr;
//			return false;
//		}
//		m_pRak->SetMaximumIncomingConnections(MAX_CLIENTS);
//		this->m_bActive = true;
//#if DB_NET_MSG_CNT
//		printf("S : Guid = %s\n", m_pRak->GetGuidFromSystemAddress(RakNet::UNASSIGNED_SYSTEM_ADDRESS).ToString());
//		printf("S : Hosting Server At IP: %s\n", m_pRak->GetLocalIP(0));
//		printf("S : Listening For Clients.\n");
//#endif
//		return true;
//	}
// /* Read_Msg */
//	bool CServer::read_msg(void)
//	{
//		if (!this->m_bActive)
//			return false;
//		RakNet::Packet* packet;
//		for (packet = m_pRak->Receive(); packet; m_pRak->DeallocatePacket(packet), packet = m_pRak->Receive())
//		{
//			//Get the client ID
//			char id = -1;
//			unsigned int size = this->m_vClients.size();
//			for (unsigned int i = 0; i < size; i++)
//			{
//				if (packet->systemAddress == m_vClients[i].m_Addr)
//				{
//					id = m_vClients[i].m_scID;
//					break;
//				}
//			}
//
//			switch (packet->data[0])
//			{
//			case ID_NEW_INCOMING_CONNECTION:{
//#if DB_NET_MSG_CNT
//												printf("S : Incoming Connection.\n");
//#endif
//												this->connect_client(packet);
//												break; }
//			case ID_DISCONNECTION_NOTIFICATION:{
//#if DB_NET_MSG_CNT
//												   printf("S : Client # %i Has Disconnected.\n", id);
//#endif
//												   this->disconnect_client(packet);
//												   break; }
//			case ID_CONNECTION_LOST:{
//#if DB_NET_MSG_CNT
//										printf("S : Lost Connection With Client # %i.\n", id);
//#endif
//										this->disconnect_client(packet);
//										break; }
//			case ID_TIMESTAMP: {
//#if DB_NET_MSG_SR
//								   printf("S : Received Msg From Client # %i.\n", id);
//#endif
//								   break;	}
//			case ID_UNCONNECTED_PING:{
//#if DB_NET_MSG_CNT
//										 printf("S : Received Broadcast.\n");
//#endif
//										 break; }
//			default: {
//#if DB_NET_MSG_CNT
//						 printf("S : Received Identifier %i From Client # %i.\n", packet->data[0], id);
//#endif
//						 break;	}
//			}
//		}
//
//		return true;
//	}
//	/* Send_Msg */
//	bool CServer::send_msg(const RakNet::BitStream* msg, const RakNet::SystemAddress* dest, const PacketPriority prio, const PacketReliability reli)
//	{
//		if (dest == nullptr)
//		{
//			unsigned int size = this->m_vClients.size();
//			bool result = true;
//			for (unsigned int i = 0; i < size; i++)
//			{
//#if DB_NET_MSG_SR
//				printf("S : Sending Msg To Client # %i.\n", m_vClients[i].m_scID);
//#endif
//				result = this->send_msg(msg, &this->m_vClients[i].m_Addr, prio, reli);
//				if (!result)
//					return false;
//			}
//			return true;
//		}
//		return m_pRak->Send(msg, prio, reli, 0, *dest, false) != 0;
//	}
//	/* Stop */
//	void CServer::stop(void)
//	{
//		if (this->m_bActive)
//		{
//#if DB_NET_MSG_CNT
//			printf("S : Shuting Down Server.\n");
//#endif
//			//Disconnect all clients
//			unsigned int size = this->m_vClients.size();
//			while (!this->m_vClients.empty())
//			{
//#if DB_NET_MSG_CNT
//				printf("S : Closed Connection With Client # %i.\n", m_vClients[0].m_scID);
//#endif
//				this->disconnect_client(m_vClients[0].m_scID);
//			}
//			this->m_bActive = false;
//			//Close RakPeerInterface
//			RakNet::RakPeerInterface::DestroyInstance(this->m_pRak);
//			RakNet::NatPunchthroughServer::DestroyInstance(this->m_pNatPunch);
//			this->m_pRak = NULL;
//			this->m_pNatPunch = NULL;
//		}
//	}
//	/* Lock */
//	void CServer::lock(bool b)
//	{
//		if (!m_pRak)
//			return;
//		if (b)
//		{
//			m_pRak->SetMaximumIncomingConnections(0);
//#if DB_NET_MSG_CNT
//			printf("S : Locked Incoming Connections.\n");
//#endif
//		}
//		else
//		{
//			m_pRak->SetMaximumIncomingConnections(MAX_CLIENTS);
//#if DB_NET_MSG_CNT
//			printf("S : Unlocked Incoming Connections.\n");
//#endif
//		}
//	}
//}