#pragma once

class HexGrid;
class PlayerPawn;

class HexBomberman : public GameScene
{
public:
	HexBomberman() : GameScene(L"HexBomberman") {}
	~HexBomberman() override = default;

	HexBomberman(const HexBomberman& other) = delete;
	HexBomberman(HexBomberman&& other) noexcept = delete;
	HexBomberman& operator=(const HexBomberman& other) = delete;
	HexBomberman& operator=(HexBomberman&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override {}
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
		CharacterPlaceBomb
	};

	PlayerPawn* m_pCharacter{};
};
