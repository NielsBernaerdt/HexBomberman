#include "stdafx.h"
#include "SoftwareSkinningScene_3.h"
#include "Materials/ColorMaterial.h"
#include "Prefabs/BoneObject.h"

void SoftwareSkinningScene_3::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;

	//FX Materials
	ColorMaterial* pColorMaterial = MaterialManager::Get()->CreateMaterial<ColorMaterial>();

	//Bone Objects
	auto* pRoot = new GameObject{};
	const float boneLength = 15.f;
	m_pBone0 = pRoot->AddChild(new BoneObject{ pColorMaterial, boneLength });
	m_pBone1 = new BoneObject{ pColorMaterial, boneLength };
	m_pBone0->AddBone(m_pBone1);
	AddChild(pRoot);

	//Box Drawer
	m_pBone0->CalculateBindPose();
	GameObject* pBoxDrawer = AddChild(new GameObject{});
	m_pMeshDrawer = pBoxDrawer->AddComponent(new MeshDrawComponent{ 24, true });

	InitializeVertices(boneLength);
}

void SoftwareSkinningScene_3::Update()
{
	if (m_AutoRotate)
	{
		m_CurrentBoneRotation += m_RotationSpeed * m_CurrentRotationSign * m_SceneContext.pGameTime->GetElapsed();

		if (m_CurrentRotationSign > 0 && m_CurrentBoneRotation >= m_MaxAngle)
		{
			m_CurrentRotationSign = -1;
		}
		else if (m_CurrentRotationSign < 0 && m_CurrentBoneRotation <= -m_MaxAngle)
		{
			m_CurrentRotationSign = 1;
		}

		m_pBone0->GetTransform()->Rotate(0, 0, m_CurrentBoneRotation);
		m_pBone1->GetTransform()->Rotate(0, 0, -m_CurrentBoneRotation * 2);
	}
	else
	{
		m_pBone0->GetTransform()->Rotate(m_Bone0Rotation);
		m_pBone1->GetTransform()->Rotate(m_Bone1Rotation);
	}

	//PART 2
	const XMMATRIX boneTransform0 = XMMatrixMultiply(XMLoadFloat4x4(&m_pBone0->GetBindPose()), XMLoadFloat4x4(&m_pBone0->GetTransform()->GetWorld()));
	const XMMATRIX boneTransform1 = XMMatrixMultiply(XMLoadFloat4x4(&m_pBone1->GetBindPose()), XMLoadFloat4x4(&m_pBone1->GetTransform()->GetWorld()));

	for (unsigned int i = 0; i < m_SkinnedVertices.size(); ++i)
	{
		const XMVECTOR originalVertex = XMLoadFloat3(&m_SkinnedVertices[i].originalVertex.Position);
		XMFLOAT3 posBone0;
		XMFLOAT3 posBone1;
		XMStoreFloat3(&posBone0, XMVector3TransformCoord(originalVertex, boneTransform0));
		XMStoreFloat3(&posBone1, XMVector3TransformCoord(originalVertex, boneTransform1));

		m_SkinnedVertices[i].transformedVertex.Position = XMFLOAT3(posBone0.x * m_SkinnedVertices[i].blendWeight0 + posBone1.x * m_SkinnedVertices[i].blendWeight1
			, posBone0.y * m_SkinnedVertices[i].blendWeight0 + posBone1.y * m_SkinnedVertices[i].blendWeight1
			, posBone0.z * m_SkinnedVertices[i].blendWeight0 + posBone1.z * m_SkinnedVertices[i].blendWeight1);
	}

	m_pMeshDrawer->RemoveTriangles();
	for (size_t i{}; i < m_SkinnedVertices.size() / 4; ++i)
	{
		m_pMeshDrawer->AddQuad(
			m_SkinnedVertices[i * 4].transformedVertex
			, m_SkinnedVertices[i * 4 + 1].transformedVertex
			, m_SkinnedVertices[i * 4 + 2].transformedVertex
			, m_SkinnedVertices[i * 4 + 3].transformedVertex);
	}
	m_pMeshDrawer->UpdateBuffer();
}

void SoftwareSkinningScene_3::OnGUI()
{
	//automatic
	ImGui::SliderFloat("MaxAngle", &m_MaxAngle, -0.f, 60.f);
	ImGui::SliderFloat("RotationSpeed", &m_RotationSpeed, 0.f, 90.f);
	ImGui::SliderInt("RotationSign", &m_CurrentRotationSign, -1, 1);
	ImGui::Checkbox("AutoRotate", &m_AutoRotate);
	//Non automatic
	float* bone0[3]{ &m_Bone0Rotation.x,&m_Bone0Rotation.y, &m_Bone0Rotation.z };
	ImGui::InputFloat3("Bone0Rotation", *bone0);
	float* bone1[3]{ &m_Bone1Rotation.x,&m_Bone1Rotation.y, &m_Bone1Rotation.z };
	ImGui::InputFloat3("Bone1Rotation", *bone1);
}

void SoftwareSkinningScene_3::InitializeVertices(float length)
{
	auto pos = XMFLOAT3(length / 2.f, 0.f, 0.f);
	const auto offset = XMFLOAT3(length / 2.f, 2.5f, 2.5f);
	auto col = XMFLOAT4(1.f, 0.f, 0.f, 0.5f);
#pragma region BOX 1
	//FRONT
	XMFLOAT3 norm = { 0, 0, -1 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 1.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 1.f);
	//BACK
	norm = { 0, 0, 1 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 1.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 1.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f);
	//TOP
	norm = { 0, 1, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 1.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 1.f);
	//BOTTOM
	norm = { 0, -1, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 1.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 1.f);
	//LEFT
	norm = { -1, 0, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 1.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 1.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 1.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 1.f);
	//RIGHT
	norm = { 1, 0, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f);
#pragma endregion

	col = { 0.f, 1.f, 0.f, 0.5f };
	pos = { 22.5f, 0.f, 0.f };
#pragma region BOX 2
	//FRONT
	norm = { 0, 0, -1 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.0f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.0f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f);
	//BACK
	norm = { 0, 0, 1 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.0f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.0f);
	//TOP
	norm = { 0, 1, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.0f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.0f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f);
	//BOTTOM
	norm = { 0, -1, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.0f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.0f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f);
	//LEFT
	norm = { -1, 0, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f);
	//RIGHT
	norm = { 1, 0, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.0f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.0f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.0f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.0f);
#pragma endregion
}