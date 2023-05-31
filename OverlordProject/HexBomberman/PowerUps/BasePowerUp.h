#pragma once

class PlayerPawn;

class BasePowerUp : public BaseComponent
{
public:
	BasePowerUp() = default;
	~BasePowerUp() override = default;
	BasePowerUp(const BasePowerUp& other) = delete;
	BasePowerUp(BasePowerUp&& other) noexcept = delete;
	BasePowerUp& operator=(const BasePowerUp& other) = delete;
	BasePowerUp& operator=(BasePowerUp&& other) noexcept = delete;

	virtual void CollectPowerUp(PlayerPawn* /*pPlayer*/) {}

protected:
	void Update(const SceneContext&) override;
};
