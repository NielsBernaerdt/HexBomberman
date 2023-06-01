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

	PlayerPawn* m_pCharacter{};

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

	std::string m_Text{ "Hello World!" };
	XMFLOAT2 m_TextPosition{ 106.f, 120.f };
	XMFLOAT4 m_TextColor{ 1.f,1.f,1.f,1.f };
};
