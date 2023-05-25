#include "stdafx.h"
#include "HexBomberman.h"

#include "HexBomberman/Gameplay/Bomb.h"
#include "HexBomberman/Gameplay/Explosion.h"
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
	characterDesc.actionId_PlaceBomb = CharacterPlaceBomb;

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

	inputAction = InputAction(CharacterPlaceBomb, InputState::down, VK_SPACE);
	m_SceneContext.pInput->AddInputAction(inputAction);

	//Set Trigger Callback functions
	auto callback = [=](GameObject* pTriggerObject, GameObject* pOtherObject, PxTriggerAction triggerAction)
	{
		if (pOtherObject == m_pCharacter && triggerAction == PxTriggerAction::ENTER)
		{
			m_pCharacter->SetCurrentTile(pTriggerObject->GetComponent<HexCell>());
		}
	};

	for(const auto& cell: m_pHexGrid->GetGrid())
	{
		cell->GetGameObject()->SetOnTriggerCallBack(callback);
	}
}

void HexBomberman::OnGUI()
{
	m_pCharacter->DrawImGui();
}