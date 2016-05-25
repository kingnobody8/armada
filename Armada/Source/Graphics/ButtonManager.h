#pragma once
#include <vector>
#include "../Input/InputCMD.h"
#include "ButtonGroup.h"

namespace GUI
{
	//Used for controller input and moving between buttons
	class CButtonManager
	{
	private:	//Data
		std::vector<CButtonGroup>	m_vGroups;
		slong						m_slSelectedIndex;

	private:	//Methods
		void FindClosestButton(slong group, eDIRECTION eDir);
	public:		//Methods
		CButtonManager(void);
		~CButtonManager(void);

		void Selection(Input::CInput* pInput);
		void Update(Time dDelta, Audio::CAudio* pAudio);
		void MoveGroups(eDIRECTION eDir);
		void ClearInput(void);

		//Gets
		inline std::vector<CButtonGroup> GetGroups(void) const { return this->m_vGroups; }
		inline slong GetSelectedIndex(void) const { return this->m_slSelectedIndex; }
		//Sets
		inline void SetGroups(const std::vector<CButtonGroup> vGroups) { this->m_vGroups = vGroups; }
		inline void SetSelectedIndex(const slong slSelectedIndex) { this->m_slSelectedIndex = slSelectedIndex; }
	};
}