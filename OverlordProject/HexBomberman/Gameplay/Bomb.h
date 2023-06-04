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
	const int m_BlastRange;

	float m_AccTime{};
	std::vector<Explosion*> m_pExplosionObjects{};
	const float m_ExplosionDuration{ 1.f };
	std::vector<GameObject*> m_pDangerTiles{};
	bool m_HasSpawnedDanger{ false };
	GameObject* pParticle{ nullptr };
	bool m_HasExploded{ false };
	bool m_HasUpdatedParticlePosition{ false };
	FMOD::Channel* m_pChannel2D{};

	void SpawnDangerSigns();
	void EndExplosion();
};