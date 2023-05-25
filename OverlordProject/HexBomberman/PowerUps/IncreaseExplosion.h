#pragma once
#include "BasePowerUp.h"

class IncreaseExplosion : public BasePowerUp
{
public:
	IncreaseExplosion() = default;
	~IncreaseExplosion() override = default;
	IncreaseExplosion(const IncreaseExplosion& other) = delete;
	IncreaseExplosion(IncreaseExplosion&& other) noexcept = delete;
	IncreaseExplosion& operator=(const IncreaseExplosion& other) = delete;
	IncreaseExplosion& operator=(IncreaseExplosion&& other) noexcept = delete;

	void CollectPowerUp(PlayerPawn* pPlayer) override;

protected:
	void Initialize(const SceneContext& sceneContext) override;

private:

};
