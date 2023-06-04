#include "stdafx.h"
#include "Bomb.h"

#include "Explosion.h"
#include "HexBomberman/HexGrid/HexCell.h"
#include "HexBomberman/Player/PlayerPawn.h"
#include "Materials/BasicMaterial_Deferred.h"
#include "Materials/DiffuseMaterial.h"
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
	const auto pBombMaterial = MaterialManager::Get()->CreateMaterial<BasicMaterial_Deferred>();
	pBombMaterial->SetDiffuseMap(L"Textures/Bomb_Diffuse.png");
	pBombMaterial->SetSpecularMap(L"Textures/Bomb_Specular.png");

	const auto pObject = m_pGameObject->AddChild(new GameObject);
	const auto pModel = pObject->AddComponent(new ModelComponent(L"Meshes/Bomb.ovm"));
	pModel->SetMaterial(pBombMaterial);

	pObject->GetTransform()->Rotate(90.f, 0.f, 0.f);
	pObject->GetTransform()->Translate(0.f, -0.5f, 0.f);


	//Particle System
	ParticleEmitterSettings settings{};
	settings.velocity = { 0.f,6.f,0.f };
	settings.minSize = 0.2f;
	settings.maxSize = 0.6f;
	settings.minEnergy = 0.f;
	settings.maxEnergy = 0.f;
	settings.minScale = 3.5f;
	settings.maxScale = 5.5f;
	settings.minEmitterRadius = .0f;
	settings.maxEmitterRadius = .1f;
	settings.color = { 1.f,1.f,1.f, 1.f };

	pParticle = m_pGameObject->AddChild(new GameObject);
	pParticle->AddComponent(new ParticleEmitterComponent(L"Textures/Fire.png", settings, 200));

	//Sound 2D
	const auto pFmod = SoundManager::Get()->GetSystem();
	FMOD::Sound* pSound{};
	FMOD_RESULT result = pFmod->createStream("Resources/Sounds/FuseLoop.wav", FMOD_DEFAULT | FMOD_LOOP_NORMAL, nullptr, &pSound);
	SoundManager::Get()->ErrorCheck(result); //Be sure to errocheck the result

	result = pFmod->playSound(pSound, nullptr, false, &m_pChannel2D);
	SoundManager::Get()->ErrorCheck(result); //again: check result!
}

void Bomb::Update(const SceneContext& sceneContext)
{
	if(m_HasSpawnedDanger == false)
	{
		m_HasSpawnedDanger = true;
		SpawnDangerSigns();
	}
	//////////
	auto pos = GetTransform()->GetWorldPosition();
	pos.y += 0.7f;
	pParticle->GetTransform()->Translate(pos);
	/// <summary>
	/// ///////
	/// </summary>
	/// <param name="sceneContext"></param>
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
		//if (m_AccTime >= 0.25f)
		if (m_AccTime >= 0.01f)
		{
			if (m_pOwnerCell)
				EndExplosion();
		}
	}
}

void Bomb::SpawnDangerSigns()
{
	//Spawn Explosions on Neighbors
	for (const auto neighbor : m_pOwnerCell->GetTilesToExplode(m_BlastRange))
	{
		if (neighbor == nullptr)
			continue;

		const auto pMaterial = MaterialManager::Get()->CreateMaterial<BasicMaterial_Deferred>();
		pMaterial->SetDiffuseMap(L"Textures/Danger.png");

		const auto pGroundObj = m_pGameObject->AddChild(new GameObject);
		const auto pGroundModel = pGroundObj->AddComponent(new ModelComponent(L"Meshes/Plane.ovm"));
		pGroundModel->SetMaterial(pMaterial);
		pGroundObj->GetTransform()->Scale(1.2f, 1.2f, 1.2f);
		pGroundObj->GetTransform()->Rotate(90.f, 0.f, 0.f);
		m_pDangerTiles.push_back(pGroundObj);

		const auto currentPos = m_pGameObject->GetTransform()->GetWorldPosition();
		const auto destinationPos = neighbor->GetTransform()->GetPosition();
		pGroundObj->GetTransform()->Translate(
			destinationPos.x - currentPos.x
			, destinationPos.y - currentPos.y + 0.1f
			, destinationPos.z - currentPos.z
		);
	}
}

void Bomb::Explode(int blastRange)
{
	for(auto& pDangerTile : m_pDangerTiles)
	{
		m_pGameObject->RemoveChild(pDangerTile, true);
	}
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

void Bomb::EndExplosion()
{
	//
	m_pChannel2D->setPaused(true);
	//Sound 2D
	const auto pFmod = SoundManager::Get()->GetSystem();
	FMOD::Sound* pSound{};
	FMOD_RESULT result = pFmod->createStream("Resources/Sounds/Explosion.wav", FMOD_DEFAULT, nullptr, &pSound);
	SoundManager::Get()->ErrorCheck(result); //Be sure to errocheck the result

	result = pFmod->playSound(pSound, nullptr, false, &m_pChannel2D);
	m_pChannel2D->setVolume(0.075f);
	SoundManager::Get()->ErrorCheck(result); //again: check result!

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