#include "stdafx.h"
#include "HexCell.h"

#include "HexUtils.h"
#include "HexBomberman/Gameplay/Bomb.h"
#include "HexBomberman/Player/PlayerPawn.h"
#include "HexBomberman/PowerUps/Crate.h"
#include "Prefabs/SpherePrefab.h"
#include "HexBomberman/PowerUps/BasePowerUp.h"

void HexCell::Initialize(const SceneContext&)
{
	//Materials
	const auto pMaterial = PxGetPhysics().createMaterial(0.f, 0.f, 1.f);

	//Ground Tile
	AddHexComp();
	
	//Actor
	const auto pRigidBody = m_pGameObject->AddComponent(new RigidBodyComponent(true));
	pRigidBody->AddCollider(PxSphereGeometry{ 1.f }, *pMaterial, true);
}

void HexCell::Update(const SceneContext&)
{
	if(m_UpdateOnce == true) //Because the transform of the Hexcell is not yet updated during intialization
	{
		m_UpdateOnce = false;

		//Crate
		if (std::rand() % 3 != 0) //2 / 3 chance to spawn crate
		{
			m_HasCrate = true;
			const auto pCrateObject = m_pGameObject->AddChild(new GameObject{});
			m_pCrateComponent = pCrateObject->AddComponent(new Crate{});
			m_pCrateComponent->GetTransform()->Translate(0.f, 0.5f, 0.f);
		}
	}
}


HexCell* HexCell::GetNeighbor(HexDirection direction) const
{
	return m_pNeighbors[static_cast<int>(direction)];
}

void HexCell::SetNeighbor(HexDirection direction, HexCell* cell)
{
	m_pNeighbors[static_cast<int>(direction)] = cell;
	cell->GetNeighbors()[static_cast<int>(HexDirectionExtensions::Opposite(direction))] = this;
}

void HexCell::PlaceBomb(PlayerPawn* pPlayer, int blastRange)
{
	//Bomb Object
	const auto bombObject = m_pGameObject->AddChild(new GameObject{});
	bombObject->AddComponent(new Bomb{this, pPlayer, blastRange});
	bombObject->GetTransform()->Translate(0.f, 0.5f, 0.f);
	//bombObject->GetTransform()->Scale(1.1f, 1.1f, 1.1f);
}

void HexCell::DestroyCrate()
{
	m_HasCrate = false;
	m_pCrateComponent->SpawnPowerUp(this);
	m_pGameObject->RemoveChild(m_pCrateComponent->GetGameObject(), true);
}

void HexCell::AddPowerUp(BasePowerUp* pPowerUp)
{
	m_HasPowerUp = true;
	m_pPowerUp = pPowerUp;
}

void HexCell::CollectPowerUp(PlayerPawn* pPlayer)
{
	m_HasPowerUp = false;
	m_pPowerUp->CollectPowerUp(pPlayer);
	m_pGameObject->RemoveChild(m_pPowerUp->GetGameObject(), true);
}

std::vector<HexCell*> HexCell::GetTilesToExplode(int length) const
{
	const std::vector<HexCell*>::size_type maxNrTilesToExplode = 6 * length;
	std::vector<HexCell*> blastTiles{ maxNrTilesToExplode };

	for(int currentHexDirection{}; currentHexDirection < m_pNeighbors.size(); ++currentHexDirection)
	{
		HexCell* currentCell = m_pNeighbors[currentHexDirection];
		if (currentCell == nullptr)
		{
			continue;
		}

		blastTiles.push_back(currentCell);
		if (currentCell->HasCrate() == true)
		{
			continue;
		}

		for (int i{}; i < length - 1; ++i)
		{
			currentCell = currentCell->GetNeighbor(static_cast<HexDirection>(currentHexDirection));
			if (currentCell == nullptr)
			{
				break;
			}

			blastTiles.push_back(currentCell);

			if (currentCell->HasCrate() == true)
			{
				break;
			}
		}
	}

	return blastTiles;
}

void HexCell::AddHexComp()
{
	//Create Cube
	const auto pMesh = new MeshDrawComponent(12);

	//RIGHTUNDER -> RIGHTUPPER
	pMesh->AddTriangle(
		VertexPosNormCol(XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 1), currentCellColor),
		VertexPosNormCol(XMFLOAT3(cos(XM_PI/6), 0, sin(-XM_PI/6)), XMFLOAT3(0, 0, 1), currentCellColor),
		VertexPosNormCol(XMFLOAT3(cos(XM_PI/6), 0, sin(XM_PI/6)), XMFLOAT3(0, 0, 1), currentCellColor)
	);
	//RIGHTUPPER -> UPPER
	pMesh->AddTriangle(
		VertexPosNormCol(XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 1), currentCellColor),
		VertexPosNormCol(XMFLOAT3(cos(XM_PI/6), 0, sin(XM_PI/6)), XMFLOAT3(0, 0, 1), currentCellColor),
		VertexPosNormCol(XMFLOAT3(cos(XM_PI/2), 0, sin(XM_PI/2)), XMFLOAT3(0, 0, 1), currentCellColor)
	);
	//UPPER -> LEFTUPPER
	pMesh->AddTriangle(
		VertexPosNormCol(XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 1), currentCellColor),
		VertexPosNormCol(XMFLOAT3(cos(XM_PI/2), 0, sin(XM_PI/2)), XMFLOAT3(0, 0, 1), currentCellColor),
		VertexPosNormCol(XMFLOAT3(-cos(XM_PI/6), 0, sin(XM_PI/6)), XMFLOAT3(0, 0, 1), currentCellColor)
	);
	//LEFTUPPER -> LEFTUNDER
	pMesh->AddTriangle(
		VertexPosNormCol(XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 1), currentCellColor),
		VertexPosNormCol(XMFLOAT3(-cos(XM_PI/6), 0, sin(XM_PI/6)), XMFLOAT3(0, 0, 1), currentCellColor),
		VertexPosNormCol(XMFLOAT3(-cos(XM_PI/6), 0, -sin(XM_PI/6)), XMFLOAT3(0, 0, 1), currentCellColor)
	);
	//LEFTUPPER -> UNDER
	pMesh->AddTriangle(
		VertexPosNormCol(XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 1), currentCellColor),
		VertexPosNormCol(XMFLOAT3(-cos(XM_PI / 6), 0, -sin(XM_PI / 6)), XMFLOAT3(0, 0, 1), currentCellColor),
		VertexPosNormCol(XMFLOAT3(cos(XM_PI / 2), 0, -sin(XM_PI / 2)), XMFLOAT3(0, 0, 1), currentCellColor)
	);
	//UNDER -> RIGHTUNDER
	pMesh->AddTriangle(
		VertexPosNormCol(XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 1), currentCellColor),
		VertexPosNormCol(XMFLOAT3(cos(XM_PI / 2), 0, -sin(XM_PI / 2)), XMFLOAT3(0, 0, 1), currentCellColor),
		VertexPosNormCol(XMFLOAT3(cos(XM_PI/6), 0, -sin(XM_PI/6)), XMFLOAT3(0, 0, 1), currentCellColor)
	);

	m_pGameObject->AddComponent(pMesh);
}