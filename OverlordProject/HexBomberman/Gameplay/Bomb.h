#pragma once

class PlayerPawn;
class HexCell;
class Explosion;

class Bomb : public BaseComponent
{
public:
	Bomb(HexCell* ownerCell, PlayerPawn* pPlayer, int blastRange);
	~Bomb() override = default;

	Bomb(const Bomb& other) = delete;
	Bomb(Bomb&& other) noexcept = delete;
	Bomb& operator=(const Bomb& other) = delete;
	Bomb& operator=(Bomb&& other) noexcept = delete;

	void Explode(int blastRange);

protected:
	void Initialize(const SceneContext& sceneContext) override;
	void Update(const SceneContext&) override;

private:
	HexCell* m_pOwnerCell;
	PlayerPawn* m_pPlayer;

	std::vector<Explosion*> m_pExplosionObjects{};
	const float m_ExplosionDuration{ 1.f };
	const int m_BlastRange{ 0 };
	float m_AccTime{};
	void EndExplosion() const;
};