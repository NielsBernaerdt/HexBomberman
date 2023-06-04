#include "stdafx.h"
#include "MainMenu.h"

void MainMenu::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;
	m_SceneContext.settings.drawPhysXDebug = false;
	m_SceneContext.settings.drawGrid = false;

	m_SceneContext.useDeferredRendering = true;

	const auto pBackground = AddChild(new GameObject);
	pBackground->AddComponent(new SpriteComponent(L"Textures/Background.png"));

	const auto pStartButton = AddChild(new GameObject);
	m_pStart = pStartButton->AddComponent(new SpriteComponent(L"Textures/Button_Start.png"));
	m_pStart->GetTransform()->Translate(470.f, 700.f, 0.f);

	const auto pExitButton = AddChild(new GameObject);
	m_pExit = pExitButton->AddComponent(new SpriteComponent(L"Textures/Button_Exit.png"));
	m_pExit->GetTransform()->Translate(960.f, 700.f, 0.f);

	//Sound
	FMOD::Channel* m_pChannel2D{ nullptr };
	const auto pFmod = SoundManager::Get()->GetSystem();
	FMOD::Sound* pSound{};
	FMOD_RESULT result = pFmod->createStream("Resources/Sounds/BackgroundMusic.mp3", FMOD_DEFAULT | FMOD_LOOP_NORMAL, nullptr, &pSound);
	SoundManager::Get()->ErrorCheck(result);
	result = pFmod->playSound(pSound, nullptr, false, &m_pChannel2D);
	m_pChannel2D->setVolume(0.025f);
	SoundManager::Get()->ErrorCheck(result);
}
void MainMenu::Update()
{
	CheckInput();
}

void MainMenu::CheckInput() const
{
	if ((InputManager::IsMouseButton(InputState::pressed, VK_LBUTTON) && IsOverlapping(m_pStart))
		|| InputManager::IsGamepadButton(InputState::pressed, XINPUT_GAMEPAD_A, GamepadIndex::playerOne)
		|| InputManager::IsGamepadButton(InputState::pressed, XINPUT_GAMEPAD_A, GamepadIndex::playerTwo)
		|| InputManager::IsGamepadButton(InputState::pressed, XINPUT_GAMEPAD_A, GamepadIndex::playerThree)
		|| InputManager::IsGamepadButton(InputState::pressed, XINPUT_GAMEPAD_A, GamepadIndex::playerFour))
	{
		SceneManager::Get()->SetActiveGameScene(L"HexBomberman");
	}
	if ((InputManager::IsMouseButton(InputState::pressed, VK_LBUTTON) && IsOverlapping(m_pExit))
		|| InputManager::IsGamepadButton(InputState::pressed, XINPUT_GAMEPAD_B, GamepadIndex::playerOne)
		|| InputManager::IsGamepadButton(InputState::pressed, XINPUT_GAMEPAD_B, GamepadIndex::playerTwo)
		|| InputManager::IsGamepadButton(InputState::pressed, XINPUT_GAMEPAD_B, GamepadIndex::playerThree)
		|| InputManager::IsGamepadButton(InputState::pressed, XINPUT_GAMEPAD_B, GamepadIndex::playerFour))
	{
		PostQuitMessage(0);
	}
}

bool MainMenu::IsOverlapping(SpriteComponent* pSpriteComponent) const
{
	const auto posMouse{ m_SceneContext.pInput->GetMousePosition() };
	const auto spritePos{ pSpriteComponent->GetTransform()->GetWorldPosition() };
	auto spriteDimensions{ pSpriteComponent->GetTextureDimensions() };
	spriteDimensions.x *= pSpriteComponent->GetTransform()->GetScale().x;
	spriteDimensions.y *= pSpriteComponent->GetTransform()->GetScale().y;

	return (posMouse.x >= spritePos.x &&
		posMouse.x <= spritePos.x + spriteDimensions.x &&
		posMouse.y >= spritePos.y &&
		posMouse.y <= spritePos.y + spriteDimensions.y);
}