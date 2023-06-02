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

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override {}
	void OnGUI() override;

private:
	HexGrid* m_pHexGrid{ nullptr };

	const int blastRange{ 2 };

	enum InputIds
	{
		CharacterMoveLeft,
		CharacterMoveRight,
		CharacterMoveForward,
		CharacterMoveBackward,
		CharacterPlaceBomb,
		PauseGame
	};

	std::vector<PlayerPawn*> m_pCharacters{};

	PostBloom* m_pPostBloom{};

	//Pause Menu
	bool m_PreviousPauseState{ false };
	SpriteComponent* m_pPauseMenu{ nullptr };
	SpriteComponent* m_pResume{ nullptr };
	SpriteComponent* m_pRestart{ nullptr };
	SpriteComponent* m_pExit{ nullptr };
	bool IsOverlapping(SpriteComponent* pSpriteComponent) const;

	//HUD
	/*SpriteComponent* m_pHUD{ nullptr };*/
	SpriteFont* m_pFont{};

	XMFLOAT2 m_TextPosition{ 260.f, 260.f };
	XMFLOAT4 m_TextColor{ 1.f,1.f,1.f,1.f };

	//Player Start Positions
	XMFLOAT2 m_StartPos[4]{ XMFLOAT2{0.f, 0.f}, XMFLOAT2{16.35f, 13.f}, XMFLOAT2{1.f, 13.f}, XMFLOAT2{15.43f, 0.f} };
};