#pragma once
#include "HexUtils.h"

class HexCell : public BaseComponent
{
public:
	HexCell() = default;
	~HexCell() override = default;

	HexCell(const HexCell& other) = delete;
	HexCell(HexCell&& other) noexcept = delete;
	HexCell& operator=(const HexCell& other) = delete;
	HexCell& operator=(HexCell&& other) noexcept = delete;

	std::vector<HexCell*>& GetNeighbors() { return m_pNeighbors; }
	HexCell* GetNeighbor(HexDirection direction) const;
	void SetNeighbor(HexDirection direction, HexCell* cell);

	void SpawnCrate();
	std::vector<HexCell*> GetBlastTiles(int length) const;

protected:
	void Initialize(const SceneContext&) override;
	void Update(const SceneContext&) override;

private:
	std::vector<HexCell*> m_pNeighbors{6};

	GameObject* m_pGroundTile{ nullptr };

	bool m_IsExploding{ false };
	const float m_ExplosionDuration{ 0.5f };
	float m_AccTime{};
	GameObject* m_pCrate{ nullptr };
};
