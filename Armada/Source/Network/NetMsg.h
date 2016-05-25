#pragma once
#include "Network.h"
#include <MessageIdentifiers.h>

#define NET_MSG_MAX_SIZE	2048

namespace Network
{
	enum ENetMsg
	{
		NM_DEFAULT_MSG = DefaultMessageIDTypes::ID_USER_PACKET_ENUM + 1,

		/* Used for server/client connection */
		NM_CLIENT_ID,	/* tells the client what it's slot id is [ schar id ]							*/
		/* Used in Lobby */
		NM_LOBBY_ENTER,	/* Informs the server that this client is now in the lobby (SetupState) [ ]		*/	
		NM_LOBBY_DATA,	/* Info on each client and what board is selected [ tClientData vNames[4] ]		*/
		NM_CLIENT_DATA, /* Info on a specific client, sent from client to server [ tClientData data	]	*/
		NM_CLIENT_TYPE, /* Tells the server to adjust the client type of a slot [ ]						*/	
		NM_TXTMSG,		/* A string that will be reciprocated to all clients [ string ]					*/	
		NM_SLOT_REQUEST,/* Asks the server for a player to be a certain color [ schar id ]				*/		
		NM_BOARD,		/* Tells the server & clients what board we are using [ CBoard ]				*/
	};
	const char* ENetMsgStr(const ENetMsg eMsg);

	class CNetMsg
	{
	private:
		char				m_cpData[NET_MSG_MAX_SIZE];
		ushort				m_usReadOffset;
		ushort				m_usWritOffset;
		TPeer				m_tPeer;
		ENetMsg				m_eType;
		RakNet::Time		m_tTimeStamp;

	public:
		CNetMsg		( void );
		CNetMsg		( const char* mem, const ushort len, const TPeer tPeer, const ulong stamp, const ENetMsg type);
		CNetMsg		( const ENetMsg eType );

		//Accessors
		inline ENetMsg		GetType			( void ) const  { return this->m_eType; }
		inline TPeer		GetPeer			( void ) const  { return this->m_tPeer; }
		inline RakNet::Time	GetTime			( void ) const  { return this->m_tTimeStamp; }
		inline ushort		GetRead			( void ) const	{ return this->m_usReadOffset; }
		inline ushort		GetWrite		( void ) const	{ return this->m_usWritOffset; }
		inline char*		GetData			( void )		{ return this->m_cpData; }

		//Reads a type and returns it, also increments the read_ptr by the size of that type
		uchar				ReadByte		( void );
		ushort				ReadShort		( void );
		ulong				ReadLong		( void );
		ulonglong			ReadLongLong	( void );
		float				ReadFloat		( void );
		double				ReadDouble		( void );
		bool				ReadBool		( void );
		char*				ReadString		( void );
		void				ReadRaw			( char* buffer, const ushort len);

		//Mutators
		inline void			SetType			( const ENetMsg eType )			{ this->m_eType = eType; }
		inline void			SetPeer			( const TPeer tPeer )			{ this->m_tPeer = tPeer; }
		inline void			SetTime			( const RakNet::Time tTimeStamp )		{ this->m_tTimeStamp = tTimeStamp; }
		inline void			SetRead			( const ushort usReadOffset )	{ this->m_usReadOffset = usReadOffset; }
		inline void			SetWrite		( const ushort usWritOffset )	{ this->m_usReadOffset = usWritOffset; }

		//Writes a type, also increments the write_ptr by the size of that type
		void				WriteByte		( const uchar val );
		void				WriteShort		( const ushort val );
		void				WriteLong		( const ulong val );
		void				WriteLongLong	( const ulonglong val );
		void				WriteFloat		( const float val );
		void				WriteDouble		( const double val );
		void				WriteBool		( const bool val );
		void				WriteString		( const char* val );
		void				WriteRaw		( const char* const buffer, const ushort len );
	};

	//For sorting by GUID
	inline bool operator==(const CNetMsg& lhs, const CNetMsg& rhs)
	{
		if (lhs.GetPeer().m_Guid != rhs.GetPeer().m_Guid)
			return false;
		if (lhs.GetWrite() != rhs.GetWrite())
			return false;
		return true;
	}
	inline bool operator!=(const CNetMsg& lhs, const CNetMsg& rhs){ return !operator==(lhs, rhs); }
	inline bool operator< (const CNetMsg& lhs, const CNetMsg& rhs){ return lhs.GetPeer().m_Guid < rhs.GetPeer().m_Guid; }
	inline bool operator> (const CNetMsg& lhs, const CNetMsg& rhs){ return  operator< (rhs, lhs); }
	inline bool operator<=(const CNetMsg& lhs, const CNetMsg& rhs){ return !operator> (lhs, rhs); }
	inline bool operator>=(const CNetMsg& lhs, const CNetMsg& rhs){ return !operator< (lhs, rhs); }
}