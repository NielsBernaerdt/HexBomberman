#pragma once

class PongScene final : public GameScene
{
public:
	PongScene() : GameScene(L"PhongScene") {}
	~PongScene() override = default;

	PongScene(const PongScene& other) = delete;
	PongScene(PongScene&& other) noexcept = delete;
	PongScene& operator=(const PongScene& other) = delete;
	PongScene& operator=(PongScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void OnGUI() override;

	void Start();
	void Reset() override;

private:
	bool m_IsStarted{ false };

	GameObject* m_pBall = nullptr;
	GameObject* m_pPeddleLeft = nullptr;
	GameObject* m_pPeddleRight = nullptr;

	GameObject* m_pUpperBoundary = nullptr;
	GameObject* m_pLowerBoundary = nullptr;

	GameObject* m_pTriggerLeft = nullptr;
	GameObject* m_pTriggerRight = nullptr;
};