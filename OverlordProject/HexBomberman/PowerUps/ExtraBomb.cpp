#include "stdafx.h"
#include "ExtraBomb.h"

#include "HexBomberman/Player/PlayerPawn.h"
#include "Materials/BasicMaterial_Deferred.h"

void ExtraBomb::Initialize(const SceneContext&)
{
	const auto pItemMaterial = MaterialManager::Get()->CreateMaterial<BasicMaterial_Deferred>();
	pItemMaterial->SetDiffuseMap(L"Textures/BombItem_Diffuse.png");
	pItemMaterial->SetNormalMap(L"Textures/Item_Normal.png");
	pItemMaterial->SetSpecularMap(L"Textures/Item_Specular.png");

	const auto pObject = m_pGameObject->AddChild(new GameObject);
	const auto pModel = pObject->AddComponent(new ModelComponent(L"Meshes/Item.ovm"));
	pModel->SetMaterial(pItemMaterial);

	pObject->GetTransform()->Rotate(90.f, 0.f, 0.f);
	pObject->GetTransform()->Translate(0.f, 0.5f, -0.5f);
}

void ExtraBomb::CollectPowerUp(PlayerPawn* pPlayer)
{
	pPlayer->IncreaseNrBombs();
}
