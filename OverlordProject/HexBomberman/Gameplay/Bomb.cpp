#include "stdafx.h"
#include "Bomb.h"

#include "Explosion.h"
#include "HexBomberman/HexGrid/HexCell.h"
#include "HexBomberman/Player/PlayerPawn.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow.h"
#include "Prefabs/SpherePrefab.h"

Bomb::Bomb(HexCell* ownerCell, PlayerPawn* pPlayer, int blastRange)
	: m_pOwnerCell(ownerCell)
	, m_pPlayer(pPlayer)
	, m_BlastRange(blastRange)
{
}


void Bomb::Initialize(const SceneContext& /*sceneContext*/)
{
	const auto pBombMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pBombMaterial->SetDiffuseTexture(L"Textures/Bomb_Diffuse.png");

	const auto pObject = m_pGameObject->AddChild(new GameObject);
	const auto pModel = pObject->AddComponent(new ModelComponent(L"Meshes/Bomb.ovm"));
	pModel->SetMaterial(pBombMaterial);
	pObject->GetTransform()->Rotate(123.f, 90, 90.f);
}

void Bomb::Update(const SceneContext& sceneContext)
{
	m_AccTime += sceneContext.pGameTime->GetElapsed();
	if (m_HasExploded == false)
	{
		if (m_AccTime >= m_ExplosionDuration)
		{
			m_HasExploded = true;
			m_AccTime = 0.f;
			if (m_pOwnerCell)
				Explode(m_BlastRange);
		}
	}
	else
	{
		if (m_AccTime >= 0.25f)
		{
			if (m_pOwnerCell)
				EndExplosion();
		}
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