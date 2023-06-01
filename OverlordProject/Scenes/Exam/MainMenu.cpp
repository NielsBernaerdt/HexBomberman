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
}
void MainMenu::Update()
{
	if (InputManager::IsMouseButton(InputState::pressed, VK_LBUTTON))
	{
		if (IsOverlapping(m_pStart))
		{
			SceneManager::Get()->SetActiveGameScene(L"HexBomberman");
		}
		else if(IsOverlapping(m_pExit))
		{
			PostQuitMessage(0);
		}
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