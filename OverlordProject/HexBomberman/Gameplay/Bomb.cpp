#include "stdafx.h"
#include "Bomb.h"

#include "Explosion.h"
#include "HexBomberman/HexGrid/HexCell.h"
#include "HexBomberman/Player/PlayerPawn.h"
#include "Prefabs/SpherePrefab.h"

Bomb::Bomb(HexCell* ownerCell, PlayerPawn* pPlayer, int blastRange)
	: m_pOwnerCell(ownerCell)
	, m_pPlayer(pPlayer)
	, m_BlastRange(blastRange)
{
}


void Bomb::Initialize(const SceneContext& /*sceneContext*/)
{
	//Materials
	//const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.f, 0.f, 1.f);
	//Geometry
	m_pGameObject->AddChild(new SpherePrefab{0.5f});
	////Actor
	//const auto bombTrigger = m_pGameObject->AddComponent(new RigidBodyComponent(false));
	//bombTrigger->AddCollider(PxBoxGeometry{ 0.5f, 0.5f, 0.5f }, *pDefaultMaterial);

	if(m_pOwnerCell)
		Explode(m_BlastRange);
}

void Bomb::Update(const SceneContext& sceneContext)
{
	m_AccTime += sceneContext.pGameTime->GetElapsed();
	if (m_AccTime >= m_ExplosionDuration)
	{
		if(m_pOwnerCell)
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
		, destinationPos.y - currentPos.y
		, destinationPos.z - currentPos.z
	);

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
			, destinationPos.y - currentPos.y
			, destinationPos.z - currentPos.z
		);
	}
}

void Bomb::EndExplosion() const
{
	m_pPlayer->BombExploded();

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