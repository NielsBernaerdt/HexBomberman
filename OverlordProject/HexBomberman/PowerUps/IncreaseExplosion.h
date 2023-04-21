#pragma once

class IncreaseExplosion : public BaseComponent
{
public:
	IncreaseExplosion() = default;
	~IncreaseExplosion() override = default;
	IncreaseExplosion(const IncreaseExplosion& other) = delete;
	IncreaseExplosion(IncreaseExplosion&& other) noexcept = delete;
	IncreaseExplosion& operator=(const IncreaseExplosion& other) = delete;
	IncreaseExplosion& operator=(IncreaseExplosion&& other) noexcept = delete;

protected:
	void Initialize(const SceneContext& sceneContext) override;

private:

};