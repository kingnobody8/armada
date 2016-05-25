/*
Author:			Daniel Habig
Date:			2 - 11 - 14
File:			Client.cpp
Purpose:		Class to perform operations required of a client on a network
Sends Data to a connected server
Recvs data from the connected server
*/
#include "Client.h"
#include <RakPeerInterface.h>
#include <NatPunchthroughClient.h>
#include <BitStream.h>
#include <GetTime.h>

namespace Network
{
	CClient::CClient(void)
	{
		this->m_pRak = NULL;
	}
	CClient::~CClient(void)
	{

	}

	bool CClient::init(void)
	{
		//Early Out
		if (this->m_pRak != NULL)
			return false;

		//Clear out any 'old' data
		this->m_vMsgs.clear();
		this->m_vBroadcasts.clear();

		//Init RakNet
		m_pRak = RakNet::RakPeerInterface::GetInstance();
		RakNet::SocketDescriptor sd;
		if (m_pRak->Startup(1, &sd, 1) != RakNet::RAKNET_STARTED)
		{
			this->stop();
			return false;
		}

		//Set Peer
		this->m_tPeer.m_Guid = this->m_pRak->GetGuidFromSystemAddress(RakNet::UNASSIGNED_SYSTEM_ADDRESS);
		this->m_tPeer.m_Addr = this->m_pRak->GetSystemAddressFromGuid(this->m_tPeer.m_Guid);

#if DB_NET_MSG_CNT
		printf("C : Client Initialized\n");
		this->OutputInfo();
		printf("C : Ready To Connect\n");
#endif
		return true;
	}
	bool CClient::broadcast(ushort port)
	{
		//Early Out
		if (this->m_pRak == NULL)
			return false;
		//NOTE also check if we are connected to a server already

#if DB_NET_MSG_CNT
		printf("C : Broadcasting to Port: %d.\n", port);
#endif
		return this->m_pRak->Ping("255.255.255.255", port, false);
	}
	bool CClient::read_msg(void)
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
	bool CClient::send_msg(CNetMsg& msg, const PacketPriority& prio, const PacketReliability& reli)
	{
		//Early Out
		if (this->m_pRak == NULL)
			return false;

		//Parse msg to bitstream
		RakNet::BitStream stream;
		RakNet::Time timeStamp = RakNet::GetTime();
		stream.Write(RakNet::MessageID(ID_TIMESTAMP));
		stream.Write(timeStamp);
		stream.Write(RakNet::MessageID(msg.GetType()));
		stream.Write(msg.GetData(), msg.GetWrite());

#if DB_NET_MSG_SR
		printf("C : Sending Msg to: \t\t\t%s\n", this->m_tServer.m_Addr.ToString());
		printf("  : Msg Type:\t%s\n", ENetMsgStr(msg.GetType()));
		printf("  : Msg Size:\t%d\n", msg.GetWrite());
		printf("  : Msg Time:\t%d\n", timeStamp);
#endif

		return this->m_pRak->Send(&stream, prio, reli, 0, this->m_tServer.m_Guid, false) != 0;
	}
	bool CClient::connect_lan(const char* ip_addr, const ushort port)
	{
		//Early Out
		if (this->m_pRak == NULL)
			return false;

		//Send Connection Msg
		if (m_pRak->Connect(ip_addr, port, 0, 0) != RakNet::CONNECTION_ATTEMPT_STARTED)
		{
			this->stop();
			return false;
		}
#if DB_NET_MSG_CNT
		printf("C : Attempting Connection to IP: \t%s|%i\n", ip_addr, port);
#endif
		return true;
	}

	void CClient::stop(void)
	{
		if (this->m_pRak != NULL)
		{
			this->disconnect();
			RakNet::RakPeerInterface::DestroyInstance(this->m_pRak);
			this->m_pRak = NULL;
			this->m_tPeer = TPeer();
#if DB_NET_MSG_CNT
			printf("C : Shuting Down.\n");
#endif
		}
	}

	bool CClient::parse_msg(RakNet::Packet* packet)
	{
#pragma region DEBUG_OUTPUT
#if DB_NET_MSG_CNT
		switch (packet->data[0])
		{
		case ID_CONNECTION_REQUEST_ACCEPTED:{
												printf("C : Connected to Server at: \t\t%s\n", packet->systemAddress.ToString());
												break; }
		case ID_NO_FREE_INCOMING_CONNECTIONS:{
												 printf("C : No Free Slots at: \t\t%s\n", packet->systemAddress.ToString());
												 break; }
		case ID_DISCONNECTION_NOTIFICATION:{
											   printf("C : Disconnection from: \t\t%s\n", packet->systemAddress.ToString());
											   break; }
		case ID_CONNECTION_LOST:{
									printf("C : Lost Connection from: \t\t%s\n", packet->systemAddress.ToString());
									break; }
		case ID_UNCONNECTED_PONG:{
									 printf("C : Received Pong Broadcast from: \t%s\n", packet->systemAddress.ToString());
									 break; }
		case ID_TIMESTAMP:{
							  printf("C : Received Msg from: \t\t\t%s\n", packet->systemAddress.ToString());
							  break;	}
		default: {
					 printf("C : Received Identifier %i from: \t%s\n", packet->data[0], packet->systemAddress.ToString());
					 break; }
		}
#endif
#pragma endregion


		//Set Msg Type
		CNetMsg msg = CNetMsg(ENetMsg(packet->data[0]));

		//Get Peer Data
		TPeer peer;
		peer.m_Addr = packet->systemAddress;
		peer.m_Guid = packet->guid;
		msg.SetPeer(peer);

		//If pong broadcast
		if (packet->data[0] == ID_UNCONNECTED_PONG)
		{
			bool bInList = false;
			ulong size = this->m_vBroadcasts.size();
			for (ulong i = 0; i < size; i++)
			{
				if (this->m_vBroadcasts[i].GetPeer() == peer)
					bInList = true;
			}
			if (!bInList)
			{
				//Get the header parts of the packet
				RakNet::BitStream		bsIn(packet->data, packet->length, false);
				RakNet::TimeMS			timeMS;
				uchar					typeID;
				//Read them in
				bsIn.Read((RakNet::MessageID)typeID);
				bsIn.Read((RakNet::TimeMS)timeMS);
				ulong offset = bsIn.GetReadOffset() / 8;
				//Parse into a CNetMsg
				msg.SetType(ENetMsg(typeID));
				msg.WriteRaw((const char*)(packet->data + offset), ushort(packet->length - offset));
				this->m_vBroadcasts.push_back(msg);
			}
		}
		//If user type, then get data
		else if (packet->data[0] == ID_TIMESTAMP)
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
			printf("  : Msg Type:\t%s\n", ENetMsgStr(msg.GetType()));
			printf("  : Msg Size:\t%d\n", msg.GetWrite());
			printf("  : Msg Time:\t%d\n", msg.GetTime());
#endif

			//If NM_CLIENT_ID
			if (msg.GetType() == NM_CLIENT_ID)
			{
				this->m_tPeer.m_scID = msg.ReadByte();
				this->m_tServer.m_Addr = packet->systemAddress;
				this->m_tServer.m_Guid = packet->guid;
				this->m_tServer.m_scID = 0;
				RakNet::TimeMS timeOut = LOST_TIME;
				m_pRak->SetTimeoutTime(timeOut, RakNet::UNASSIGNED_SYSTEM_ADDRESS);
#if DB_NET_MSG_CNT
				printf("C : Obtained ID #%d\n", this->m_tPeer.m_scID);
#endif
			}
		}

		//Add msg to vector
		this->m_vMsgs.push_back(msg);
		return true;
	}
	bool CClient::disconnect(void)
	{
		//Early Out
		if (this->m_pRak == NULL)
			return false;
		if (this->m_tServer.m_scID == -1)
			return false;

		//Disconnect
		this->m_pRak->CloseConnection(this->m_tServer.m_Guid, true, 0, HIGH_PRIORITY);
		this->m_tServer = TPeer();
		this->m_tPeer.m_scID = -1;

#if DB_NET_MSG_CNT
		printf("C : Disconnected from Server\n");
#endif
		return true;
	}
	void CClient::OutputInfo(void)
	{
		printf("C : Guid: \t\t%s\n", this->m_pRak->GetGuidFromSystemAddress(RakNet::UNASSIGNED_SYSTEM_ADDRESS).ToString());
		ulong numAddr = this->m_pRak->GetNumberOfAddresses();
		printf("C : IP Count: \t\t%d\n", numAddr);
		for (ulong i = 0; i < numAddr; ++i)
			printf("C : IP Addr[%d]: \t%s\n", i, this->m_pRak->GetLocalIP(i));
	}

	//	CClient::CClient(void)
	//	{
	//		this->m_bActive = false;
	//		this->m_bConnected = false;
	//		this->m_bConnectOnBroadcast = false;
	//		this->m_eConnectionType = LAN;
	//		this->m_pRak = NULL;
	//		this->m_scID = -1;
	//	}
	//	CClient::~CClient(void)
	//	{
	//		this->stop();
	//	}
	//
	//	void CClient::parse_msg(const RakNet::Packet* packet)
	//	{
	//		//Get the header parts of the packet
	//		RakNet::BitStream		bsIn(packet->data, packet->length, false);
	//		unsigned char	useTimeStamp;
	//		RakNet::Time	timeStamp;
	//		unsigned char	typeID;
	//		//Read them in
	//		bsIn.Read((RakNet::MessageID)useTimeStamp);
	//		bsIn.Read(timeStamp);
	//		bsIn.Read((RakNet::MessageID)typeID);
	//		unsigned int offset = bsIn.GetReadOffset() / 8;
	//		//Parse into a CNetMsg
	//		CNetMsg msg = CNetMsg((char*)(packet->data + offset), (unsigned short)packet->length - offset, -1, (ulong)timeStamp, (ENetMsg)typeID);
	//		this->m_vMsg_List.push_back(msg);
	//
	//#if DB_NET_MSG_BLD
	//		printf("C : Parsing Msg of Type: %s & Size: %i.\n", ENetMsgStr(msg.GetType()), msg.GetWrite());
	//#endif
	//	}
	//	void CClient::handle_pong(const RakNet::Packet* packet)
	//	{
	//#if DB_NET_MSG_CNT
	//		printf("C : Recieved Pong.\n");
	//#endif
	//		if (this->m_bConnectOnBroadcast && !this->m_bConnected)
	//		{
	//			if (m_pRak->Connect(packet->systemAddress.ToString(), packet->systemAddress.GetPort(), 0, 0) != RakNet::CONNECTION_ATTEMPT_STARTED)
	//				this->stop();
	//		}
	//		else
	//		{
	//			bool found = false;
	//			ulong size = this->m_vPotentialServers.size();
	//			for (ulong i = 0; i < size; i++)
	//			if (this->m_vPotentialServers[i] == packet->systemAddress)
	//				found = true;
	//			this->m_vPotentialServers.push_back(packet->systemAddress);
	//		}
	//	}
	//	void CClient::handle_connection_accepted(const RakNet::Packet* packet)
	//	{
	//#if DB_NET_MSG_CNT
	//		printf("C : Connection Request Accepted.\n");
	//#endif
	//		if (this->m_bConnected)
	//			return;
	//
	//		//if (this->m_eConnectionType == EClientConnectionType::NET)
	//		//{
	//		//	
	//		//}
	//		//else
	//		//{
	//			this->m_bConnected = true;
	//			this->m_Server = packet->systemAddress;
	//			RakNet::TimeMS timeOut = LOST_TIME;
	//			m_pRak->SetTimeoutTime(timeOut, RakNet::UNASSIGNED_SYSTEM_ADDRESS);
	//			//Add Acceptance Msg
	//			CNetMsg msg;
	//			msg.SetType((ENetMsg)ID_CONNECTION_REQUEST_ACCEPTED);
	//			this->m_vMsg_List.push_back(msg);
	//		//}
	//	}
	//	/* Init */
	//	bool CClient::init(void)
	//	{
	//		//Error checking, if the rak peer interface is already initialized
	//		if (this->m_pRak != NULL)
	//			return false;
	//
	//		m_pRak = RakNet::RakPeerInterface::GetInstance();
	//		RakNet::SocketDescriptor sd;
	//		if (m_pRak->Startup(1, &sd, 1) != RakNet::RAKNET_STARTED)
	//		{
	//			RakNet::RakPeerInterface::DestroyInstance(this->m_pRak);
	//			this->m_pRak = nullptr;
	//			return false;
	//		}
	//		return true;
	//	}
	//	/* Broadcast */
	//	bool CClient::broadcast(const unsigned short port)
	//	{
	//		//Error checking, if the rak peer interface hasn't been initialized yet
	//		if (this->m_pRak == NULL)
	//			return false;
	//		//Error checking, if we are already connected to a server
	//		if (this->m_bConnected)
	//			return false;
	//
	//		this->m_vPotentialServers.clear();
	//		m_pRak = RakNet::RakPeerInterface::GetInstance();
	//		RakNet::SocketDescriptor sd;
	//		if (m_pRak->Startup(1, &sd, 1) != RakNet::RAKNET_STARTED)
	//		{
	//			RakNet::RakPeerInterface::DestroyInstance(this->m_pRak);
	//			this->m_pRak = nullptr;
	//			return false;
	//		}
	//		if (!this->m_pRak->Ping("255.255.255.255", port, false))
	//		{
	//			RakNet::RakPeerInterface::DestroyInstance(this->m_pRak);
	//			this->m_pRak = nullptr;
	//			return false;
	//		}
	//#if DB_NET_MSG_CNT
	//		printf("C : Pinging Port: %i.\n", port);
	//#endif
	//		this->m_bActive = true;
	//		return true;
	//	}
	//	/* Read_Msg */
	//	bool CClient::read_msg(void)
	//	{
	//		//Error checking, if the rak peer interface hasn't been initialized yet
	//		if (this->m_pRak == NULL)
	//			return false;
	//		//Error checking, if we are already connected to a server
	//		if (this->m_bConnected)
	//			return false;
	//
	//		RakNet::Packet* packet;
	//		for (packet = m_pRak->Receive(); packet; m_pRak->DeallocatePacket(packet), packet = m_pRak->Receive())
	//		{
	//			switch (packet->data[0])
	//			{
	//			case ID_UNCONNECTED_PONG:{
	//										 this->handle_pong(packet);
	//										 break; }
	//			case ID_CONNECTION_REQUEST_ACCEPTED:{
	//													this->handle_connection_accepted(packet);
	//													break; }
	//			case ID_NAT_PUNCHTHROUGH_SUCCEEDED:{
	//#if DB_NET_MSG_CNT
	//												   printf("C : Nat Punch Succeeded");
	//#endif
	//												  break; }
	//			case ID_NAT_PUNCHTHROUGH_FAILED:
	//			case ID_NAT_ALREADY_IN_PROGRESS:
	//			case ID_NAT_CONNECTION_TO_TARGET_LOST:
	//			case ID_NAT_TARGET_UNRESPONSIVE:
	//			case ID_NAT_TARGET_NOT_CONNECTED:{
	//#if DB_NET_MSG_CNT
	//												 printf("C : Nat Punch Failed");
	//#endif
	//												break; }
	//			case NM_CLIENT_ID:{
	//								  this->m_scID = packet->data[1];
	//#if DB_NET_MSG_CNT
	//								  printf("C : ID: %d\n", this->m_scID);
	//#endif
	//								  break; }
	//			case ID_CONNECTION_ATTEMPT_FAILED: {
	//#if DB_NET_MSG_CNT
	//												   printf("C : Connection Attempt Failed.\n");
	//#endif
	//												   this->stop();
	//												   //Add Failure Msg
	//												   CNetMsg msg;
	//												   msg.SetType((ENetMsg)ID_CONNECTION_ATTEMPT_FAILED);
	//												   this->m_vMsg_List.push_back(msg);
	//												   return false;
	//												   break; }
	//			case ID_NO_FREE_INCOMING_CONNECTIONS: {
	//#if DB_NET_MSG_CNT
	//													  printf("C : No Free Slots.\n");
	//#endif
	//													  this->stop();
	//													  //Add Failure Msg
	//													  CNetMsg msg;
	//													  msg.SetType((ENetMsg)ID_NO_FREE_INCOMING_CONNECTIONS);
	//													  this->m_vMsg_List.push_back(msg);
	//													  return false;
	//													  break; }
	//			case ID_DISCONNECTION_NOTIFICATION: {
	//#if DB_NET_MSG_CNT
	//													printf("C : Disconnected From Server.\n");
	//#endif
	//													this->stop();
	//													return false;
	//													break; }
	//			case ID_CONNECTION_LOST: {
	//#if DB_NET_MSG_CNT
	//										 printf("C : Connection Lost.\n");
	//#endif
	//										 this->stop();
	//										 return false;
	//										 break; }
	//			case ID_TIMESTAMP: {
	//#if DB_NET_MSG_SR
	//								   printf("C : Received Msg.\n");
	//#endif
	//								   this->parse_msg(packet);
	//								   break;	}
	//			default: {
	//#if DB_NET_MSG_SR
	//						 printf("C : Msg Identifier %i.\n", packet->data[0]);
	//#endif
	//						 break;	}
	//			}
	//		}
	//		return true;
	//	}
	//	/* Send_Msg */
	//	bool CClient::send_msg(const RakNet::BitStream* msg, const PacketPriority prio, const PacketReliability reli)
	//	{
	//		//Error checking, if the rak peer interface hasn't been initialized yet
	//		if (this->m_pRak == NULL)
	//			return false;
	//		//Error checking, if we are already connected to a server
	//		if (this->m_bConnected)
	//			return false;
	//
	//#if DB_NET_MSG_SR
	//		printf("C : Sending Msg.\n");
	//#endif
	//		return m_pRak->Send(msg, prio, reli, 0, this->m_Server, false) != 0;
	//	}
	//	/* Stop */
	//	void CClient::stop(void)
	//	{
	//		if (this->m_bActive)
	//		{
	//#if DB_NET_MSG_CNT
	//			printf("C : Closing Connection To Server.\n");
	//#endif
	//			//Disconnect from server
	//			m_pRak->CloseConnection(m_Server, true, 0);
	//			//Close RakPeerInterface
	//			RakNet::RakPeerInterface::DestroyInstance(this->m_pRak);
	//			this->m_pRak = NULL;
	//			this->m_bActive = false;
	//			this->m_bConnectOnBroadcast = false;
	//			this->m_bConnected = false;
	//			this->m_eConnectionType = LAN;
	//			this->m_scID = -1;
	//			this->m_vPotentialServers.clear();
	//		}
	//	}
	//
	//	void CClient::punch(const char* guid)
	//	{
	//		RakNet::RakNetGUID g;
	//		g.FromString(guid);
	//		this->m_pNatPunch->OpenNAT(g, this->m_Server);
	//	}
	//
	//	bool CClient::lan(const char* ip_address, const unsigned short port)
	//	{
	//		if (m_pRak->Connect(ip_address, port, 0, 0) != RakNet::CONNECTION_ATTEMPT_STARTED)
	//		{
	//			RakNet::RakPeerInterface::DestroyInstance(this->m_pRak);
	//			this->m_pRak = nullptr;
	//			return false;
	//		}
	//		this->m_bActive = true;
	//#if DB_NET_MSG_CNT
	//		printf("C : Attempting Connection to IP: %s at Port: %i.\n", ip_address, port);
	//#endif
	//		return true;
	//	}
	//	bool CClient::wan(const char* ip_address, const unsigned short port)
	//	{
	//		//Error checking, if the rak peer interface hasn't been initialized yet
	//		if (this->m_pRak == NULL)
	//			return false;
	//		//Error checking, if we are already connected to a server
	//		if (this->m_bConnected)
	//			return false;
	//
	//		//Nat punchthrough
	//		this->m_pNatPunch = RakNet::NatPunchthroughClient::GetInstance();
	//		this->m_pRak->AttachPlugin(this->m_pNatPunch);
	//		this->m_eConnectionType = EClientConnectionType::WAN;
	//
	//		//Attempt connection
	//		if (m_pRak->Connect(ip_address, port, 0, 0) != RakNet::CONNECTION_ATTEMPT_STARTED)
	//		{
	//			RakNet::RakPeerInterface::DestroyInstance(this->m_pRak);
	//			RakNet::NatPunchthroughClient::DestroyInstance(this->m_pNatPunch);
	//			this->m_pRak = NULL;
	//			this->m_pNatPunch = NULL;
	//			return false;
	//		}
	//
	//#if DB_NET_MSG_CNT
	//		printf("C : Attempting Connection to IP: %s at Port: %i.\n", ip_address, port);
	//#endif
	//
	//		this->m_bActive = true;
	//		return true;
	//	}
}