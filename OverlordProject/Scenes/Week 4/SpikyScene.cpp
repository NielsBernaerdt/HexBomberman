#include "stdafx.h"
#include "SpikyScene.h"
#include "Materials/SpikyMaterial.h"

void SpikyScene::Initialize()
{
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.settings.enableOnGUI = true;

	//PX Materials
	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.f, 0.f, 0.f);

	//Ground Plane
	GameSceneExt::CreatePhysXGroundPlane(*this, pDefaultMaterial);

	//FX Materials
	m_pMaterial = MaterialManager::Get()->CreateMaterial<SpikyMaterial>();

	//Sphere
	m_pSphere = AddChild(new GameObject());
	const int scale{ 15 };
	m_pSphere->GetTransform()->Scale(scale);

	const auto pSphereModel = m_pSphere->AddComponent(new ModelComponent(L"Meshes/OctaSphere.ovm"));
	pSphereModel->SetMaterial(m_pMaterial);
}

void SpikyScene::Update()
{
	m_pSphere->GetTransform()->Rotate(0, m_RotateSpeed * m_SceneContext.pGameTime->GetTotal(), 0, true);
}

void SpikyScene::OnGUI()
{
	m_pMaterial->OnGUI();
}
