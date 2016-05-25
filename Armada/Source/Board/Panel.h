#pragma once
#include <vector>
#include "../Util/Types.h"
#include <fstream>
using namespace Type;

//Forward Declare
namespace Sprite	{ class CSprite; }
namespace Text		{ class CText; }
namespace Input		{ class CInput; }
namespace Network	{ class CNetMsg; }

namespace Board
{
	const uchar kusGridSize		= 9;
	const uchar kucMaxNumQSlots = 4;
	const _COLOR<uchar> kutPlayerColors[4] = { CLR::RED, CLR::GREEN, CLR::BLUE, CLR::YELLOW };
	const _COLOR<uchar> kutPlayerDarkColors[4] = { _COLOR<uchar>(128, 0, 0, 255), _COLOR<uchar>(0, 128, 0, 255), _COLOR<uchar>(0, 0, 128, 255), _COLOR<uchar>(128, 128, 0, 255) };


	class CTile
	{
	private:	//Data
		schar				m_scID;		//ID of entity that occupies this tile, -1 if unnoccupied
	private:	//Methods
	public:		//Methods
		CTile	( void );
		~CTile	( void );

		void Update	( Time dDelta );									//Updates the button
		void Render	( Sprite::CSprite* pSprite, Text::CText* pText );	//Renders the tile at it's location

		//Gets
		inline schar				GetID		( void )						const	{ return this->m_scID; }
		//Sets
		inline void					SetID		( const schar scID)						{ this->m_scID = scID; }
	};

	class CPanel
	{
	private:	//Data
		CTile				m_vGrid[kusGridSize];		//Grid of tiles
		std::vector<schar>	m_vSlots;					//Slots for quantum cubes (value represents owner of slot) (if no slots, then panel is a void panel [no planet])
		bool				m_bActive;					//Is this panel a part of the map

	private:	//Methods
	public:		//Methods
		CPanel(void);
		~CPanel(void);

		void Input(Input::CInput* pInput);
		void Update(Time dDelta);
		void Render(Sprite::CSprite* pSprite, Text::CText* pText);
		void SavePanel(std::ofstream &fout);
		void LoadPanel(std::ifstream &fin);
		ulong LoadPanel(const char* pBuffer);

		//Gets
		inline std::vector<schar>	GetSlots	( void )				const			{ return this->m_vSlots; }
		inline CTile				GetGrid		( const schar index )	const			{ return (index >= 0 && index <= kusGridSize) ? m_vGrid[index] : CTile(); }
		inline bool					GetActive	( void )				const			{ return this->m_bActive; }

		//Sets
		inline void					SetSlots	( const std::vector<schar> vSlots )		{ this->m_vSlots = vSlots; }
		inline void					SetGrid		( const schar index, const CTile tTile ){ if (index >= 0 && index <= 9) { this->m_vGrid[index] = tTile; } }
		inline void					SetActive	( const bool bActive )					{ this->m_bActive = bActive; }

		void FillMsg(Network::CNetMsg* pMsg);
		void LoadMsg(Network::CNetMsg* pMsg);

	};

	inline bool operator==(const CPanel& lhs, const CPanel& rhs)
	{
		if (lhs.GetActive() != rhs.GetActive())
			return false;
		for (uchar i = 0; i < kucMaxNumQSlots; i++)
		{
			if (lhs.GetGrid(i).GetID() != rhs.GetGrid(i).GetID())
				return false;
		}
		if (lhs.GetSlots() != rhs.GetSlots())
			return false;
		return true;
	}
	inline bool operator!=(const CPanel& lhs, const CPanel& rhs){ return !operator==(lhs, rhs); }
	inline bool operator< (const CPanel& lhs, const CPanel& rhs){ return lhs.GetSlots().size() < rhs.GetSlots().size(); }
	inline bool operator> (const CPanel& lhs, const CPanel& rhs){ return  operator< (rhs, lhs); }
	inline bool operator<=(const CPanel& lhs, const CPanel& rhs){ return !operator> (lhs, rhs); }
	inline bool operator>=(const CPanel& lhs, const CPanel& rhs){ return !operator< (lhs, rhs); }
}