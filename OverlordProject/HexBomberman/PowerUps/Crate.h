#pragma once

class HexCell;

class Crate : public BaseComponent
{
public:
	Crate() = default;
	~Crate() override = default;

	Crate(const Crate& other) = delete;
	Crate(Crate&& other) noexcept = delete;
	Crate& operator=(const Crate& other) = delete;
	Crate& operator=(Crate&& other) noexcept = delete;

	void SpawnPowerUp(HexCell* pHexCellParent) const;

protected:
	void Initialize(const SceneContext& sceneContext) override;

private:
	const float m_SpawnChance{ 0.25f };
};
