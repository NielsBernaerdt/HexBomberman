#include "stdafx.h"
#include "HexCell.h"

#include "HexUtils.h"
#include "Prefabs/CubePrefab.h"
#include "Prefabs/SpherePrefab.h"

void HexCell::Initialize(const SceneContext&)
{
	//Materials
	const auto pMaterial = PxGetPhysics().createMaterial(0.f, 0.f, 1.f);

	//Ground Tile
	m_pGroundTile = m_pGameObject->AddChild(new SpherePrefab{1, 6, XMFLOAT4(Colors::Green)});
	m_pGroundTile->GetTransform()->Scale(1.f, 0.01f, 1.f);
	m_pGroundTile->GetTransform()->Rotate(0.f, 30.f, 0.f, true);

	//Actor
	const auto pRigidBody = m_pGameObject->AddComponent(new RigidBodyComponent(true));
	pRigidBody->AddCollider(PxSphereGeometry{ 1.f }, *pMaterial);
	pRigidBody->Translate(m_pGroundTile->GetTransform()->GetPosition());
}

void HexCell::Update(const SceneContext& sceneContext)
{
	if(m_IsExploding == true)
	{
		m_AccTime += sceneContext.pGameTime->GetElapsed();
		if(m_AccTime >= m_ExplosionDuration)
		{
			m_AccTime = 0.f;
			m_pGameObject->RemoveChild(m_pCrate, true);
			m_IsExploding = false;
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

void HexCell::SpawnCrate()
{
	if (m_IsExploding == true)
		return;

	//Crate
	m_pCrate = m_pGameObject->AddChild(new CubePrefab{ 1.f, 1.f, 1.f, XMFLOAT4(Colors::SlateGray) });
	m_pCrate->GetTransform()->Translate(0.f, 0.5f, 0.f);

	m_IsExploding = true;
}

std::vector<HexCell*> HexCell::GetBlastTiles(int length) const
{
	std::vector<HexCell*> blastTiles{};

	for(int currentHexDirection{}; currentHexDirection < m_pNeighbors.size(); ++currentHexDirection)
	{
		blastTiles.push_back(m_pNeighbors[currentHexDirection]);

		HexCell* currentCell = m_pNeighbors[currentHexDirection];
		for (int i{}; i < length - 1; ++i)
		{
			currentCell = currentCell->GetNeighbor(static_cast<HexDirection>(currentHexDirection));
			blastTiles.push_back(currentCell);
		}
	}

	return blastTiles;
}