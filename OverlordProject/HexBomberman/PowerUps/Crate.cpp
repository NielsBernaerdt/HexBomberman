#include "stdafx.h"
#include "Crate.h"

#include "ExtraBomb.h"
#include "IncreaseExplosion.h"
#include "Prefabs/CubePrefab.h"
#include "HexBomberman/HexGrid/HexCell.h"

void Crate::Initialize(const SceneContext&)
{
	//Materials
	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.f, 0.f, 1.f);

	//Cube Object
	AddCubeComp();

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

void Crate::AddCubeComp()
{
	//Create Cube
	const auto pMesh = new MeshDrawComponent(12);

	const float halfWidth = 0.45f;
	const float halfHeight = 0.45f;
	const float halfDepth = 0.45f;
	const XMFLOAT4 color{ Colors::Gray };

	//FRONT
	pMesh->AddQuad(
		VertexPosNormCol(XMFLOAT3(-halfWidth, halfHeight, -halfDepth), XMFLOAT3(0, 0, -1), color),
		VertexPosNormCol(XMFLOAT3(halfWidth, halfHeight, -halfDepth), XMFLOAT3(0, 0, -1), color),
		VertexPosNormCol(XMFLOAT3(halfWidth, -halfHeight, -halfDepth), XMFLOAT3(0, 0, -1), color),
		VertexPosNormCol(XMFLOAT3(-halfWidth, -halfHeight, -halfDepth), XMFLOAT3(0, 0, -1), color)
	);

	//BACK
	pMesh->AddQuad(
		VertexPosNormCol(XMFLOAT3(halfWidth, halfHeight, halfDepth), XMFLOAT3(0, 0, 1), color),
		VertexPosNormCol(XMFLOAT3(-halfWidth, halfHeight, halfDepth), XMFLOAT3(0, 0, 1), color),
		VertexPosNormCol(XMFLOAT3(-halfWidth, -halfHeight, halfDepth), XMFLOAT3(0, 0, 1), color),
		VertexPosNormCol(XMFLOAT3(halfWidth, -halfHeight, halfDepth), XMFLOAT3(0, 0, 1), color)
	);

	//LEFT
	pMesh->AddQuad(
		VertexPosNormCol(XMFLOAT3(-halfWidth, halfHeight, halfDepth), XMFLOAT3(-1, 0, 0), color),
		VertexPosNormCol(XMFLOAT3(-halfWidth, halfHeight, -halfDepth), XMFLOAT3(-1, 0, 0), color),
		VertexPosNormCol(XMFLOAT3(-halfWidth, -halfHeight, -halfDepth), XMFLOAT3(-1, 0, 0), color),
		VertexPosNormCol(XMFLOAT3(-halfWidth, -halfHeight, halfDepth), XMFLOAT3(-1, 0, 0), color)
	);

	//RIGHT
	pMesh->AddQuad(
		VertexPosNormCol(XMFLOAT3(halfWidth, halfHeight, -halfDepth), XMFLOAT3(1, 0, 0), color),
		VertexPosNormCol(XMFLOAT3(halfWidth, halfHeight, halfDepth), XMFLOAT3(1, 0, 0), color),
		VertexPosNormCol(XMFLOAT3(halfWidth, -halfHeight, halfDepth), XMFLOAT3(1, 0, 0), color),
		VertexPosNormCol(XMFLOAT3(halfWidth, -halfHeight, -halfDepth), XMFLOAT3(1, 0, 0), color)
	);

	//TOP
	pMesh->AddQuad(
		VertexPosNormCol(XMFLOAT3(-halfWidth, halfHeight, halfDepth), XMFLOAT3(0, 1, 0), color),
		VertexPosNormCol(XMFLOAT3(halfWidth, halfHeight, halfDepth), XMFLOAT3(0, 1, 0), color),
		VertexPosNormCol(XMFLOAT3(halfWidth, halfHeight, -halfDepth), XMFLOAT3(0, 1, 0), color),
		VertexPosNormCol(XMFLOAT3(-halfWidth, halfHeight, -halfDepth), XMFLOAT3(0, 1, 0), color)
	);

	//BOTTOM
	pMesh->AddQuad(
		VertexPosNormCol(XMFLOAT3(-halfWidth, -halfHeight, -halfDepth), XMFLOAT3(0, -1, 0), color),
		VertexPosNormCol(XMFLOAT3(halfWidth, -halfHeight, -halfDepth), XMFLOAT3(0, -1, 0), color),
		VertexPosNormCol(XMFLOAT3(halfWidth, -halfHeight, halfDepth), XMFLOAT3(0, -1, 0), color),
		VertexPosNormCol(XMFLOAT3(-halfWidth, -halfHeight, halfDepth), XMFLOAT3(0, -1, 0), color)
	);

	m_pGameObject->AddComponent(pMesh);
}