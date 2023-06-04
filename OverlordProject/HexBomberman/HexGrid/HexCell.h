#pragma once
#include "HexUtils.h"

class PlayerPawn;
class BasePowerUp;
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
	std::vector<HexCell*> GetTilesToExplode(int length) const;

	void PlaceBomb(PlayerPawn* pPlayer, int blastRange);

	bool HasCrate() const { return m_HasCrate; }
	void DestroyCrate(bool spawnCrate = true);

	bool HasPowerUp() { return m_HasPowerUp; }
	void AddPowerUp(BasePowerUp* pPowerUp);
	void CollectPowerUp(PlayerPawn* pPlayer);

protected:
	void Initialize(const SceneContext&) override;
	void Update(const SceneContext&) override;

private:
	std::vector<HexCell*> m_pNeighbors{6, nullptr};
	bool m_UpdateOnce{ true };

	bool m_HasCrate{ false };
	Crate* m_pCrateComponent{ nullptr };
	bool m_HasPowerUp{ false };
	BasePowerUp* m_pPowerUp{ nullptr };
};