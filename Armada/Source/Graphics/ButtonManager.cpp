#include "ButtonManager.h"
#include "../Util/Collision.h"
using namespace Collision;
using namespace Input;

namespace GUI
{
	CButtonManager::CButtonManager(void)
	{
		this->m_slSelectedIndex = 0;
	}
	CButtonManager::~CButtonManager(void)
	{

	}

	void CButtonManager::Selection(Input::CInput* pInput)
	{
		if (pInput->Ctrl_Push(CTRL_KEYS::PLUGIN) || pInput->Ctrl_Pull(CTRL_KEYS::PLUGIN))
			this->ClearInput();

		Input::eInputType eType = pInput->GetFinalInputType();
		if (eType == MOUSE)
		{
			ulong size = this->m_vGroups.size();
			for (ulong i = 0; i < size; i++)
				this->m_vGroups[i].Selection(Input::eInputType::MOUSE);
		}
		else
		{
			ulong size = this->m_vGroups.size();
			for (ulong i = 0; i < size; i++)
			{
				if (i == this->m_slSelectedIndex)
					continue;
				this->m_vGroups[i].UnSelect();
			}
			slong result = this->m_vGroups[this->m_slSelectedIndex].Selection(Input::eInputType::CTRL_KEYBOARD);
			if (result == -1)
				return;

			this->MoveGroups(eDIRECTION(result));
			this->m_vGroups[this->m_slSelectedIndex].SetCtrlStickTimer(kCtrlStickRate);

			//-------------------------

			//slong index = this->m_vGroups[this->m_slSelectedIndex].GetButtonGroupDir(eDIRECTION(result));
			//if (index != -1)
			//{
			//	switch (this->m_vGroups[index].GetMoveType())
			//	{
			//	case CButton::EMoveType::CLOSEST:
			//		this->FindClosestButton(index, eDIRECTION(result));
			//		break;
			//	case CButton::EMoveType::PREVIOUS:
			//		break;
			//	case CButton::EMoveType::RESET:
			//		this->m_vGroups[index].SetSelectedIndex(0);
			//		break;
			//	}
			//
			//	this->m_slSelectedIndex = index;
			//}
		}
	}
	void CButtonManager::Update(Time dDelta, Audio::CAudio* pAudio)
	{
		ulong size = this->m_vGroups.size();
		for (ulong i = 0; i < size; i++)
			this->m_vGroups[i].Update(dDelta, pAudio);
	}

	void CButtonManager::FindClosestButton(slong group, eDIRECTION eDir)
	{
		if (group == -1)
		{
			ulong size = this->m_vGroups.size();
			for (ulong i = 0; i < size; i++)
			{
				if (i != this->m_slSelectedIndex)
					this->FindClosestButton(i, eDir);
			}
			return;
		}

		std::vector<CButton*> alpha = this->m_vGroups[this->m_slSelectedIndex].GetButtonList();
		std::vector<CButton*> omega = this->m_vGroups[group].GetButtonList();

		CButton* pBut = alpha[this->m_vGroups[this->m_slSelectedIndex].GetSelectedIndex()];

		float distance = -1.0f;
		slong index = -1;

		_RECT<slong> rect = pBut->GetRect();

		ulong size = omega.size();
		for (ulong i = 0; i < size; i++)
		{
			if (omega[i]->GetActive() == false)
				continue;

			_RECT<slong> tempR = omega[i]->GetRect();
			Type2<float> between = Type2<float>(float(tempR.GetPos().x - rect.GetPos().x), float(tempR.GetPos().y - rect.GetPos().y));

			if (between.magnitude() < distance || index == -1)
			{
				bool bConfirm = false;
				switch (eDir)
				{
				case UP:
					bConfirm = between.y > 0.0f;
					break;
				case DOWN:
					bConfirm = between.y < 0.0f;
					break;
				case LEFT:
					bConfirm = between.x < 0.0f;
					break;
				case RIGHT:
					bConfirm = between.x > 0.0f;
					break;
				}
				if (bConfirm)
				{
					distance = between.magnitude();
					index = i;
				}
			}
		}

		if (index == -1)
			this->m_vGroups[group].SetSelectedIndex(0);
		else
			this->m_vGroups[group].SetSelectedIndex(index);
	}
	void CButtonManager::MoveGroups(eDIRECTION eDir)
	{
		slong index = this->m_slSelectedIndex;

		//Check if the button overrides the normal movement
		int slGroup = this->m_vGroups[index].GetButtonList()[this->m_vGroups[index].GetSelectedIndex()]->GetButtonGroupDir(eDir);
		if (slGroup == -2)
			return;
		if (slGroup == -1)
		{
			//Check if the group overrides the normal movement
			if (!this->m_vGroups[index].GetFindClosestButtonGroup())
			{
				do
				{
					index = this->m_vGroups[index].GetButtonGroupDir(eDir);
					if (index != -1)
					{
						switch (this->m_vGroups[index].GetMoveType())
						{
						case CButton::EMoveType::CLOSEST:
							this->FindClosestButton(index, eDir);
							break;
						case CButton::EMoveType::PREVIOUS:
							//If the previous button is now inactive, then just find the closest one
							if (!this->m_vGroups[index].GetButtonList()[this->m_vGroups[index].GetSelectedIndex()]->GetActive())
								this->FindClosestButton(index, eDir);
							break;
						case CButton::EMoveType::RESET:
							this->m_vGroups[index].SetSelectedIndex(0);
							break;
						}
					}
					else
					{
						break;
					}
				} while (!this->m_vGroups[index].GetActive());
			}
			else
			{
				//Set all groups' selected index
				this->FindClosestButton(-1, eDir);

				//Make list of all groups selected indices
				std::vector<CButton*> vSelected;
				ulong size = this->m_vGroups.size();
				for (ulong i = 0; i < size; i++)
				{
					if (i != this->m_slSelectedIndex)
						vSelected.push_back(this->m_vGroups[i].GetButtonList()[this->m_vGroups[i].GetSelectedIndex()]);
					else
						vSelected.push_back(NULL);
				}

				//Parse through list to find the closest button
				CButton* pBut = this->m_vGroups[this->m_slSelectedIndex].GetButtonList()[this->m_vGroups[this->m_slSelectedIndex].GetSelectedIndex()];
				_RECT<slong> tRect = pBut->GetRect();
				float distance = -1.0f;
				index = -1;
				for (ulong i = 0; i < size; i++)
				{
					if (vSelected[i] == NULL || vSelected[i]->GetActive() == false)
						continue;

					_RECT<slong> tempR = vSelected[i]->GetRect();
					Type2<float> between = Type2<float>(float(tempR.GetPos().x - tRect.GetPos().x), float(tempR.GetPos().y - tRect.GetPos().y));

					if (between.magnitude() < distance || index == -1)
					{
						bool bConfirm = false;
						switch (eDir)
						{
						case UP:
							bConfirm = between.y > 0.0f;
							break;
						case DOWN:
							bConfirm = between.y < 0.0f;
							break;
						case LEFT:
							bConfirm = between.x < 0.0f;
							break;
						case RIGHT:
							bConfirm = between.x > 0.0f;
							break;
						}
						if (bConfirm)
						{
							distance = between.magnitude();
							index = i;
						}
					}
				}
			}
		}
		else
		{
			index = slGroup;
			bool first = true;
			do
			{
				if (!first)
					index = this->m_vGroups[index].GetButtonGroupDir(eDir);
				first = false;
				if (index != -1)
				{
					switch (this->m_vGroups[index].GetMoveType())
					{
					case CButton::EMoveType::CLOSEST:
						this->FindClosestButton(index, eDir);
						break;
					case CButton::EMoveType::PREVIOUS:
						break;
					case CButton::EMoveType::RESET:
						this->m_vGroups[index].SetSelectedIndex(0);
						break;
					}
				}
				else
				{
					break;
				}

			} while (!this->m_vGroups[index].GetActive());
		}

		if (index != -1)
			this->m_slSelectedIndex = index;
	}

	void CButtonManager::ClearInput(void)
	{
		ulong size = this->m_vGroups.size();
		for (ulong i = 0; i < size; i++)
			this->m_vGroups[i].ClearInput();
	}
}