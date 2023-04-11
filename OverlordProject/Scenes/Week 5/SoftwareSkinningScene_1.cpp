#include "stdafx.h"
#include "SoftwareSkinningScene_1.h"

#include "Materials/ColorMaterial.h"
#include "Prefabs/BoneObject.h"

void SoftwareSkinningScene_1::Initialize()
{
	ColorMaterial* pColorMaterial = MaterialManager::Get()->CreateMaterial<ColorMaterial>();
	pColorMaterial->SetColor(XMFLOAT4(1, 1, 1, 1));

	GameObject* pRoot = new GameObject{};

	m_pBone0 = pRoot->AddChild(new BoneObject{ pColorMaterial, 15.f });
	m_pBone1 = new BoneObject{ pColorMaterial, 15.f };
	m_pBone0->AddBone(m_pBone1);

	AddChild(pRoot);
}

void SoftwareSkinningScene_1::Update()
{
	const float rotationspeed = m_MaxAngle;
	m_BoneRotation += m_RotationSign * rotationspeed * m_SceneContext.pGameTime->GetElapsed();

	if (m_RotationSign < 0 && m_BoneRotation <= -m_MaxAngle)
		m_RotationSign = 1;
	else if (m_RotationSign > 0 && m_BoneRotation >= m_MaxAngle)
		m_RotationSign = -1;

	m_pBone0->GetTransform()->Rotate(0, 0, m_BoneRotation);
	m_pBone1->GetTransform()->Rotate(0, 0, -m_BoneRotation * 2);
}