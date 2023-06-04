#pragma once

class DiffuseMaterial_Shadow;
class HexCell;

class HexGrid : public GameObject
{
public:
	HexGrid() = default;
	~HexGrid() override = default;

	HexGrid(const HexGrid& other) = delete;
	HexGrid(HexGrid&& other) noexcept = delete;
	HexGrid& operator=(const HexGrid& other) = delete;
	HexGrid& operator=(HexGrid&& other) noexcept = delete;

	std::vector<HexCell*>& GetGrid() { return m_pGrid; }

protected:
	void Initialize(const SceneContext&) override;

private:
	std::vector<HexCell*> m_pGrid{};
	const int m_TotalColumns{ 10 };
	const int m_TotalRows{ 10 };

	const float m_HexOuterRadius{ 1.f };
	const float m_HexInnerRadius{ m_HexOuterRadius * 0.866025404f }; // = sqrt(3)/2

	void ConstructGrid();
	void ConstructWalls();
	void AddWall(const XMFLOAT2& pos, const PxMaterial* pMaterial, DiffuseMaterial_Shadow* diffuse);
};