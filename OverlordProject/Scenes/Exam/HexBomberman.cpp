#include "stdafx.h"
#include "HexBomberman.h"

#include "HexBomberman/Gameplay/Bomb.h"
#include "HexBomberman/HexGrid/HexGrid.h"
#include "HexBomberman/HexGrid/HexCell.h"
#include "HexBomberman/Player/PlayerPawn.h"

void HexBomberman::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;

	//Materials
	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.f, 0.f, 1.f);

	//Ground Plane
	GameSceneExt::CreatePhysXGroundPlane(*this, pDefaultMaterial);

	//Hexagonal Grid
	m_pHexGrid = AddChild(new HexGrid{});

	//Character
	CharacterDesc characterDesc{ pDefaultMaterial };
	characterDesc.actionId_MoveForward = CharacterMoveForward;
	characterDesc.actionId_MoveBackward = CharacterMoveBackward;
	characterDesc.actionId_MoveLeft = CharacterMoveLeft;
	characterDesc.actionId_MoveRight = CharacterMoveRight;

	m_pCharacter = AddChild(new PlayerPawn(characterDesc));
	m_pCharacter->GetTransform()->Translate(-20.f, 0.f, 0.f);

	//Input
	auto inputAction = InputAction(CharacterMoveLeft, InputState::down, VK_LEFT);
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveRight, InputState::down, VK_RIGHT);
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveForward, InputState::down, VK_UP);
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveBackward, InputState::down, VK_DOWN);
	m_SceneContext.pInput->AddInputAction(inputAction);

	//Bomb Object
	const auto bombObject = AddChild(new GameObject{});
	bombObject->AddComponent(new Bomb{ nullptr });
	bombObject->GetTransform()->Translate(-3.f, 0.f, 0.f);
	bombObject->GetTransform()->Scale(1.1f, 1.1f, 1.1f);


}

void HexBomberman::OnGUI()
{
	m_pCharacter->DrawImGui();
}

void HexBomberman::Update()
{
	if (InputManager::IsMouseButton(InputState::pressed, VK_RBUTTON))
	{
		if (const auto pPickedObject = m_SceneContext.pCamera->Pick())
		{
			const auto pPickedCell = pPickedObject->GetComponent<HexCell>();

			pPickedCell->PlaceBomb();
		}
	}
}