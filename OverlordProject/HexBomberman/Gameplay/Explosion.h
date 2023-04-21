#pragma once

class Explosion : public BaseComponent
{
public:
	Explosion() = default;
	~Explosion() override = default;
	Explosion(const Explosion& other) = delete;
	Explosion(Explosion&& other) noexcept = delete;
	Explosion& operator=(const Explosion& other) = delete;
	Explosion& operator=(Explosion&& other) noexcept = delete;

protected:
	void Initialize(const SceneContext& sceneContext) override;

private:
};