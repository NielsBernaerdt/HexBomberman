#pragma once
#include "BasePowerUp.h"

class ExtraBomb : public BasePowerUp
{
public:
	ExtraBomb() = default;
	~ExtraBomb() override = default;
	ExtraBomb(const ExtraBomb& other) = delete;
	ExtraBomb(ExtraBomb&& other) noexcept = delete;
	ExtraBomb& operator=(const ExtraBomb& other) = delete;
	ExtraBomb& operator=(ExtraBomb&& other) noexcept = delete;

	void CollectPowerUp(PlayerPawn* pPlayer) override;

protected:
	void Initialize(const SceneContext& sceneContext) override;
};