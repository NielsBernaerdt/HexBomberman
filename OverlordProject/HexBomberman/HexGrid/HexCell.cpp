#include "stdafx.h"
#include "HexCell.h"

#include "HexUtils.h"
#include "HexBomberman/Gameplay/Explosion.h"
#include "HexBomberman/PowerUps/Crate.h"
#include "Prefabs/SpherePrefab.h"

void HexCell::Initialize(const SceneContext&)
{
	//Materials
	const auto pMaterial = PxGetPhysics().createMaterial(0.f, 0.f, 1.f);

	//Ground Tile
	m_pGroundTile = m_pGameObject->AddChild(new SpherePrefab{ 1, 6, XMFLOAT4(Colors::Green) });
	m_pGroundTile->GetTransform()->Scale(1.f, 0.01f, 1.f);
	m_pGroundTile->GetTransform()->Rotate(0.f, 30.f, 0.f, true);

	//Actor
	const auto pRigidBody = m_pGameObject->AddComponent(new RigidBodyComponent(true));
	pRigidBody->AddCollider(PxSphereGeometry{ 1.f }, *pMaterial);
	pRigidBody->Translate(m_pGroundTile->GetTransform()->GetPosition());

	//Crate
	m_HasCrate = true;
	const auto pCrateObject = m_pGameObject->AddChild(new GameObject{});
	m_pCrateComponent = pCrateObject->AddComponent(new Crate{});
	m_pCrateComponent->GetTransform()->Translate(0.f, 0.5f, 0.f);
}

void HexCell::Update(const SceneContext& sceneContext)
{
	if(m_IsExploding == true)
	{
		m_AccTime += sceneContext.pGameTime->GetElapsed();
		if(m_AccTime >= m_ExplosionDuration)
		{
			EndExplosion();
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

void HexCell::StartExplosion()
{
	if (m_IsExploding == true)
		return;

	//Explosion Object
	m_pExplosionObject = m_pGameObject->AddChild(new GameObject{});
	m_pExplosionObject->AddComponent(new Explosion{});
	m_pExplosionObject->GetTransform()->Translate(0.f, 0.5f, 0.f);
	m_pExplosionObject->GetTransform()->Scale(1.1f, 1.1f, 1.1f);

	m_IsExploding = true;
}

void HexCell::EndExplosion()
{
	m_AccTime = 0.f;
	m_pGameObject->RemoveChild(m_pExplosionObject, true);
	m_IsExploding = false;

	//todo: Do this in Crate.cpp using collision event?
	if(m_HasCrate == true)
	{
		m_HasCrate = false;
		m_pCrateComponent->SpawnPowerUp(this);
		m_pGameObject->RemoveChild(m_pCrateComponent->GetGameObject(), true);
	}
}


std::vector<HexCell*> HexCell::GetTilesToExplode(int length) const
{
	std::vector<HexCell*> blastTiles{};

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
			blastTiles.push_back(currentCell);

			if (currentCell != nullptr
				&& currentCell->HasCrate() == true)
			{
				break;
			}
		}
	}

	return blastTiles;
}