#define WIN32_LEAN_AND_MEAN
#include "NetMsg.h"
#include <Windows.h>
#include <assert.h>

namespace Network
{
	const char* ENetMsgStr(const ENetMsg eMsg)
	{
		switch (eMsg)
		{
#pragma region RAKNET_MSG_IDS
		case ID_CONNECTION_REQUEST_ACCEPTED:	return "ID_CONNECTION_REQUEST_ACCEPTED";
		case ID_CONNECTION_ATTEMPT_FAILED:		return "ID_CONNECTION_ATTEMPT_FAILED";
		case ID_NEW_INCOMING_CONNECTION:		return "ID_NEW_INCOMING_CONNECTION";
		case ID_DISCONNECTION_NOTIFICATION:		return "ID_DISCONNECTION_NOTIFICATION";
		case ID_CONNECTION_LOST:				return "ID_CONNECTION_LOST";
		case ID_UNCONNECTED_PING:				return "ID_UNCONNECTED_PING";
		case ID_UNCONNECTED_PONG:				return "ID_UNCONNECTED_PONG";
		case ID_TIMESTAMP:						return "ID_TIMESTAMP";
#pragma endregion RAKNET_MSG_IDS
#pragma region SELF_MSG_IDS
		case NM_DEFAULT_MSG:					return "NM_DEFAULT_MSG";
		case NM_CLIENT_ID:						return "NM_CLIENT_ID";
		case NM_LOBBY_ENTER:					return "NM_LOBBY_ENTER";
		case NM_LOBBY_DATA:						return "NM_LOBBY_DATA";
		case NM_CLIENT_DATA:					return "NM_CLIENT_DATA";
		case NM_TXTMSG:							return "NM_TXTMSG";
		case NM_SLOT_REQUEST:					return "NM_SLOT_REQUEST";
		case NM_BOARD:							return "NM_BOARD";
#pragma endregion
		}
		if (eMsg < NM_DEFAULT_MSG)
			return "UNKNOWN_RAK_MSG";
		else
			return "UNKOWN_SELF_MSG";
	}

	CNetMsg::CNetMsg()
	{
		ZeroMemory(m_cpData, NET_MSG_MAX_SIZE);
		this->m_usReadOffset = 0;
		this->m_usWritOffset = 0;
		this->m_eType = ENetMsg::NM_DEFAULT_MSG;
		this->m_tTimeStamp = 0;
	}
	CNetMsg::CNetMsg(const char* mem, const ushort len, const TPeer tPeer, const ulong stamp, const ENetMsg type)
	{
		ZeroMemory(m_cpData, NET_MSG_MAX_SIZE);
		this->m_usReadOffset = 0;
		this->m_usWritOffset = 0;
		this->m_tTimeStamp = stamp;
		this->m_tPeer = tPeer;
		this->m_eType = type;
		WriteRaw(mem, len);
	}
	CNetMsg::CNetMsg(ENetMsg eType)
	{
		ZeroMemory(m_cpData, NET_MSG_MAX_SIZE);
		this->m_usReadOffset = 0;
		this->m_usWritOffset = 0;
		this->m_tTimeStamp = 0;
		this->m_eType = eType;
	}

	uchar CNetMsg::ReadByte(void)
	{
		assert(this->m_usReadOffset + sizeof(unsigned char) < NET_MSG_MAX_SIZE && "CNetMsg::ReadByte - Buffer Overflow");

		unsigned char ret = *(unsigned char*)(&(this->m_cpData[this->m_usReadOffset]));
		this->m_usReadOffset += sizeof(unsigned char);
		return ret;
	}
	ushort CNetMsg::ReadShort(void)
	{
		assert(this->m_usReadOffset + sizeof(unsigned short) < NET_MSG_MAX_SIZE && "CNetMsg::ReadShort - Buffer Overflow");

		unsigned short ret = *(unsigned short*)(&(this->m_cpData[this->m_usReadOffset]));
		this->m_usReadOffset += sizeof(unsigned short);
		return ret;
	}
	ulong CNetMsg::ReadLong(void)
	{
		assert(this->m_usReadOffset + sizeof(unsigned long) < NET_MSG_MAX_SIZE && "CNetMsg::ReadLong - Buffer Overflow");

		unsigned long ret = *(unsigned long*)(&(this->m_cpData[this->m_usReadOffset]));
		this->m_usReadOffset += sizeof(unsigned long);
		return ret;
	}
	ulonglong CNetMsg::ReadLongLong(void)
	{
		assert(this->m_usReadOffset + sizeof(unsigned long long) < NET_MSG_MAX_SIZE && "CNetMsg::ReadLongLong - Buffer Overflow");

		unsigned long long ret = *(unsigned long long*)(&(this->m_cpData[this->m_usReadOffset]));
		this->m_usReadOffset += sizeof(unsigned long long);
		return ret;
	}
	float CNetMsg::ReadFloat(void)
	{
		assert(this->m_usReadOffset + sizeof(float) < NET_MSG_MAX_SIZE && "CNetMsg::ReadFloat - Buffer Overflow");

		float ret = *(float*)(&(this->m_cpData[this->m_usReadOffset]));
		this->m_usReadOffset += sizeof(float);
		return ret;
	}
	double CNetMsg::ReadDouble(void)
	{
		assert(this->m_usReadOffset + sizeof(double) < NET_MSG_MAX_SIZE && "CNetMsg::ReadDouble - Buffer Overflow");

		double ret = *(double*)(&(this->m_cpData[this->m_usReadOffset]));
		this->m_usReadOffset += sizeof(double);
		return ret;
	}
	bool CNetMsg::ReadBool(void)
	{
		assert(this->m_usReadOffset + sizeof(bool) < NET_MSG_MAX_SIZE && "CNetMsg::ReadBool - Buffer Overflow");

		bool ret = *(bool*)(&(this->m_cpData[this->m_usReadOffset]));
		this->m_usReadOffset += sizeof(bool);
		return ret;
	}
	char* CNetMsg::ReadString(void)
	{
		char* ret = (char*)(&(this->m_cpData[this->m_usReadOffset]));
		unsigned short len = strlen(ret) + 1;

		assert(this->m_usReadOffset + len < NET_MSG_MAX_SIZE && "CNetMsg::ReadString - Buffer Overflow");

		this->m_usReadOffset += len;
		return ret;
	}
	void CNetMsg::ReadRaw(char* buffer, const ushort len)
	{
		assert(this->m_usReadOffset + len < NET_MSG_MAX_SIZE && "CNetMsg::ReadRaw - Buffer Overflow");

		for (unsigned short i = 0; i < len; i++, this->m_usReadOffset += 1)
			buffer[i] = this->m_cpData[this->m_usReadOffset];
	}

	void CNetMsg::WriteByte(const uchar val)
	{
		assert(this->m_usWritOffset + sizeof(uchar) < NET_MSG_MAX_SIZE && "CNetMsg::WriteByte - Buffer Overflow");

		*(unsigned char*)(&(this->m_cpData[this->m_usWritOffset])) = val;
		this->m_usWritOffset += sizeof(unsigned char);
	}
	void CNetMsg::WriteShort(const ushort val)
	{
		assert(this->m_usWritOffset + sizeof(ushort) < NET_MSG_MAX_SIZE && "CNetMsg::WriteShort - Buffer Overflow");

		*(unsigned short*)(&(this->m_cpData[this->m_usWritOffset])) = val;
		this->m_usWritOffset += sizeof(unsigned short);
	}
	void CNetMsg::WriteLong(const ulong val)
	{
		assert(this->m_usWritOffset + sizeof(ulong) < NET_MSG_MAX_SIZE && "CNetMsg::WriteLong - Buffer Overflow");

		*(unsigned long*)(&(this->m_cpData[this->m_usWritOffset])) = val;
		this->m_usWritOffset += sizeof(unsigned long);
	}
	void CNetMsg::WriteLongLong(const ulonglong val)
	{
		assert(this->m_usWritOffset + sizeof(ulonglong) < NET_MSG_MAX_SIZE && "CNetMsg::WriteLongLong - Buffer Overflow");

		*(unsigned long long*)(&(this->m_cpData[this->m_usWritOffset])) = val;
		this->m_usWritOffset += sizeof(unsigned long long);
	}
	void CNetMsg::WriteFloat(const float val)
	{
		assert(this->m_usWritOffset + sizeof(float) < NET_MSG_MAX_SIZE && "CNetMsg::WriteFloat - Buffer Overflow");

		*(float*)(&(this->m_cpData[this->m_usWritOffset])) = val;
		this->m_usWritOffset += sizeof(float);
	}
	void CNetMsg::WriteDouble(const double val)
	{
		assert(this->m_usWritOffset + sizeof(double) < NET_MSG_MAX_SIZE && "CNetMsg::WriteDouble - Buffer Overflow");

		*(double*)(&(this->m_cpData[this->m_usWritOffset])) = val;
		this->m_usWritOffset += sizeof(double);
	}
	void CNetMsg::WriteBool(const bool val)
	{
		assert(this->m_usWritOffset + sizeof(bool) < NET_MSG_MAX_SIZE && "CNetMsg::WriteBool - Buffer Overflow");

		*(bool*)(&(this->m_cpData[this->m_usWritOffset])) = val;
		this->m_usWritOffset += sizeof(bool);
	}
	void CNetMsg::WriteString(const char* val)
	{
		unsigned int len = strlen(val);

		assert(this->m_usWritOffset + len < NET_MSG_MAX_SIZE && "CNetMsg::WriteString - Buffer Overflow");

		WriteRaw(val, len);
		WriteByte(0);
	}
	void CNetMsg::WriteRaw(const char* const buffer, const ushort len)
	{
		assert(this->m_usWritOffset + len < NET_MSG_MAX_SIZE && "CNetMsg::WriteRaw - Buffer Overflow");

		for (unsigned short i = 0; i < len; i++, this->m_usWritOffset += 1)
			this->m_cpData[this->m_usWritOffset] = buffer[i];
	}
}