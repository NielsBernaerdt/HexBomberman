#pragma once

class UberMaterial : public Material<UberMaterial>
{
public:
	UberMaterial() : Material<UberMaterial>(L"Effects/UberShader.fx") {}
	~UberMaterial() override;

	UberMaterial(const UberMaterial& other) = delete;
	UberMaterial(UberMaterial&& other) noexcept = delete;
	UberMaterial& operator=(const UberMaterial& other) = delete;
	UberMaterial& operator=(UberMaterial&& other) noexcept = delete;

	void SetDiffuseTexture(const std::wstring& assetFile);
	void SetNormalTexture(const std::wstring& assetFile);
	void SetSpecularTexture(const std::wstring& assetFile);
	void SetOpacityTexture(const std::wstring& assetFile);
	void SetCubeMapTexture(const std::wstring& assetFile);

	void OnGUI() const;

protected:
	void InitializeEffectVariables() override;

private:
	//Diffuse
	bool* m_pUseTextureDiffuse = new bool{ true };
	float* m_pColorDiffuse = new float[3] {1, 0, 0};
	TextureData* m_pTextureDiffuse = nullptr;

	//Specular
	float* m_pColorSpecular = new float[3] {0, 1, 0};
	TextureData* m_pTextureSpecular = nullptr;
	bool* m_pUseTextureSpecular = new bool{ true };
	int* m_pShininess = new int{ 40 };

	bool* m_pUseBlinn = new bool{ false };
	bool* m_pUsePhong = new bool{ true };

	//Ambient
	float* m_pColorAmbient = new float[3] {0, 0, 0};
	float* m_pAmbientIntensity = new float{ 0.f };

	//Normal Mapping
	bool* m_pFlipGreenChannel = new bool{ true };
	bool* m_pUseTextureNormal = new bool{ true };
	TextureData* m_pTextureNormal = nullptr;

	//Environment Mapping
	TextureData* m_pCubeEnvironment = nullptr;
	bool* m_pUseEnvironmentMapping = new bool{ true };
	float* m_pReflectionStrength = new float{ 0.6f };
	float* m_pRefractionStrength = new float{ 0.4f };
	float* m_pRefractionIndex = new float{ 1.1f };

	//Fresnel Falloff
	bool* m_pUseFresnelFalloff = new bool{ true };
	float* m_pColorFresnel = new float[3] {1, 1, 1};
	float* m_pFresnelPower = new float{ 2.f };
	float* m_pFresnelMultiplier = new float{ 1.5f };
	float* m_pFresnelHardness = new float{ 2.f };

	//Opacity
	float* m_pOpacityIntensity = new float{ 1.f };
	bool* m_pUseTextureOpacity = new bool{ false };
	TextureData* m_pTextureOpacity = nullptr;
};