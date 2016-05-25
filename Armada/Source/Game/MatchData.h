#pragma once
#include "../Util/Types.h"
#include <string>
using namespace Type;

namespace Game
{
	const ulong kulMaxPlayers = 4;
	enum EPlayerType { CLIENT, COM, OPEN, CLOSED, NUM_CLIENT_SLOT_TYPES };
	struct TClientData
	{
		std::string		m_strName;
		EPlayerType		m_eType;
		schar			m_scColor;
		TClientData(void)
		{
			this->m_eType = EPlayerType::OPEN;
			this->m_scColor = -1;
		}
	};
}