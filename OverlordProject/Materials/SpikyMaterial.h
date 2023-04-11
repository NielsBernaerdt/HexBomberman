#pragma once

class SpikyMaterial : public Material<SpikyMaterial>
{
public:
	SpikyMaterial() : Material<SpikyMaterial>(L"Effects/SpikyShader.fx") {}
	~SpikyMaterial() override;

	SpikyMaterial(const SpikyMaterial& other) = delete;
	SpikyMaterial(SpikyMaterial&& other) noexcept = delete;
	SpikyMaterial& operator=(const SpikyMaterial& other) = delete;
	SpikyMaterial& operator=(SpikyMaterial&& other) noexcept = delete;

	void OnGUI() const;

protected:
	void InitializeEffectVariables() override;

private:
	float* m_pSpikeLength = new float{ 0.2f };
	float* m_pColorDiffuse = new float[3] {1.0f, 1.0f, 1.0f};
};