#include "stdafx.h"
#include "HexBomberman.h"

#include "HexBomberman/HexGrid/HexGrid.h"
#include "HexBomberman/HexGrid/HexCell.h"
#include "HexBomberman/Player/PlayerPawn.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow.h"

void HexBomberman::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;

	//Lights
	m_SceneContext.pLights->SetDirectionalLight({ -95.6139526f,66.1346436f,-41.1850471f }, { 0.740129888f, -0.597205281f, 0.309117377f });

	//Materials
	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.f, 0.f, 1.f);

	//Ground Actor
	GameSceneExt::CreatePhysXGroundPlane(*this, pDefaultMaterial);

	//Ground Object
	//*********
	//const auto pGroundMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	//pGroundMaterial->SetDiffuseTexture(L"Textures/GroundBrick.jpg");
	////***********
	//const auto pGroundObj = new GameObject();
	//const auto pGroundModel = new ModelComponent(L"Meshes/UnitPlane.ovm");
	//pGroundModel->SetMaterial(pGroundMaterial);
	////***********
	//pGroundObj->AddComponent(pGroundModel);
	//pGroundObj->GetTransform()->Scale(10.0f, 10.0f, 10.0f);
	//AddChild(pGroundObj);

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
	m_pCharacter->GetTransform()->Translate(-20.f, 1.f, 0.f);

	//Input
	auto inputAction = InputAction(CharacterMoveLeft, InputState::down, VK_LEFT);
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveRight, InputState::down, VK_RIGHT);
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveForward, InputState::down, VK_UP);
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveBackward, InputState::down, VK_DOWN);
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterPlaceBomb, InputState::released, VK_SPACE);
	m_SceneContext.pInput->AddInputAction(inputAction);

	//Set Trigger Callback functions
	auto callback = [=](GameObject* pTriggerObject, GameObject* pOtherObject, PxTriggerAction triggerAction)
	{
		if (pOtherObject == m_pCharacter && triggerAction == PxTriggerAction::ENTER)
		{
			m_pCharacter->SetCurrentTile(pTriggerObject->GetComponent<HexCell>());
		}
	};

	for (const auto& cell : m_pHexGrid->GetGrid())
	{
		cell->GetGameObject()->SetOnTriggerCallBack(callback);
	}
}

void HexBomberman::OnGUI()
{
	m_pCharacter->DrawImGui();
}