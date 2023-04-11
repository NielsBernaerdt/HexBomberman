#include "stdafx.h"
#include "BoneObject.h"

BoneObject::BoneObject(BaseMaterial* pMaterial, float length)
	: m_Length{ length }
	, m_pMaterial{ pMaterial }
{
}

void BoneObject::Initialize(const SceneContext&)
{
	GameObject* pEmpty = new GameObject{};
	AddChild(pEmpty);

	ModelComponent* pModel = pEmpty->AddComponent(new ModelComponent{ L"Meshes/Bone.ovm" });
	pModel->SetMaterial(m_pMaterial);

	pEmpty->GetTransform()->Rotate(0, -90, 0);
	pEmpty->GetTransform()->Scale(m_Length);


	//// Add a modelcomponent to pEmpty(pModel), use Bone.ovm
	//auto pModel = new ModelComponent(L"Meshes/Bone.ovm");

	//// Assign the BoneObject's material (m_pMaterial) to pModel
	//pModel->SetMaterial(m_pMaterial);
	//pEmpty->AddComponent(pModel);
}

void BoneObject::AddBone(BoneObject* pBone)
{
	pBone->GetTransform()->Translate(XMFLOAT3{m_Length, 0, 0});
	AddChild(pBone);
}

void BoneObject::CalculateBindPose()
{
	const XMMATRIX worldMatrix = XMLoadFloat4x4(&GetTransform()->GetWorld());
	const XMMATRIX inversedWorld = XMMatrixInverse(nullptr, worldMatrix);
	XMStoreFloat4x4(&m_BindPose, inversedWorld);

	const auto children = GetChildren<BoneObject>();
	for (const auto child : children)
	{
		child->CalculateBindPose();
	}
}