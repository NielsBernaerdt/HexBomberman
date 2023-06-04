#pragma once

class PostBloom;
class HexGrid;
class PlayerPawn;

class HexBomberman : public GameScene
{
public:
	HexBomberman() : GameScene(L"HexBomberman") {}
	~HexBomberman() override;

	HexBomberman(const HexBomberman& other) = delete;
	HexBomberman(HexBomberman&& other) noexcept = delete;
	HexBomberman& operator=(const HexBomberman& other) = delete;
	HexBomberman& operator=(HexBomberman&& other) noexcept = delete;

	void TogglePause();
	void PlayerDied(PlayerPawn* pPlayer);

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override {}
	void OnGUI() override;

private:
	//Grid
	HexGrid* m_pHexGrid{ nullptr };
	const int m_BombBlastRange{ 2 };
	bool m_IsPlayerStartingAreaCleared{ false };
	//Players
	enum InputIds
	{
		CharacterMoveLeft,
		CharacterMoveRight,
		CharacterMoveForward,
		CharacterMoveBackward,
		CharacterPlaceBomb,
		PauseGame
	};
	std::vector<PlayerPawn*> m_pPlayers{};
	XMFLOAT2 m_pPlayerStartPositions[4]{ XMFLOAT2{0.f, 0.f}, XMFLOAT2{16.35f, 13.f}, XMFLOAT2{1.f, 13.f}, XMFLOAT2{15.43f, 0.f} };
	std::vector<PlayerPawn*> m_pPlayersToDie{};
	//HUD & UI
	//Contol Scheme
	SpriteComponent* m_pControllerScheme{ nullptr };
	inline static bool m_ShowControllerScheme{true};
	float m_AccControllerScheme{};
	//Pause Menu
	SpriteComponent* m_pPauseMenu{ nullptr };
	SpriteComponent* m_pResume{ nullptr };
	SpriteComponent* m_pRestart{ nullptr };
	SpriteComponent* m_pExit{ nullptr };
	bool IsOverlapping(const SpriteComponent* pSpriteComponent) const;
	//Victory Menu
	bool m_ShowVictoryScreen{ false };
	SpriteComponent* m_pVictoryMenu{ nullptr };
	SpriteComponent* m_pRestartVictoryMenu{ nullptr };
	//Text
	SpriteFont* m_pFont{};
	XMFLOAT2 m_TextPosition{ 260.f, 260.f };
	XMFLOAT4 m_TextColor{ 1.f,1.f,1.f,1.f };
	//Misc
	PostBloom* m_pPostBloom{};
	bool m_IsVibrating{ false };
	float m_AccVibTime{};
	const float m_MaxVibTime{ 0.5f };

	void CheckVictoryCondition();
	void ClearPlayerStartingArea();
	bool ManageControllerScheme();
	void ManageControllerVibration();
	void ManagerPlayerDeaths();
	void ManageInput();
	void UpdateTextSprites() const;
};