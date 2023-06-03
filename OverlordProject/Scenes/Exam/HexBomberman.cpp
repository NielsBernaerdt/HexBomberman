#include "stdafx.h"
#include "HexBomberman.h"

#include "HexBomberman/Gameplay/Bomb.h"
#include "HexBomberman/Gameplay/Explosion.h"
#include "HexBomberman/HexGrid/HexGrid.h"
#include "HexBomberman/HexGrid/HexCell.h"
#include "HexBomberman/Player/PlayerPawn.h"
#include "Materials/Post/PostBloom.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow.h"

HexBomberman::~HexBomberman()
{
	if(m_SceneContext.settings.isGamePaused == false)
	{
		delete m_pPauseMenu->GetGameObject();
	}
}

void HexBomberman::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;
	m_SceneContext.settings.drawPhysXDebug = false;
	m_SceneContext.settings.drawGrid = false;

	m_SceneContext.useDeferredRendering = true;

	//Camera
	m_SceneContext.pCamera->GetTransform()->Translate(6.165973f, 17.315273f, -4.472385f);
	dynamic_cast<FreeCamera*>(m_SceneContext.pCamera->GetGameObject())->SetRotation(60.f, 0.f);

	//Lights

	//Directional
	m_SceneContext.pLights->SetDirectionalLight({ -95.6139526f,66.1346436f,-41.1850471f }, { 0.740129888f, -0.597205281f, 0.309117377f });
	m_SceneContext.pLights->GetDirectionalLight().intensity = 0.01f;
	//m_SceneContext.pLights->GetDirectionalLight().isEnabled = false;

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

	//Sprite
	m_pFont = ContentManager::Load<SpriteFont>(L"SpriteFonts/Consolas_32.fnt");

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
	pGroundObj->GetTransform()->Scale(5.0f, 1.0f, 5.0f);
	pGroundObj->GetTransform()->Translate(5.f, -0.01f, 2.f);
	//***********

	//Hexagonal Grid
	m_pHexGrid = AddChild(new HexGrid{});
	XMFLOAT2 hudPos{ 30.f, 100.f };

	const int nrPlayers{ 4 };
	for (int i{}; i < nrPlayers; ++i)
	{
		//Character
		CharacterDesc characterDesc{ pDefaultMaterial };
		characterDesc.actionId_MoveForward = CharacterMoveForward + (10 * i);
		characterDesc.actionId_MoveBackward = CharacterMoveBackward + (10 * i);
		characterDesc.actionId_MoveLeft = CharacterMoveLeft + (10 * i);
		characterDesc.actionId_MoveRight = CharacterMoveRight + (10 * i);
		characterDesc.actionId_PlaceBomb = CharacterPlaceBomb + (10 * i);
		characterDesc.actionId_PauseGame = PauseGame + (10 * i);
		characterDesc.playerIdx = i;

		auto pCharacter = AddChild(new PlayerPawn(characterDesc));
		m_pCharacters.push_back(pCharacter);
		pCharacter->GetTransform()->Translate(m_StartPos[i].x, 1.f, m_StartPos[i].y);

		//Input
		auto inputAction = InputAction(CharacterMoveLeft + (10 * i), InputState::down, -1, -1, XINPUT_GAMEPAD_DPAD_LEFT, static_cast<GamepadIndex>(i));
		m_SceneContext.pInput->AddInputAction(inputAction);

		inputAction = InputAction(CharacterMoveRight + (10 * i), InputState::down, -1, -1, XINPUT_GAMEPAD_DPAD_RIGHT, static_cast<GamepadIndex>(i));
		m_SceneContext.pInput->AddInputAction(inputAction);

		inputAction = InputAction(CharacterMoveForward + (10 * i), InputState::down, -1, -1, XINPUT_GAMEPAD_DPAD_UP, static_cast<GamepadIndex>(i));
		m_SceneContext.pInput->AddInputAction(inputAction);

		inputAction = InputAction(CharacterMoveBackward + (10 * i), InputState::down, -1, -1, XINPUT_GAMEPAD_DPAD_DOWN, static_cast<GamepadIndex>(i));
		m_SceneContext.pInput->AddInputAction(inputAction);

		inputAction = InputAction(CharacterPlaceBomb + (10 * i), InputState::released, -1, -1, XINPUT_GAMEPAD_RIGHT_SHOULDER, static_cast<GamepadIndex>(i));
		m_SceneContext.pInput->AddInputAction(inputAction);

		inputAction = InputAction(PauseGame + (10 * i), InputState::released, -1, -1, XINPUT_GAMEPAD_START, static_cast<GamepadIndex>(i));
		m_SceneContext.pInput->AddInputAction(inputAction);

		//Set Trigger Callback functions for hexcells
		auto cellCallBack = [=](GameObject* pTriggerObject, GameObject* pOtherObject, PxTriggerAction triggerAction)
		{
			auto it = std::find_if(m_pCharacters.begin(), m_pCharacters.end(), [&](const GameObject* elem) {
				return elem == pOtherObject;
				});

			if (it != m_pCharacters.end() && triggerAction == PxTriggerAction::ENTER)
			{
				dynamic_cast<PlayerPawn*>(pOtherObject)->SetCurrentTile(pTriggerObject->GetComponent<HexCell>());
				Logger::LogDebug(L"Yay");
			}
		};
		for (const auto& cell : m_pHexGrid->GetGrid())
		{
			cell->GetGameObject()->SetOnTriggerCallBack(cellCallBack);
		}

		//HUD
		const auto pHUD = AddChild(new GameObject);
		//Player One
		const auto pPlayerOne = pHUD->AddChild(new GameObject);
		pPlayerOne->GetTransform()->Translate(hudPos.x, hudPos.y, 0.f);

		const auto pPortrait = pPlayerOne->AddChild(new GameObject);
		pPortrait->AddComponent(new SpriteComponent(L"Textures/UI_White.png"));
		pPortrait->GetTransform()->Translate(0.f, 80.f, 0.f);
		pPortrait->GetTransform()->Scale(0.8f, 0.8f, 1.f);

		const auto pBomb = pPlayerOne->AddChild(new GameObject);
		pBomb->AddComponent(new SpriteComponent(L"Textures/UI_BombIcon.png"));
		pBomb->GetTransform()->Translate(140.f, 130.f, 0.f);

		//second hud on 340 -> calculate delta
		hudPos.y += 150.f;
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
	m_pPauseMenu = pBackground->AddComponent(new SpriteComponent(L"Textures/Background.png"));

	const auto pResumeButton = pBackground->AddChild(new GameObject);
	m_pResume = pResumeButton->AddComponent(new SpriteComponent(L"Textures/Button_Resume.png"));
	m_pResume->GetTransform()->Translate(240.f, 700.f, 0.f);

	const auto pRestartButton = pBackground->AddChild(new GameObject);
	m_pRestart = pRestartButton->AddComponent(new SpriteComponent(L"Textures/Button_Restart.png"));
	m_pRestart->GetTransform()->Translate(730.f, 700.f, 0.f);

	const auto pExitButton = pBackground->AddChild(new GameObject);
	m_pExit = pExitButton->AddComponent(new SpriteComponent(L"Textures/Button_Exit.png"));
	m_pExit->GetTransform()->Translate(1230.f, 700.f, 0.f);
}

void HexBomberman::ClearPlayerStartingArea()
{
	//todo:: not error safe:
	const auto& pCellOne = m_pCharacters[0]->GetCurrentCell();
	const auto& pCellTwo = m_pCharacters[1]->GetCurrentCell();
	if( pCellOne && pCellTwo && pCellOne == pCellTwo)
		return;

	//voor elke speler hun cell (+ zorg dat er geen crate staat)
	//voor elke neighboring cell (+ zorg dat er geen crate staat)
	//voor 1 van hun neighbors: zorg dat er geen crate staat

	for (const auto& pPlayer : m_pCharacters)
	{
		if (const auto& pPlayerCell = pPlayer->GetCurrentCell())
		{
			m_IsAreaCleared = true;

			if (pPlayerCell->HasCrate()) pPlayerCell->DestroyCrate();

			for (const auto& pNeighbors : pPlayerCell->GetNeighbors())
			{
				if (pNeighbors == nullptr)
					continue;

				if (pNeighbors->HasCrate()) pNeighbors->DestroyCrate();

				for (const auto& pNeighborsNeighbor : pNeighbors->GetNeighbors())
				{
					if (pNeighborsNeighbor == nullptr)
						continue;

					if (pNeighborsNeighbor->HasCrate())
					{
						pNeighborsNeighbor->DestroyCrate(); //Dit resulteert in 1 crate per neighbor
					}
					else
					{
						break;
					}
				}
			}
		}
	}
}

void HexBomberman::Update()
{
	if(m_IsAreaCleared == false)
	{
		ClearPlayerStartingArea();
	}

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
				SceneManager::Get()->AddGameScene(new HexBomberman());
				SceneManager::Get()->NextScene();
			}
			else if (IsOverlapping(m_pExit))
			{
				PostQuitMessage(0);
			}
		}
	}
	else
	{
		XMFLOAT2 nrBombsPos{ m_TextPosition };
		XMFLOAT2 namePos{ 160.f, 190.f };
		for (int i{}; i < m_pCharacters.size(); ++i)
		{
			std::string text{};
			if (m_pCharacters[i] != nullptr)
				text = std::to_string((m_pCharacters[i]->GetNrBombs() - m_pCharacters[i]->GetNrBombsInPlay())) + "/" + std::to_string(m_pCharacters[i]->GetNrBombs());
			else
				text = "0/0";
			TextRenderer::Get()->DrawText(m_pFont, StringUtil::utf8_decode(text), nrBombsPos, m_TextColor);
			//
			//Player One Name
			TextRenderer::Get()->DrawText(m_pFont, StringUtil::utf8_decode("Player " + std::to_string(i + 1)), namePos, XMFLOAT4(Colors::White));

			//
			namePos.y += 150.f;
			nrBombsPos.y += 150.f;
		}
	}
}

void HexBomberman::TogglePause()
{
	if(m_SceneContext.settings.isGamePaused)
	{
		AddChild(m_pPauseMenu->GetGameObject());
	}
	else
	{
		RemoveChild(m_pPauseMenu->GetGameObject());
	}
}

void HexBomberman::PlayerDied(PlayerPawn* pPlayer)
{
	Logger::LogDebug(L"Player Died");

	auto it = std::find_if(m_pCharacters.begin(), m_pCharacters.end(), [&](const GameObject* elem) {
		return elem == pPlayer;
		});
	if (it == m_pCharacters.end())
		return;

	RemoveChild(*it, false);
	*it = nullptr;

	CheckVictoryCondition();
}

void HexBomberman::CheckVictoryCondition()
{
	int nrPlayersAlive{ 0 };
	for(const auto& pPlayer : m_pCharacters)
	{
		if(pPlayer != nullptr)
		{
			++nrPlayersAlive;
		}
	}

	if(nrPlayersAlive < 2)
	{
		Logger::LogDebug(L"GAME WON!");
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
	ImGui::SliderFloat2("Position", &m_TextPosition.x, 0, m_SceneContext.windowWidth);
	ImGui::ColorEdit4("Color", &m_TextColor.x, ImGuiColorEditFlags_NoInputs);
	//
	DeferredRenderer::Get()->DrawImGui();
	//
	bool isEnabled = m_pPostBloom->IsEnabled();
	ImGui::Checkbox("Bloom PP", &isEnabled);
	m_pPostBloom->SetIsEnabled(isEnabled);
	//
	//m_pCharacters->DrawImGui();
}