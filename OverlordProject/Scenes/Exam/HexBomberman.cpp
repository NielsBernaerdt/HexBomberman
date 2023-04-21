#include "stdafx.h"
#include "HexBomberman.h"

#include "HexBomberman/HexGrid/HexGrid.h"
#include "HexBomberman/HexGrid/HexCell.h"

void HexBomberman::Initialize()
{
	m_pHexGrid = AddChild(new HexGrid{});
}

void HexBomberman::Update()
{
	if (InputManager::IsMouseButton(InputState::pressed, VK_RBUTTON))
	{
		if (const auto pPickedObject = m_SceneContext.pCamera->Pick())
		{
			const auto pPickedCell = pPickedObject->GetComponent<HexCell>();

			pPickedCell->StartExplosion();

			for(const auto neighbor : pPickedCell->GetTilesToExplode(blastRange))
			{
				if (neighbor == nullptr)
					continue;

				neighbor->StartExplosion();
			}
		}
	}
}