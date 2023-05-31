#include "stdafx.h"
#include "UberMaterial.h"

UberMaterial::~UberMaterial()
{
	//Diffuse
	delete m_pUseTextureDiffuse;
	delete m_pColorDiffuse;

	//Specular
	delete m_pColorSpecular;
	delete m_pUseTextureSpecular;
	delete m_pShininess;

	delete m_pUseBlinn;
	delete m_pUsePhong;

	//Ambient
	delete m_pColorAmbient;
	delete m_pAmbientIntensity;

	//Normal Mapping
	delete m_pFlipGreenChannel;
	delete m_pUseTextureNormal;

	//Environment Mapping
	delete m_pUseEnvironmentMapping;
	delete m_pReflectionStrength;
	delete m_pRefractionStrength;
	delete m_pRefractionIndex;

	//Fresnel Falloff
	delete m_pUseFresnelFalloff;
	delete m_pColorFresnel;
	delete m_pFresnelPower;
	delete m_pFresnelMultiplier;
	delete m_pFresnelHardness;

	//Opacity
	delete m_pOpacityIntensity;
	delete m_pUseTextureOpacity;
}

void UberMaterial::InitializeEffectVariables()
{
	//Diffuse
	SetVariable_Scalar(L"gUseTextureDiffuse", *m_pUseTextureDiffuse);
	SetVariable_Vector(L"gColorDiffuse", XMFLOAT4(m_pColorDiffuse[0], m_pColorDiffuse[1], m_pColorDiffuse[2], 1));
	SetDiffuseTexture(L"Textures/Skulls_Diffusemap.tga");

	//Specular
	SetVariable_Vector(L"gColorSpecular", XMFLOAT4(m_pColorSpecular[0], m_pColorSpecular[1], m_pColorSpecular[2], 1));
	SetSpecularTexture(L"Textures/Specular_Level.tga");
	SetVariable_Scalar(L"gUseTextureSpecularIntensity", *m_pUseTextureSpecular);
	SetVariable_Scalar(L"gShininess", *m_pShininess);

	SetVariable_Scalar(L"gUseSpecularBlinn", *m_pUseBlinn);
	SetVariable_Scalar(L"gUseSpecularPhong", *m_pUsePhong);

	//Ambient
	SetVariable_Vector(L"gColorAmbient", XMFLOAT4(m_pColorAmbient[0], m_pColorAmbient[1], m_pColorAmbient[2], 1));
	SetVariable_Scalar(L"gAmbientIntensity", *m_pAmbientIntensity);

	//Normal Mapping
	SetVariable_Scalar(L"gFlipGreenChannel", *m_pFlipGreenChannel);
	SetVariable_Scalar(L"gUseTextureNormal", *m_pUseTextureNormal);
	SetNormalTexture(L"Textures/Skulls_Normalmap.tga");

	//Environment Mapping
	SetCubeMapTexture(L"Textures/Sunol_Cubemap.dds");
	SetVariable_Scalar(L"gUseEnvironmentMapping", *m_pUseEnvironmentMapping);
	SetVariable_Scalar(L"gReflectionStrength", *m_pReflectionStrength);
	SetVariable_Scalar(L"gRefractionStrength", *m_pRefractionStrength);
	SetVariable_Scalar(L"gRefractionIndex", *m_pRefractionIndex);
	
	//Fresnel Falloff
	SetVariable_Scalar(L"gUseFresnelFalloff", *m_pUseFresnelFalloff);
	SetVariable_Vector(L"gColorFresnel", XMFLOAT4(m_pColorFresnel[0], m_pColorFresnel[1], m_pColorFresnel[2], 1));
	SetVariable_Scalar(L"gFresnelPower", *m_pFresnelPower);
	SetVariable_Scalar(L"gFresnelMultiplier", *m_pFresnelMultiplier);
	SetVariable_Scalar(L"gFresnelHardness", *m_pFresnelHardness);

	//Opacity
	SetVariable_Scalar(L"gOpacityIntensity", *m_pOpacityIntensity);
	SetVariable_Scalar(L"gTextureOpacityIntensity", *m_pUseTextureOpacity);
	SetOpacityTexture(L"Textures/Skulls_Diffusemap.tga");
}

void UberMaterial::SetDiffuseTexture(const std::wstring& assetFile)
{
	m_pTextureDiffuse = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gTextureDiffuse", m_pTextureDiffuse);
}

void UberMaterial::SetNormalTexture(const std::wstring& assetFile)
{
	m_pTextureNormal = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gTextureNormal", m_pTextureNormal);
}

void UberMaterial::SetSpecularTexture(const std::wstring& assetFile)
{
	m_pTextureSpecular = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gTextureSpecularIntensity", m_pTextureSpecular);
}

void UberMaterial::SetOpacityTexture(const std::wstring& assetFile)
{
	m_pTextureOpacity = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gTextureOpacity", m_pTextureOpacity);
}

void UberMaterial::SetCubeMapTexture(const std::wstring& assetFile)
{
	m_pCubeEnvironment = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gCubeEnvironment", m_pCubeEnvironment);
}

void UberMaterial::OnGUI() const
{
	ImGui::Text("DIFFUSE");
	if (ImGui::Checkbox("gUseTextureDiffuse", m_pUseTextureDiffuse))
	{
		SetVariable_Scalar(L"gUseTextureDiffuse", *m_pUseTextureDiffuse);
	}
	if (ImGui::ColorEdit3("gColorDiffuse", m_pColorDiffuse, ImGuiColorEditFlags_NoInputs))
	{
		SetVariable_Vector(L"gColorDiffuse", XMFLOAT4(m_pColorDiffuse[0], m_pColorDiffuse[1], m_pColorDiffuse[2], 1));
	}
	ImGui::Spacing();

	ImGui::Text("SPECULAR");
	if (ImGui::Checkbox("gUseTextureSpecularIntensity", m_pUseTextureSpecular))
	{
		SetVariable_Scalar(L"gUseTextureSpecularIntensity", *m_pUseTextureSpecular);
	}
	if (ImGui::Checkbox("gUseSpecularBlinn", m_pUseBlinn))
	{
		SetVariable_Scalar(L"gUseSpecularBlinn", *m_pUseBlinn);
	}
	if (ImGui::Checkbox("gUseSpecularPhong", m_pUsePhong))
	{
		SetVariable_Scalar(L"gUseSpecularPhong", *m_pUsePhong);
	}
	if (ImGui::ColorEdit3("gColorSpecular", m_pColorSpecular, ImGuiColorEditFlags_NoInputs))
	{
		SetVariable_Vector(L"gColorSpecular", XMFLOAT4(m_pColorSpecular[0], m_pColorSpecular[1], m_pColorSpecular[2], 1));
	}
	if (ImGui::InputInt("gShininess", m_pShininess, 1, 10))
	{
		SetVariable_Scalar(L"gShininess", *m_pShininess);
	}
	ImGui::Spacing();
	
	ImGui::Text("AMBIENT");
	if (ImGui::ColorEdit4("gColorAmbient", m_pColorAmbient, ImGuiColorEditFlags_NoInputs))
	{
		SetVariable_Vector(L"gColorAmbient", XMFLOAT4(m_pColorAmbient[0], m_pColorAmbient[1], m_pColorAmbient[2], 1));
	}
	if (ImGui::InputFloat("gAmbientIntensity", m_pAmbientIntensity, 0.1f, 0.25f))
	{
		SetVariable_Scalar(L"gAmbientIntensity", *m_pAmbientIntensity);
	}
	ImGui::Spacing();

	ImGui::Text("NORMAL MAPPING");
	if (ImGui::Checkbox("gUseTextureNormal", m_pUseTextureNormal))
	{
		SetVariable_Scalar(L"gUseTextureNormal", *m_pUseTextureNormal);
	}
	if (ImGui::Checkbox("gFlipGreenChannel", m_pFlipGreenChannel))
	{
		SetVariable_Scalar(L"gFlipGreenChannel", *m_pFlipGreenChannel);
	}
	ImGui::Spacing();

	ImGui::Text("ENVIRONMENT MAPPING");
	if (ImGui::Checkbox("gUseEnvironmentMapping", m_pUseEnvironmentMapping))
	{
		SetVariable_Scalar(L"gUseEnvironmentMapping", *m_pUseEnvironmentMapping);
	}
	if (ImGui::InputFloat("gReflectionStrength", m_pReflectionStrength, 0.1f, 0.25f))
	{
		SetVariable_Scalar(L"gReflectionStrength", *m_pReflectionStrength);
	}
	if (ImGui::InputFloat("gRefractionStrength", m_pRefractionStrength, 0.1f, 0.25f))
	{
		SetVariable_Scalar(L"gRefractionStrength", *m_pRefractionStrength);
	}
	if (ImGui::InputFloat("gRefractionIndex", m_pRefractionIndex, 0.1f, 0.25f))
	{
		SetVariable_Scalar(L"gRefractionIndex", *m_pRefractionIndex);
	}
	ImGui::Spacing();

	ImGui::Text("FRESNEL FALLOFF");
	if (ImGui::Checkbox("gUseFresnelFalloff", m_pUseFresnelFalloff))
	{
		SetVariable_Scalar(L"gUseFresnelFalloff", *m_pUseFresnelFalloff);
	}
	if (ImGui::ColorEdit3("gColorFresnel", m_pColorFresnel, ImGuiColorEditFlags_NoInputs))
	{
		SetVariable_Vector(L"gColorFresnel", XMFLOAT4(m_pColorFresnel[0], m_pColorFresnel[1], m_pColorFresnel[2], 1));
	}
	if (ImGui::InputFloat("gFresnelPower", m_pFresnelPower, 0.1f, 0.25f))
	{
		SetVariable_Scalar(L"gFresnelPower", *m_pFresnelPower);
	}
	if (ImGui::InputFloat("gFresnelMultiplier", m_pFresnelMultiplier, 0.1f, 0.25f))
	{
		SetVariable_Scalar(L"gFresnelMultiplier", *m_pFresnelMultiplier);
	}
	if (ImGui::InputFloat("gFresnelHardness", m_pFresnelHardness, 0.1f, 0.25f))
	{
		SetVariable_Scalar(L"gFresnelHardness", *m_pFresnelHardness);
	}
	ImGui::Spacing();

	ImGui::Text("OPACITY");
	if (ImGui::Checkbox("gTextureOpacityIntensity", m_pUseTextureOpacity))
	{
		SetVariable_Scalar(L"gTextureOpacityIntensity", *m_pUseTextureOpacity);
	}
	if (ImGui::InputFloat("gOpacityIntensity", m_pOpacityIntensity, 0.1f, 0.25f))
	{
		SetVariable_Scalar(L"gOpacityIntensity", *m_pOpacityIntensity);
	}
}