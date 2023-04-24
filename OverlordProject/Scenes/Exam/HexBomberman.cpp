#include "stdafx.h"
#include "HexBomberman.h"

#include "HexBomberman/HexGrid/HexGrid.h"
#include "HexBomberman/HexGrid/HexCell.h"
#include "HexBomberman/Player/PlayerPawn.h"

void HexBomberman::Initialize()
{
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
	//m_pCharacter->GetTransform()->Translate(0.f, 5.f, 0.f);

	//Input
	auto inputAction = InputAction(CharacterMoveLeft, InputState::down, VK_LEFT);
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveRight, InputState::down, VK_RIGHT);
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveForward, InputState::down, VK_UP);
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveBackward, InputState::down, VK_DOWN);
	m_SceneContext.pInput->AddInputAction(inputAction);
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

			pPickedCell->StartExplosion();

			for(const auto neighbor : pPickedCell->GetTilesToExplode(blastRange))
			{
				if (neighbor == nullptr)
					continue;

				neighbor->StartExplosion();
			}
		}
	}
}