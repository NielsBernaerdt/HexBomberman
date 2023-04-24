#include "stdafx.h"
#include "DiffuseMaterial_Shadow_Skinned.h"

DiffuseMaterial_Shadow_Skinned::DiffuseMaterial_Shadow_Skinned():
	Material(L"Effects/Shadow/PosNormTex3D_Shadow_Skinned.fx")
{}

void DiffuseMaterial_Shadow_Skinned::SetDiffuseTexture(const std::wstring& assetFile)
{
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gDiffuseMap", m_pDiffuseTexture);
}

void DiffuseMaterial_Shadow_Skinned::InitializeEffectVariables()
{
}

void DiffuseMaterial_Shadow_Skinned::OnUpdateModelVariables(const SceneContext& sceneContext, const ModelComponent* pModel) const
{
	// 1. Update the LightWVP > Used to Transform a vertex into Light clipping space
	const auto world = pModel->GetTransform()->GetWorld();
	const auto worldMat = XMLoadFloat4x4(&world);

	const auto pShadowMapRenderer = ShadowMapRenderer::Get();
	const auto lightVP = pShadowMapRenderer->GetLightVP();
	const auto lightVPMat = XMLoadFloat4x4(&lightVP);

	const auto lightWVPMat = worldMat * lightVPMat;
	XMFLOAT4X4 lightWVP;
	XMStoreFloat4x4(&lightWVP, lightWVPMat);

	SetVariable_Matrix(L"gWorldViewProj_Light", lightWVP);

	// 2. Update the ShadowMap texture
	SetVariable_Texture(L"gShadowMap", pShadowMapRenderer->GetShadowMap());

	// 3. Update the Light Direction (retrieve the direction from the LightManager > sceneContext)
	SetVariable_Vector(L"gLightDirection", sceneContext.pLights->GetDirectionalLight().direction);

	// 4. Update Bones
	auto modelAnimator = pModel->GetAnimator();
	ASSERT_NULL_(modelAnimator);
	auto boneTransforms = modelAnimator->GetBoneTransforms();
	SetVariable_MatrixArray(L"gBones", &boneTransforms[0]._11, (UINT)boneTransforms.size());
}
