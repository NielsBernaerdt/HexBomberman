#include "stdafx.h"
#include "HexGrid.h"
#include "HexCell.h"
#include "HexUtils.h"
#include "Materials/BasicMaterial_Deferred.h"
#include "Materials/ColorMaterial.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow.h"

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

	//Ground Plane
	const auto pGroundMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pGroundMaterial->SetDiffuseTexture(L"Textures/Floor_diffuse.png");
	//pGroundMaterial->SetDiffuseTexture(L"Textures/GroundBrick.jpg");
	//***********
	const auto pGroundObj = AddChild(new GameObject());
	const auto pGroundModel = pGroundObj->AddComponent(new ModelComponent(L"Meshes/UnitPlane.ovm"));
	pGroundModel->SetMaterial(pGroundMaterial);
	pGroundObj->GetTransform()->Scale(1.85f, 1.0f, 1.55f);
	pGroundObj->GetTransform()->Translate(8.f, 0.f, 7.f);

	//WALLS//
	//Materials
	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.f, 0.f, 1.f);

	//Wall Left
	XMFLOAT2 pos{ -1.5f, -1.2f };
	for (int i{}; i < 16; ++i)
	{
		pos.y += 1.f;
		//Cube Object
		//const auto pCrateMaterial = MaterialManager::Get()->CreateMaterial<BasicMaterial_Deferred>();
		//pCrateMaterial->SetDiffuseMap(L"Textures/Crate_Diffuse.png");
		//pCrateMaterial->SetNormalMap(L"Textures/Crate_Normal.png");
		//pCrateMaterial->SetSpecularMap(L"Textures/Crate_Specular.png");
		const auto pCrateMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
		pCrateMaterial->SetDiffuseTexture(L"Textures/Wall_Diffuse.jpg");
		const auto pObject = AddChild(new GameObject);
		const auto pModel = pObject->AddComponent(new ModelComponent(L"Meshes/Bricks.ovm"));
		pModel->SetMaterial(pCrateMaterial);

		//Actor
		const auto explosionTrigger = pObject->AddComponent(new RigidBodyComponent(true));
		explosionTrigger->AddCollider(PxBoxGeometry{ 0.5f, 2.f, 0.5f }, *pDefaultMaterial);

		//SCALE
		pObject->GetTransform()->Scale(0.5f, 0.5f, 0.5f);
		pObject->GetTransform()->Translate(pos.x, 0.5f, pos.y);
	}
	//Wall Up
	for (int i{}; i < 19; ++i)
	{
		pos.x += 1.f;
		//Cube Object
		//const auto pCrateMaterial = MaterialManager::Get()->CreateMaterial<BasicMaterial_Deferred>();
		//pCrateMaterial->SetDiffuseMap(L"Textures/Crate_Diffuse.png");
		//pCrateMaterial->SetNormalMap(L"Textures/Crate_Normal.png");
		//pCrateMaterial->SetSpecularMap(L"Textures/Crate_Specular.png");
		const auto pCrateMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
		pCrateMaterial->SetDiffuseTexture(L"Textures/Wall_Diffuse.jpg");
		const auto pObject = AddChild(new GameObject);
		const auto pModel = pObject->AddComponent(new ModelComponent(L"Meshes/Bricks.ovm"));
		pModel->SetMaterial(pCrateMaterial);

		//Actor
		const auto explosionTrigger = pObject->AddComponent(new RigidBodyComponent(true));
		explosionTrigger->AddCollider(PxBoxGeometry{ 0.5f, 2.f, 0.5f }, *pDefaultMaterial);

		//SCALE
		pObject->GetTransform()->Scale(0.5f, 0.5f, 0.5f);
		pObject->GetTransform()->Translate(pos.x, 0.5f, pos.y);
	}
	//Wall RIght
	for (int i{}; i < 16; ++i)
	{
		pos.y -= 1.f;
		//Cube Object
		//const auto pCrateMaterial = MaterialManager::Get()->CreateMaterial<BasicMaterial_Deferred>();
		//pCrateMaterial->SetDiffuseMap(L"Textures/Crate_Diffuse.png");
		//pCrateMaterial->SetNormalMap(L"Textures/Crate_Normal.png");
		//pCrateMaterial->SetSpecularMap(L"Textures/Crate_Specular.png");
		const auto pCrateMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
		pCrateMaterial->SetDiffuseTexture(L"Textures/Wall_Diffuse.jpg");
		const auto pObject = AddChild(new GameObject);
		const auto pModel = pObject->AddComponent(new ModelComponent(L"Meshes/Bricks.ovm"));
		pModel->SetMaterial(pCrateMaterial);

		//Actor
		const auto explosionTrigger = pObject->AddComponent(new RigidBodyComponent(true));
		explosionTrigger->AddCollider(PxBoxGeometry{ 0.5f, 2.f, 0.5f }, * pDefaultMaterial);

		//SCALE
		pObject->GetTransform()->Scale(0.5f, 0.5f, 0.5f);
		pObject->GetTransform()->Translate(pos.x, 0.5f, pos.y);
	}
	//Wall Under
	for (int i{}; i < 19; ++i)
	{
		pos.x -= 1.f;
		//Cube Object
		//const auto pCrateMaterial = MaterialManager::Get()->CreateMaterial<BasicMaterial_Deferred>();
		//pCrateMaterial->SetDiffuseMap(L"Textures/Crate_Diffuse.png");
		//pCrateMaterial->SetNormalMap(L"Textures/Crate_Normal.png");
		//pCrateMaterial->SetSpecularMap(L"Textures/Crate_Specular.png");
		const auto pCrateMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
		pCrateMaterial->SetDiffuseTexture(L"Textures/Wall_Diffuse.jpg");
		const auto pObject = AddChild(new GameObject);
		const auto pModel = pObject->AddComponent(new ModelComponent(L"Meshes/Bricks.ovm"));
		pModel->SetMaterial(pCrateMaterial);

		//Actor
		const auto explosionTrigger = pObject->AddComponent(new RigidBodyComponent(true));
		explosionTrigger->AddCollider(PxBoxGeometry{ 0.5f, 2.f, 0.5f }, *pDefaultMaterial);

		//SCALE
		pObject->GetTransform()->Scale(0.5f, 0.5f, 0.5f);
		pObject->GetTransform()->Translate(pos.x, 0.5f, pos.y);
	}
}
