#pragma once

class HexCell;

class Explosion : public BaseComponent
{
public:
	Explosion(HexCell* pOwnerTile);
	~Explosion() override = default;
	Explosion(const Explosion& other) = delete;
	Explosion(Explosion&& other) noexcept = delete;
	Explosion& operator=(const Explosion& other) = delete;
	Explosion& operator=(Explosion&& other) noexcept = delete;

	HexCell* GetOwnerTile() const { return m_pOwnerTile; }

protected:
	void Initialize(const SceneContext& sceneContext) override;

private:
	HexCell* m_pOwnerTile{ nullptr };

	void AddCubeComp() const;
};