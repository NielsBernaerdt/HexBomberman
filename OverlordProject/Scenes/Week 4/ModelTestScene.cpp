#include "stdafx.h"
#include "ModelTestScene.h"

#include "Materials/ColorMaterial.h"
#include "Materials/DiffuseMaterial.h"


void ModelTestScene::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;

	//Materials
	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.f, 0.f, 0.f);

	//Ground Plane
	GameSceneExt::CreatePhysXGroundPlane(*this, pDefaultMaterial);

	//Color Materials
	//ColorMaterial* pMaterial = MaterialManager::Get()->CreateMaterial<ColorMaterial>();
	//pMaterial->SetColor(XMFLOAT4{ Colors::Red });

	//Diffuse Material
	DiffuseMaterial* pMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
	pMaterial->SetDiffuseTexture(L"Textures/Chair_Dark.dds");

	//Chair
	m_pChair = AddChild(new GameObject());
	const auto pChairModel = m_pChair->AddComponent(new ModelComponent(L"Meshes/Chair.ovm"));
	pChairModel->SetMaterial(pMaterial);
	m_pChair->GetTransform()->Translate(0, 3, 0);
	m_pChair->GetTransform()->Rotate(XMFLOAT3{ 30.f, 0.f, 0.f }, true);

	//Chair Actor
	const auto chairConvexMesh = ContentManager::Load<PxConvexMesh>(L"Meshes/Chair.ovpc");
	const auto pChairActor = m_pChair->AddComponent(new RigidBodyComponent());
	pChairActor->AddCollider(PxConvexMeshGeometry{ chairConvexMesh }, *pDefaultMaterial);
}

void ModelTestScene::Update()
{
	if (m_SceneContext.pInput->IsKeyboardKey(InputState::down, 'R'))
	{
		Reset();
	}
}

void ModelTestScene::Reset()
{
	m_pChair->GetTransform()->Translate(0, 3, 0);
	m_pChair->GetTransform()->Rotate(XMFLOAT3{ 30.f, 0.f, 0.f }, true);
}

void ModelTestScene::OnGUI()
{
	ImGui::Text("RESET scene: \t 'R'");
}