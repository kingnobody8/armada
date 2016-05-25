#pragma once
#include "Panel.h"
#include "../Util/Types.h"
#include <fstream>
using namespace Type;

//Forward Declare
namespace Input { class CInput; }
namespace Network { class CNetMsg; }

namespace Board
{
	const slong kslBoardSize = 8;

	class CBoard
	{
	private:	//Data
	protected:	//Data
		CPanel						m_vPanel[kslBoardSize][kslBoardSize];
		schar						m_scWinAmount;
		std::string					m_strMapName;
		_RECT<slong>				m_tRect;
		bool						m_bDefault;

	private:	//Methods
	protected:	//Methods
	public:		//Methods
		CBoard(void);
		~CBoard(void);

		virtual void Input		( Input::CInput* pInput );
		virtual void Update		( Time dDelta );
		virtual void Render		( Sprite::CSprite* pSprite, Text::CText* pText );
				void SaveBoard	( std::ofstream& fout );
				void LoadBoard	( std::ifstream& fin );
				ulong LoadBoard(const char* pBuffer);



		//Gets
		inline CPanel			GetPanel(const uchar ucX, const uchar ucY) const { return this->m_vPanel[ucX][ucY]; }
		inline _RECT<slong>		GetRect(void) const { return this->m_tRect; }
		inline schar			GetWinAmount(void) const { return this->m_scWinAmount; }
		inline std::string		GetMapName(void) const { return this->m_strMapName; }
		inline bool				GetDefault(void) const { return this->m_bDefault; }
		//Sets
		inline void	SetPanel(const CPanel cPanel, const uchar ucX, const uchar ucY) { this->m_vPanel[ucX][ucY] = cPanel; }
		inline void	SetRect(const _RECT<slong> tRect) { this->m_tRect = tRect; }
		inline void	SetWinAmount(const schar scWinAmount) { this->m_scWinAmount = scWinAmount; }
		inline void SetMapName(const std::string strMapName) { this->m_strMapName = strMapName; }
		inline void SetDefault(const bool bDefault) { this->m_bDefault = bDefault; }

		void FillMsg(Network::CNetMsg* pMsg);
		void LoadMsg(Network::CNetMsg* pMsg);

		//Shared
		void SetDimensions(Type2<slong> offset, Type2<slong> tile_size, Type2<slong> tile_space, Type2<slong> panel_space);

	};

	//For Sorting
	inline bool operator==(const CBoard& lhs, const CBoard& rhs)
	{
		if (lhs.GetDefault() != rhs.GetDefault())
			return false;
		if (lhs.GetMapName() != rhs.GetMapName())
			return false;
		if (lhs.GetRect() != rhs.GetRect())
			return false;
		for (uchar x = 0; x < kslBoardSize; x++)
		{
			for (uchar y = 0; y < kslBoardSize; y++)
			{
				if (lhs.GetPanel(x, y) != rhs.GetPanel(x, y))
					return false;
			}
		}
		if (lhs.GetWinAmount() != rhs.GetWinAmount())
			return false;
		return true;
	}
	inline bool operator!=(const CBoard& lhs, const CBoard& rhs){ return !operator==(lhs, rhs); }
	inline bool operator< (const CBoard& lhs, const CBoard& rhs){ return lhs.GetMapName() < rhs.GetMapName(); }
	inline bool operator> (const CBoard& lhs, const CBoard& rhs){ return  operator< (rhs, lhs); }
	inline bool operator<=(const CBoard& lhs, const CBoard& rhs){ return !operator> (lhs, rhs); }
	inline bool operator>=(const CBoard& lhs, const CBoard& rhs){ return !operator< (lhs, rhs); }
}