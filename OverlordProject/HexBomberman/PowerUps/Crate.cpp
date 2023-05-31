#include "stdafx.h"
#include "Crate.h"

#include "ExtraBomb.h"
#include "IncreaseExplosion.h"
#include "Prefabs/CubePrefab.h"
#include "HexBomberman/HexGrid/HexCell.h"
#include "Materials/BasicMaterial_Deferred.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow.h"

void Crate::Initialize(const SceneContext&)
{
	//Materials
	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.f, 0.f, 1.f);

	//Cube Object
	const auto pCrateMaterial = MaterialManager::Get()->CreateMaterial<BasicMaterial_Deferred>();
	pCrateMaterial->SetDiffuseMap(L"Textures/Crate_Diffuse.png");
	pCrateMaterial->SetNormalMap(L"Textures/Crate_Normal.png");
	//const auto pCrateMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	//pCrateMaterial->SetDiffuseTexture(L"Textures/Crate_Diffuse.png");
	const auto pObject = m_pGameObject->AddChild(new GameObject);
	const auto pModel = pObject->AddComponent(new ModelComponent(L"Meshes/Crate.ovm"));
	pModel->SetMaterial(pCrateMaterial);

	//SCALE
	//pObject->GetTransform()->Scale(0.5f, 0.5f, 0.5f);

	//RANDOM ROTATION
	const float randomRotationValue{ static_cast<float>(std::rand() % 360) };
	pObject->GetTransform()->Rotate(0.f, randomRotationValue, 0.f);

	//TEMP GLOBAL POSE
	const PxTransform localPose{ GetTransform()->GetWorldPosition().x
	,GetTransform()->GetWorldPosition().y
	,GetTransform()->GetWorldPosition().z };

	//Explosion Actor
	const auto explosionTrigger = m_pGameObject->AddComponent(new RigidBodyComponent(true));
	explosionTrigger->AddCollider(PxBoxGeometry{ 0.5f, 2.f, 0.5f }, *pDefaultMaterial, false, localPose);
}

void Crate::SpawnPowerUp(HexCell* pHexCellParent) const
{
	const float randomPercentage{ rand() % 101 / 100.f };
	if(randomPercentage <= m_SpawnChance)
	{
        GameObject* pPowerUp{ nullptr };
        switch (const int randomPowerUp{ rand() % 2 })
        {
        case 0:
            pPowerUp = pHexCellParent->GetGameObject()->AddChild(new GameObject{});
			pHexCellParent->AddPowerUp(pPowerUp->AddComponent(new ExtraBomb{}));
            pPowerUp->GetTransform()->Translate(0.f, 0.5f, 0.f);
            break;
        case 1:
        	pPowerUp = pHexCellParent->GetGameObject()->AddChild(new GameObject{});
			pHexCellParent->AddPowerUp(pPowerUp->AddComponent(new IncreaseExplosion{}));
            pPowerUp->GetTransform()->Translate(0.f, 0.5f, 0.f);
            break;
        default:
            Logger::LogError(L"Crate::SpawnPowerUp - Invalid random number.\n");
            break;
        }
	}
}