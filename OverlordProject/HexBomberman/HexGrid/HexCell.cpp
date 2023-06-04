#include "stdafx.h"
#include "HexCell.h"

#include "HexUtils.h"
#include "HexBomberman/Gameplay/Bomb.h"
#include "HexBomberman/Player/PlayerPawn.h"
#include "HexBomberman/PowerUps/Crate.h"
#include "HexBomberman/PowerUps/BasePowerUp.h"

void HexCell::Initialize(const SceneContext&)
{
	//Materials
	const auto pMaterial = PxGetPhysics().createMaterial(0.f, 0.f, 1.f);

	//Actor
	const auto pRigidBody = m_pGameObject->AddComponent(new RigidBodyComponent(true));
	pRigidBody->AddCollider(PxSphereGeometry{ 0.85f }, *pMaterial, true);
}

void HexCell::Update(const SceneContext&)
{
	//todo: place in PostInitialize
	if(m_UpdateOnce == true)
	{
		m_UpdateOnce = false;

		if (std::rand() % 5 != 0) //4 / 5 chance to spawn crate
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

std::vector<HexCell*> HexCell::GetTilesToExplode(int length) const
{
	const std::vector<HexCell*>::size_type maxNrTilesToExplode = 6 * length;
	std::vector<HexCell*> blastTiles{ maxNrTilesToExplode };

	for (int currentHexDirection{}; currentHexDirection < m_pNeighbors.size(); ++currentHexDirection)
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

void HexCell::PlaceBomb(PlayerPawn* pPlayer, int blastRange)
{
	const auto bombObject = m_pGameObject->AddChild(new GameObject{});
	bombObject->AddComponent(new Bomb{this, pPlayer, blastRange});
	bombObject->GetTransform()->Translate(0.f, 0.5f, 0.f);
}

void HexCell::DestroyCrate(bool spawnCrate)
{
	m_HasCrate = false;
	if(spawnCrate) m_pCrateComponent->SpawnPowerUp(this);
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