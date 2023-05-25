#pragma once
#include "BasePowerUp.h"

class IncreaseMovementSpeed : public BasePowerUp
{
public:
	void CollectPowerUp(PlayerPawn* /*pPlayer*/) override {}
};
