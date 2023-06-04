#include "stdafx.h"
#include "HexUtils.h"
#include "HexGrid.h"
#include "HexCell.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow.h"

void HexGrid::Initialize(const SceneContext&)
{
	ConstructGrid();

	//Ground Plane
	const auto pGroundMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pGroundMaterial->SetDiffuseTexture(L"Textures/Floor_diffuse.png");
	//***********
	const auto pGroundObj = AddChild(new GameObject());
	const auto pGroundModel = pGroundObj->AddComponent(new ModelComponent(L"Meshes/UnitPlane.ovm"));
	pGroundModel->SetMaterial(pGroundMaterial);
	pGroundObj->GetTransform()->Scale(1.85f, 1.0f, 1.55f);
	pGroundObj->GetTransform()->Translate(8.f, 0.f, 7.f);

	ConstructWalls();
}

void HexGrid::ConstructGrid()
{
	for (int rowIndex = 0, currentCellIndex = 0; rowIndex < m_TotalRows; rowIndex++)
	{
		for (int columnIndex = 0; columnIndex < m_TotalColumns; columnIndex++, currentCellIndex++)
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
					cell->SetNeighbor(HexDirection::SE, m_pGrid[currentCellIndex - m_TotalColumns]);

					if (columnIndex > 0)
					{
						cell->SetNeighbor(HexDirection::SW, m_pGrid[currentCellIndex - m_TotalColumns - 1]);
					}
				}
				else
				{
					cell->SetNeighbor(HexDirection::SW, m_pGrid[currentCellIndex - m_TotalColumns]);

					if (columnIndex < m_TotalColumns - 1)
					{
						cell->SetNeighbor(HexDirection::SE, m_pGrid[currentCellIndex - m_TotalColumns + 1]);
					}
				}
			}
		}
	}
}

void HexGrid::ConstructWalls()
{
	//Materials
	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.f, 0.f, 1.f);
	//DiffuseMaterial
	const auto pCrateMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pCrateMaterial->SetDiffuseTexture(L"Textures/Wall_Diffuse.jpg");

	//Wall Left
	const int nrBlocksWidth{ 19 };
	const int nrBlocksHeight{ 16 };
	XMFLOAT2 pos{ -1.5f, -1.2f };
	for (int i{}; i < nrBlocksHeight; ++i)
	{
		pos.y += 1.f;
		AddWall(pos, pDefaultMaterial, pCrateMaterial);
	}
	//Wall Up
	for (int i{}; i < nrBlocksWidth; ++i)
	{
		pos.x += 1.f;
		AddWall(pos, pDefaultMaterial, pCrateMaterial);
	}
	//Wall Right
	for (int i{}; i < nrBlocksHeight; ++i)
	{
		pos.y -= 1.f;
		AddWall(pos, pDefaultMaterial, pCrateMaterial);
	}
	//Wall Under
	for (int i{}; i < nrBlocksWidth; ++i)
	{
		pos.x -= 1.f;
		AddWall(pos, pDefaultMaterial, pCrateMaterial);
	}
}

void HexGrid::AddWall(const XMFLOAT2& pos, const PxMaterial* pDefaultMaterial, DiffuseMaterial_Shadow* pCrateMaterial)
{
	//Object
	const auto pObject = AddChild(new GameObject);
	const auto pModel = pObject->AddComponent(new ModelComponent(L"Meshes/Bricks.ovm"));
	pModel->SetMaterial(pCrateMaterial);

	//Actor
	const auto pActor = pObject->AddComponent(new RigidBodyComponent(true));
	pActor->AddCollider(PxBoxGeometry{ 0.5f, 2.f, 0.5f }, *pDefaultMaterial);

	//Scale
	pObject->GetTransform()->Scale(0.5f, 0.5f, 0.5f);
	pObject->GetTransform()->Translate(pos.x, 0.5f, pos.y);
}