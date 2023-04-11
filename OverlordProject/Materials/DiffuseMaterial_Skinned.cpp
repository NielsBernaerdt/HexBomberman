#include "stdafx.h"
#include "DiffuseMaterial_Skinned.h"

void DiffuseMaterial_Skinned::SetDiffuseTexture(const std::wstring& assetFile)
{
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gDiffuseMap", m_pDiffuseTexture);
}

void DiffuseMaterial_Skinned::InitializeEffectVariables()
{
}

void DiffuseMaterial_Skinned::OnUpdateModelVariables(const SceneContext&, const ModelComponent* pModel) const
{
	const auto modelAnimator = pModel->GetAnimator();
	ASSERT_NULL_(modelAnimator);
	const auto boneTransforms = modelAnimator->GetBoneTransforms();
	SetVariable_MatrixArray(L"gBones", &boneTransforms[0]._11, (UINT)boneTransforms.size());
}