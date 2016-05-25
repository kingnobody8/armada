/*
Author:			Daniel Habig
Date:			11-15-13
File:			IntroState.cpp
Purpose:		gamestate that begins the game, shows logos, transitions to StartState
*/
#include "LobbyStateServer.h"
using namespace Network;
using namespace Game;

CLobbyStateServer::CLobbyStateServer(void)
{
	this->m_eType = EGameStateType::gs_LOBBY_SERVER;
}
CLobbyStateServer::~CLobbyStateServer(void)
{
}

void CLobbyStateServer::Enter(Sprite::CSprite* pSprite, Input::CInput* pInput, Text::CText* pText, Audio::CAudio* pAudio)
{
	IGameState::Enter(pSprite, pInput, pText, pAudio);
	this->m_cServer.SetMaxClients(Game::kulMaxPlayers);
	this->m_cServer.init(SERVER_PORT);

	for (ulong i = 0; i < Game::kulMaxPlayers; ++i)
		this->m_vClientData[i] = Game::TClientData();
}
void CLobbyStateServer::Exit(void)
{
	IGameState::Exit();
	this->m_cServer.stop();
}

void CLobbyStateServer::Update(Time dDelta, Time dGameTime)
{
	this->m_cServer.read_msg();

	auto vec = this->m_cServer.GetMsgs();
	this->m_cServer.SetMsgsClear();
	ulong size = vec.size();
	for (ulong i = 0; i < size; ++i)
	{
		switch (vec[i].GetType())
		{
		case ID_CONNECTION_LOST:
		case ID_DISCONNECTION_NOTIFICATION:
			this->HandleDisconnect(vec[i]);
			break;
		case NM_LOBBY_ENTER:
			this->RecvLobbyEnter(vec[i]);
			break;
		case NM_CLIENT_DATA:
			this->RecvClientData(vec[i]);
			break;
		case NM_CLIENT_TYPE:
			this->RecvClientType(vec[i]);
			break;
		case NM_TXTMSG:
			this->RecvTxtMsg(vec[i]);
			break;
		case NM_SLOT_REQUEST:
			this->RecvSlotRequest(vec[i]);
			break;
		case NM_BOARD:
			this->RecvBoard(vec[i]);
			break;
		}
	}
}

void CLobbyStateServer::HandleDisconnect(Network::CNetMsg &msg)
{
	//Make txt msg
	std::pair < schar, std::string> pair;
	pair.first = -1;
	pair.second = this->m_vClientData[msg.GetPeer().m_scID].m_strName + std::string(" left.");
	this->m_vTxtMsgs.push_back(pair);

	this->m_cServer.disconnect(msg.GetPeer().m_scID);
	this->m_vClientData[msg.GetPeer().m_scID] = TClientData();
	this->SendLobbyData();
	this->SendTextMsg(false);
}
void CLobbyStateServer::SendLobbyData(void)
{
	CNetMsg msg = CNetMsg(Network::ENetMsg::NM_LOBBY_DATA);

	for (ulong i = 0; i < kulMaxPlayers; ++i)
	{
		msg.WriteString(this->m_vClientData[i].m_strName.c_str());
		msg.WriteByte(this->m_vClientData[i].m_eType);
		msg.WriteByte(this->m_vClientData[i].m_scColor);
	}
	this->m_cServer.send_msg(msg);
	//Reset the Broadcast msg
	CNetMsg broadcast;
	for (ulong i = 0; i < kulMaxPlayers; ++i)
	{
		broadcast.WriteString(this->m_vClientData[i].m_strName.c_str());
		broadcast.WriteByte(this->m_vClientData[i].m_eType);
	}
	this->m_cServer.SetBroadcastResponse(broadcast);
}
void CLobbyStateServer::SendTextMsg(bool bSendAll)
{
	CNetMsg msg(NM_TXTMSG);
	msg.WriteBool(bSendAll);
	ulong size = this->m_vTxtMsgs.size();
	if (bSendAll)
	{
		msg.WriteLong(size);
		for (ulong i = 0; i < size; ++i)
		{
			msg.WriteByte(this->m_vTxtMsgs[i].first);
			msg.WriteString(this->m_vTxtMsgs[i].second.c_str());
		}
	}
	else //Send just the most recent one
	{
		msg.WriteByte(this->m_vTxtMsgs[size - 1].first);
		msg.WriteString(this->m_vTxtMsgs[size - 1].second.c_str());
	}
	this->m_cServer.send_msg(msg);
}
void CLobbyStateServer::SendBoard(void)
{
	CNetMsg msg(NM_BOARD);
	this->m_cBoard.FillMsg(&msg);
	this->m_cServer.send_msg(msg);
}
void CLobbyStateServer::RecvLobbyEnter(CNetMsg &msg)
{
	Game::EPlayerType type = this->m_vClientData[msg.GetPeer().m_scID].m_eType;
	this->m_vClientData[msg.GetPeer().m_scID].m_strName = std::string("Player ") + std::to_string(msg.GetPeer().m_scID + 1);
	this->m_vClientData[msg.GetPeer().m_scID].m_eType = EPlayerType::CLIENT;
	//Move the client type into an open slot, if necessary
	if (type == Game::EPlayerType::COM || type == Game::EPlayerType::CLOSED)
	{
		for (ulong i = msg.GetPeer().m_scID; i < Game::kulMaxPlayers; ++i)
		{
			if (this->m_vClientData[i].m_eType == Game::EPlayerType::OPEN)
			{
				this->m_vClientData[i].m_eType = type;
				//break out
				i = Game::kulMaxPlayers;
			}
		}
	}

	//Make txt msg
	std::pair < schar, std::string> pair;
	pair.first = -1;
	pair.second = this->m_vClientData[msg.GetPeer().m_scID].m_strName + std::string(" joined.");
	this->m_vTxtMsgs.push_back(pair);

	//Resend the data to clients
	this->SendLobbyData();
	this->SendTextMsg(true);
	this->SendBoard();
	this->DetermineClientLock();
}
void CLobbyStateServer::RecvClientData(CNetMsg &msg)
{
	//Parse the Msg
	schar id = msg.GetPeer().m_scID;
	this->m_vClientData[id].m_strName = msg.ReadString();
	this->m_vClientData[id].m_eType = Game::EPlayerType(msg.ReadByte());
	this->m_vClientData[id].m_scColor = Game::EPlayerType(msg.ReadByte());
	//Update all the other clients
	this->SendLobbyData();
}
void CLobbyStateServer::RecvSlotRequest(Network::CNetMsg &msg)
{
	schar slot = msg.ReadByte();
	//Check if we already own it
	if (this->m_vClientData[msg.GetPeer().m_scID].m_scColor == slot)
	{
		this->m_vClientData[msg.GetPeer().m_scID].m_scColor = -1;
	}
	else
	{
		bool bTaken = false;
		for (uchar i = 0; i < Game::kulMaxPlayers; ++i)
		{
			if (this->m_vClientData[i].m_scColor == slot)
				bTaken = true;
		}
		if (!bTaken)
			this->m_vClientData[msg.GetPeer().m_scID].m_scColor = slot;
	}
	this->SendLobbyData();
}
void CLobbyStateServer::RecvClientType(Network::CNetMsg &msg)
{
	ulong index = msg.ReadByte();
	switch (this->m_vClientData[index].m_eType)
	{
	case Game::EPlayerType::OPEN:
		this->m_vClientData[index].m_eType = Game::EPlayerType::COM;
		break;
	case Game::EPlayerType::COM:
		this->m_vClientData[index].m_eType = Game::EPlayerType::CLOSED;
		break;
	case Game::EPlayerType::CLOSED:
		this->m_vClientData[index].m_eType = Game::EPlayerType::OPEN;
		break;
	}
	this->SendLobbyData();
	this->DetermineClientLock();
}
void CLobbyStateServer::RecvTxtMsg(Network::CNetMsg &msg)
{
	std::pair < schar, std::string> pair;
	pair.first = msg.GetPeer().m_scID;
	pair.second = msg.ReadString();
	this->m_vTxtMsgs.push_back(pair);
	this->SendTextMsg(false);
}
void CLobbyStateServer::RecvBoard(Network::CNetMsg &msg)
{
	//Reset the colors
	for (ulong i = 0; i < Game::kulMaxPlayers; i++)
		this->m_vClientData[i].m_scColor = -1;
	m_cBoard.LoadMsg(&msg);
	this->SendLobbyData();
	//Just resend the same msg (faster than remaking board msg)
	this->m_cServer.send_msg(msg);
}
void CLobbyStateServer::DetermineClientLock(void)
{
	bool lock = true;
	for (ulong i = 0; i < Game::kulMaxPlayers; ++i)
	{
		if (this->m_vClientData[i].m_eType == Game::EPlayerType::OPEN)
			lock = false;
	}
	this->m_cServer.lock(lock);
}

void CLobbyStateServer::Input(void)	{/* DO NOTHING */ }
void CLobbyStateServer::Render(void){/* DO NOTHING */ }
