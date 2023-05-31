#include "stdafx.h"
#include "HexBomberman.h"

#include "HexBomberman/Gameplay/Bomb.h"
#include "HexBomberman/HexGrid/HexGrid.h"
#include "HexBomberman/HexGrid/HexCell.h"
#include "HexBomberman/Player/PlayerPawn.h"
#include "Materials/Post/PostBloom.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow.h"

void HexBomberman::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;
	m_SceneContext.settings.drawPhysXDebug = false;
	m_SceneContext.settings.drawGrid = false;

	m_SceneContext.useDeferredRendering = true;

	//Lights

	//Directional
	m_SceneContext.pLights->SetDirectionalLight({ -95.6139526f,66.1346436f,-41.1850471f }, { 0.740129888f, -0.597205281f, 0.309117377f });

	//Spot Light
	Light light = {};
	light.isEnabled = true;
	light.position = { 1.f,3.f,1.f,1.0f };
	//light.color = { 0.f,1.f,0.f,1.f };
	light.color = { 1.f,1.f,1.f,1.f };
	light.intensity = 1.f;
	light.range = 5.0f;
	light.type = LightType::Point;
	m_SceneContext.pLights->AddLight(light);

	////Point Light
	//light = {};
	//light.isEnabled = true;
	//light.position = { 0.f,10.f,0.f,1.0f };
	//light.color = { 0.f,1.f,0.f,1.f };
	//light.intensity = 1.f;
	//light.range = 30.0f;
	//light.type = LightType::Point;
	//m_SceneContext.pLights->AddLight(light);

	//Materials
	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.f, 0.f, 1.f);

	//Ground Actor
	GameSceneExt::CreatePhysXGroundPlane(*this, pDefaultMaterial);

	//Ground Object
	//*********
	const auto pGroundMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pGroundMaterial->SetDiffuseTexture(L"Textures/GroundBrick.jpg");
	//***********
	const auto pGroundObj = AddChild(new GameObject());
	const auto pGroundModel = pGroundObj->AddComponent(new ModelComponent(L"Meshes/UnitPlane.ovm"));
	pGroundModel->SetMaterial(pGroundMaterial);
	pGroundObj->GetTransform()->Scale(10.0f, 10.0f, 10.0f);
	pGroundObj->GetTransform()->Translate(0.f, -0.01f, 0.f);
	//***********

	//Hexagonal Grid
	m_pHexGrid = AddChild(new HexGrid{});

	//Character
	CharacterDesc characterDesc{ pDefaultMaterial };
	characterDesc.actionId_MoveForward = CharacterMoveForward;
	characterDesc.actionId_MoveBackward = CharacterMoveBackward;
	characterDesc.actionId_MoveLeft = CharacterMoveLeft;
	characterDesc.actionId_MoveRight = CharacterMoveRight;
	characterDesc.actionId_PlaceBomb = CharacterPlaceBomb;
	characterDesc.actionId_PauseGame = PauseGame;

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

	inputAction = InputAction(PauseGame, InputState::released, 'P');
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
	
	//Post Processing Stack
	//=====================

	//Particle System to help post processing work (TODO)
	ParticleEmitterSettings settings{};
	settings.velocity = { 0.f,6.f,0.f };
	settings.minSize = 0.2f;
	settings.maxSize = 0.6f;
	settings.minEnergy = 0.f;
	settings.maxEnergy = 0.f;
	settings.minScale = 3.5f;
	settings.maxScale = 5.5f;
	settings.minEmitterRadius = .0f;
	settings.maxEmitterRadius = .1f;
	settings.color = { 1.f,1.f,1.f, 1.f };

	auto pParticle = AddChild(new GameObject);
	pParticle->AddComponent(new ParticleEmitterComponent(L"Textures/Fire.png", settings, 200));
	pParticle->GetTransform()->Translate(0.f, -1.f, 1.f);

	m_pPostBloom = MaterialManager::Get()->CreateMaterial<PostBloom>();
	AddPostProcessingEffect(m_pPostBloom);
	m_pPostBloom->SetIsEnabled(false);


	//Pause Menu
	const auto pBackground = new GameObject{};
	m_pBackground = pBackground->AddComponent(new SpriteComponent(L"Textures/Background.png"));

	const auto pResumeButton = new GameObject{};
	m_pResume = pResumeButton->AddComponent(new SpriteComponent(L"Textures/Background.png"));
	m_pResume->GetTransform()->Scale(0.5f, 0.12f, 1.f);
	m_pResume->GetTransform()->Translate(300.f, 100.f, 0.f);

	const auto pRestartButton = new GameObject{};
	m_pRestart = pRestartButton->AddComponent(new SpriteComponent(L"Textures/Background.png"));
	m_pRestart->GetTransform()->Scale(0.5f, 0.12f, 1.f);
	m_pRestart->GetTransform()->Translate(300.f, 300.f, 0.f);

	const auto pExitButton = new GameObject{};
	m_pExit = pExitButton->AddComponent(new SpriteComponent(L"Textures/Background.png"));
	m_pExit->GetTransform()->Scale(0.5f, 0.12f, 1.f);
	m_pExit->GetTransform()->Translate(300.f, 500.f, 0.f);
}

void HexBomberman::Update()
{
	if(m_SceneContext.settings.isGamePaused != m_PreviousPauseState)
	{
		TogglePause();
	}
	m_PreviousPauseState = m_SceneContext.settings.isGamePaused;

	if(m_SceneContext.settings.isGamePaused == true)
	{
		if (InputManager::IsMouseButton(InputState::pressed, VK_LBUTTON))
		{
			if (IsOverlapping(m_pResume))
			{
				m_SceneContext.settings.isGamePaused = !m_SceneContext.settings.isGamePaused;
			}
			else if (IsOverlapping(m_pRestart))
			{
				PostQuitMessage(0);
			}
			else if (IsOverlapping(m_pExit))
			{
				PostQuitMessage(0);
			}
		}
	}
}

void HexBomberman::TogglePause()
{
	if(m_SceneContext.settings.isGamePaused)
	{
		AddChild(m_pBackground->GetGameObject());
		AddChild(m_pResume->GetGameObject());
		AddChild(m_pRestart->GetGameObject());
		AddChild(m_pExit->GetGameObject());
	}
	else
	{
		RemoveChild(m_pBackground->GetGameObject());
		RemoveChild(m_pResume->GetGameObject());
		RemoveChild(m_pRestart->GetGameObject());
		RemoveChild(m_pExit->GetGameObject());
	}
}


bool HexBomberman::IsOverlapping(SpriteComponent* pSpriteComponent) const
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

void HexBomberman::OnGUI()
{
	DeferredRenderer::Get()->DrawImGui();
	//
	bool isEnabled = m_pPostBloom->IsEnabled();
	ImGui::Checkbox("Bloom PP", &isEnabled);
	m_pPostBloom->SetIsEnabled(isEnabled);
	//
	m_pCharacter->DrawImGui();
}