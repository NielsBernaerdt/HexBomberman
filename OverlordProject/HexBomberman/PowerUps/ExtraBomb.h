#pragma once

class ExtraBomb : public BaseComponent
{
public:
	ExtraBomb() = default;
	~ExtraBomb() override = default;
	ExtraBomb(const ExtraBomb& other) = delete;
	ExtraBomb(ExtraBomb&& other) noexcept = delete;
	ExtraBomb& operator=(const ExtraBomb& other) = delete;
	ExtraBomb& operator=(ExtraBomb&& other) noexcept = delete;

protected:
	void Initialize(const SceneContext& sceneContext) override;

private:
};