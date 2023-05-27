#include "stdafx.h"
#include "IncreaseExplosion.h"

#include "HexBomberman/Player/PlayerPawn.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow.h"

void IncreaseExplosion::Initialize(const SceneContext&)
{
	const auto BombItemMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	BombItemMaterial->SetDiffuseTexture(L"Textures/BlastRangeItem_Diffuse.png");

	const auto pObject = m_pGameObject->AddChild(new GameObject);
	const auto pModel = pObject->AddComponent(new ModelComponent(L"Meshes/Item.ovm"));
	pModel->SetMaterial(BombItemMaterial);

	pObject->GetTransform()->Rotate(90.f, 0.f, 0.f);
	pObject->GetTransform()->Translate(0.f, 0.5f, -0.5f);
}

void IncreaseExplosion::CollectPowerUp(PlayerPawn* pPlayer)
{
	pPlayer->IncreaseBlastRange();
}