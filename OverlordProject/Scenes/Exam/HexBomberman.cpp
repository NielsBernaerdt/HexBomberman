#include "stdafx.h"
#include "HexBomberman.h"

#include "HexGrid/HexCell.h"
#include "HexGrid/HexGrid.h"

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

			pPickedCell->SpawnCrate();

			for(const auto neighbor : pPickedCell->GetBlastTiles(blastRange))
			{
				if (neighbor == nullptr)
					continue;

				neighbor->SpawnCrate();
			}
		}
	}
}