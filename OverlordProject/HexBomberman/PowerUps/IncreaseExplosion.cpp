#include "stdafx.h"
#include "IncreaseExplosion.h"

#include "Prefabs/CubePrefab.h"
#include "HexBomberman/Player/PlayerPawn.h"

void IncreaseExplosion::Initialize(const SceneContext&)
{
	m_pGameObject->AddChild(new CubePrefab{ 1.f, 1.f, 1.f, XMFLOAT4(Colors::Yellow) });
}

void IncreaseExplosion::CollectPowerUp(PlayerPawn* pPlayer)
{
	pPlayer->IncreaseBlastRange();
}