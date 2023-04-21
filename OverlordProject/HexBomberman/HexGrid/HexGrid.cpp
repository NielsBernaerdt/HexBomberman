#include "stdafx.h"
#include "HexGrid.h"
#include "HexCell.h"
#include "HexUtils.h"

void HexGrid::Initialize(const SceneContext&)
{
	const int totalColumns{ 10 };
	const int totalRows{ 10 };

	for (int rowIndex = 0, currentCellIndex = 0; rowIndex < totalRows; rowIndex++)
	{
		for (int columnIndex = 0; columnIndex < totalColumns; columnIndex++, currentCellIndex++)
		{
			//Create Cell Object & HexCell Component
			GameObject* cellObject = AddChild(new GameObject{});
			HexCell* cell = cellObject->AddComponent(new HexCell{});
			m_pGrid.push_back(cell);

			//Translate Cell Object
			XMFLOAT3 position;
			position.x = (columnIndex + rowIndex * 0.5f - rowIndex / 2) * (m_HexInnerRadius * 2.f);
			position.y = 0.f;
			position.z = rowIndex * (m_HexOuterRadius * 1.5f);
			cellObject->GetTransform()->Translate(position);

			//Assign Neighbors
			if (columnIndex > 0)
			{
				cell->SetNeighbor(HexDirection::W, m_pGrid[currentCellIndex - 1]);
			}
			if (rowIndex > 0)
			{
				if ((rowIndex & 1) == 0)
				{
					cell->SetNeighbor(HexDirection::SE, m_pGrid[currentCellIndex - totalColumns]);

					if (columnIndex > 0)
					{
						cell->SetNeighbor(HexDirection::SW, m_pGrid[currentCellIndex - totalColumns - 1]);
					}
				}
				else
				{
					cell->SetNeighbor(HexDirection::SW, m_pGrid[currentCellIndex - totalColumns]);

					if (columnIndex < totalColumns - 1)
					{
						cell->SetNeighbor(HexDirection::SE, m_pGrid[currentCellIndex - totalColumns + 1]);
					}
				}
			}
		}
	}
}