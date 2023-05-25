#include "stdafx.h"
#include "Explosion.h"

#include "Prefabs/CubePrefab.h"
#include "HexBomberman/HexGrid/HexCell.h"
#include "Materials/DiffuseMaterial.h"

Explosion::Explosion(HexCell* pOwnerTile)
	: m_pOwnerTile(pOwnerTile)
{
}


void Explosion::Initialize(const SceneContext&)
{
	//Materials
	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.f, 0.f, 1.f);

	//this is CHILD OF
	//Cube Object
	AddCubeComp();

	//TEMP GLOBAL POSE
	const PxTransform localPose{ GetTransform()->GetWorldPosition().x
	,GetTransform()->GetWorldPosition().y + 0.5f
	,GetTransform()->GetWorldPosition().z };

	//Explosion Actor
	const auto explosionTrigger = m_pGameObject->AddComponent(new RigidBodyComponent(true));
	explosionTrigger->AddCollider(PxBoxGeometry{ 0.5f, 0.5f, 0.5f }, *pDefaultMaterial, true, localPose);
}

void Explosion::AddCubeComp()
{
	//Create Cube
	const auto pMesh = new MeshDrawComponent(12);

	const float halfWidth = 0.5f;
	const float halfHeight = 0.5f;
	const float halfDepth = 0.5f;
	const XMFLOAT4 color{ Colors::OrangeRed };

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
