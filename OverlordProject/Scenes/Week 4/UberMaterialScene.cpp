#include "stdafx.h"
#include "UberMaterialScene.h"

void UberMaterialScene::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;

	//PX Materials
	auto pDefaultMaterial = PxGetPhysics().createMaterial(0.f, 0.f, 0.f);

	//Ground Plane
	GameSceneExt::CreatePhysXGroundPlane(*this, pDefaultMaterial);

	//FX Materials
	m_pMaterial = MaterialManager::Get()->CreateMaterial<UberMaterial>();

	//Sphere
	m_pSphere = AddChild(new GameObject());
	m_pSphere->GetTransform()->Scale(20, 20, 20);
	auto pSphereModel = m_pSphere->AddComponent(new ModelComponent(L"Meshes/Sphere.ovm"));
	pSphereModel->SetMaterial(m_pMaterial);
}

void UberMaterialScene::Update()
{
	m_pSphere->GetTransform()->Rotate(0, m_RotateSpeed * m_SceneContext.pGameTime->GetTotal(), 0, true);
}

void UberMaterialScene::OnGUI()
{
	m_pMaterial->OnGUI();
}