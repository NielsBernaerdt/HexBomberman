#include "stdafx.h"
#include "Bomb.h"

#include "Explosion.h"
#include "HexBomberman/HexGrid/HexCell.h"
#include "Prefabs/SpherePrefab.h"

Bomb::Bomb(HexCell* ownerCell)
	: m_pOwnerCell(ownerCell)
{
}


void Bomb::Initialize(const SceneContext& /*sceneContext*/)
{
	m_pGameObject->AddChild(new SpherePrefab{0.5f});

	Explode(3);
}

void Bomb::Update(const SceneContext& sceneContext)
{
	m_AccTime += sceneContext.pGameTime->GetElapsed();
	if (m_AccTime >= m_ExplosionDuration)
	{
		EndExplosion();
	}
}


void Bomb::Explode(int blastRange)
{
	//Spawn Initial Explosion
	auto explosionObject = m_pGameObject->AddChild(new GameObject{});
	m_pExplosionObjects.push_back(explosionObject->AddComponent(new Explosion{m_pOwnerCell}));

	auto currentPos{ m_pGameObject->GetTransform()->GetWorldPosition() };
	auto destinationPos{ m_pOwnerCell->GetTransform()->GetPosition() };
	explosionObject->GetTransform()->Translate(
		destinationPos.x - currentPos.x
		, destinationPos.y - currentPos.y + 0.5f
		, destinationPos.z - currentPos.z
	);
	explosionObject->GetTransform()->Scale(.1f, .1f, .1f);

	//Spawn Explosions on Neighbors
	for (const auto neighbor : m_pOwnerCell->GetTilesToExplode(blastRange))
	{
		if (neighbor == nullptr)
			continue;
		
		explosionObject = m_pGameObject->AddChild(new GameObject{});
		m_pExplosionObjects.push_back(explosionObject->AddComponent(new Explosion{ neighbor }));

		currentPos = m_pGameObject->GetTransform()->GetWorldPosition();
		destinationPos = neighbor->GetTransform()->GetPosition();
		explosionObject->GetTransform()->Translate(
			destinationPos.x - currentPos.x
			, destinationPos.y - currentPos.y + 0.5f
			, destinationPos.z - currentPos.z
		);
		explosionObject->GetTransform()->Scale(1.1f, 1.1f, 1.1f);
	}
}

void Bomb::EndExplosion() const
{
	//todo: Do this in Crate.cpp using collision event?
	for(auto explosionObject : m_pExplosionObjects)
	{
		const auto pHexCell = explosionObject->GetOwnerTile();
		if (pHexCell->HasCrate() == true)
		{
			pHexCell->DestroyCrate();
		}
	}
	//Remove Bomb & all Explosion Objects
	m_pGameObject->GetParent()->RemoveChild(m_pGameObject, true);
}