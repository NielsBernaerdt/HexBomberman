#pragma once
#include "HexUtils.h"

class Crate;

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

	void PlaceBomb();
	
	std::vector<HexCell*> GetTilesToExplode(int length) const;

	bool HasCrate() const { return m_HasCrate; }
	void DestroyCrate();

protected:
	void Initialize(const SceneContext&) override;

private:
	std::vector<HexCell*> m_pNeighbors{6, nullptr};

	GameObject* m_pGroundTile{ nullptr };

	bool m_HasCrate{ true };
	Crate* m_pCrateComponent{ nullptr };
};
