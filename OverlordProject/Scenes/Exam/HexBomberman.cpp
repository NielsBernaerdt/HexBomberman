#include "stdafx.h"
#include "HexBomberman.h"

#include "HexBomberman/HexGrid/HexGrid.h"
#include "HexBomberman/HexGrid/HexCell.h"
#include "HexBomberman/Player/PlayerPawn.h"
#include "Materials/Post/PostBloom.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow.h"

HexBomberman::~HexBomberman()
{
	if (m_ShowControllerScheme == false)
		AddChild(m_pControllerScheme->GetGameObject());
	if(m_ShowVictoryScreen == false)
		AddChild(m_pVictoryMenu->GetGameObject());
	if(m_SceneContext.settings.isGamePaused == false)
		AddChild(m_pPauseMenu->GetGameObject());
}

void HexBomberman::Initialize()
{
	//Scene Settings
	m_SceneContext.settings.showInfoOverlay = false;
	m_SceneContext.settings.enableOnGUI = true;
	m_SceneContext.settings.drawPhysXDebug = false;
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.useDeferredRendering = true;

	//Camera
	m_SceneContext.pCamera->GetTransform()->Translate(6.165973f, 17.315273f, -4.472385f);
	dynamic_cast<FreeCamera*>(m_SceneContext.pCamera->GetGameObject())->SetRotation(60.f, 0.f);

	//Lights
	m_SceneContext.pLights->SetDirectionalLight({ -95.6139526f,66.1346436f,-41.1850471f }, { 0.740129888f, -0.597205281f, 0.309117377f });

	//Materials
	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.f, 0.f, 1.f);

	//Fonts
	m_pFont = ContentManager::Load<SpriteFont>(L"SpriteFonts/Consolas_32.fnt");

	//Ground Actor
	GameSceneExt::CreatePhysXGroundPlane(*this, pDefaultMaterial);

	//Ground Object
	const auto pGroundMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pGroundMaterial->SetDiffuseTexture(L"Textures/GroundBrick.jpg");
	const auto pGroundObj = AddChild(new GameObject());
	const auto pGroundModel = pGroundObj->AddComponent(new ModelComponent(L"Meshes/UnitPlane.ovm"));
	pGroundModel->SetMaterial(pGroundMaterial);
	pGroundObj->GetTransform()->Scale(5.0f, 1.0f, 5.0f);
	pGroundObj->GetTransform()->Translate(5.f, -0.01f, 2.f);

	//Hexagonal Grid
	m_pHexGrid = AddChild(new HexGrid{});

	//Players
	const int nrPlayers{ 4 };
	XMFLOAT2 hudPos{ 30.f, 100.f };
	for (int i{}; i < nrPlayers; ++i)
	{
		//Character
		CharacterDesc characterDesc{ pDefaultMaterial };
		characterDesc.actionId_MoveForward = CharacterMoveForward + (10 * i);
		characterDesc.actionId_MoveBackward = CharacterMoveBackward + (10 * i);
		characterDesc.actionId_MoveLeft = CharacterMoveLeft + (10 * i);
		characterDesc.actionId_MoveRight = CharacterMoveRight + (10 * i);
		characterDesc.actionId_PlaceBomb = CharacterPlaceBomb + (10 * i);
		characterDesc.playerIdx = i;

		auto pCharacter = AddChild(new PlayerPawn(characterDesc));
		m_pPlayers.push_back(pCharacter);
		pCharacter->GetTransform()->Translate(m_pPlayerStartPositions[i].x, 1.f, m_pPlayerStartPositions[i].y);

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

		//Set Trigger Callback functions for hexcells
		auto cellCallBack = [=](GameObject* pTriggerObject, GameObject* pOtherObject, PxTriggerAction triggerAction)
		{
			auto it = std::find_if(m_pPlayers.begin(), m_pPlayers.end(), [&](const GameObject* elem) {
				return elem == pOtherObject;
				});

			if (it != m_pPlayers.end() && triggerAction == PxTriggerAction::ENTER)
			{
				dynamic_cast<PlayerPawn*>(pOtherObject)->SetCurrentTile(pTriggerObject->GetComponent<HexCell>());
			}
		};
		for (const auto& cell : m_pHexGrid->GetGrid())
		{
			cell->GetGameObject()->SetOnTriggerCallBack(cellCallBack);
		}

		//HUD
		const auto pHUD = AddChild(new GameObject);
		
		const auto pPlayerHUD = pHUD->AddChild(new GameObject);
		pPlayerHUD->GetTransform()->Translate(hudPos.x, hudPos.y, 0.f);

		const auto pPortrait = pPlayerHUD->AddChild(new GameObject);
		pPortrait->AddComponent(new SpriteComponent(L"Textures/UI_Bomberman" + std::to_wstring(i) + L".png"));
		pPortrait->GetTransform()->Translate(0.f, 80.f, 0.f);
		pPortrait->GetTransform()->Scale(0.8f, 0.8f, 1.f);

		const auto pBomb = pPlayerHUD->AddChild(new GameObject);
		pBomb->AddComponent(new SpriteComponent(L"Textures/UI_BombIcon.png"));
		pBomb->GetTransform()->Translate(140.f, 130.f, 0.f);

		//Update position for next players' HUD
		hudPos.y += 150.f;
	}
	
	//Particle System
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

	//Post Processing
	m_pPostBloom = MaterialManager::Get()->CreateMaterial<PostBloom>();
	AddPostProcessingEffect(m_pPostBloom);

	//Controller Scheme
	const auto pControllerBackground = new GameObject{};
	m_pControllerScheme = pControllerBackground->AddComponent(new SpriteComponent(L"Textures/ControlScheme.png"));
	if (m_ShowControllerScheme)
	{
		AddChild(pControllerBackground);
		m_pPostBloom->SetIsEnabled(false);
	}

	//Pause Menu
	auto inputAction = InputAction(PauseGame, InputState::released, -1, -1, XINPUT_GAMEPAD_START, GamepadIndex::playerOne);
	m_SceneContext.pInput->AddInputAction(inputAction);
	inputAction = InputAction(PauseGame, InputState::released, -1, -1, XINPUT_GAMEPAD_START, GamepadIndex::playerTwo);
	m_SceneContext.pInput->AddInputAction(inputAction);
	inputAction = InputAction(PauseGame, InputState::released, -1, -1, XINPUT_GAMEPAD_START, GamepadIndex::playerThree);
	m_SceneContext.pInput->AddInputAction(inputAction);
	inputAction = InputAction(PauseGame, InputState::released, -1, -1, XINPUT_GAMEPAD_START, GamepadIndex::playerFour);
	m_SceneContext.pInput->AddInputAction(inputAction);

	const auto pBackground = new GameObject{};
	m_pPauseMenu = pBackground->AddComponent(new SpriteComponent(L"Textures/BackgroundPause.png"));

	const auto pResumeButton = pBackground->AddChild(new GameObject);
	m_pResume = pResumeButton->AddComponent(new SpriteComponent(L"Textures/Button_Resume.png"));
	m_pResume->GetTransform()->Translate(240.f, 700.f, 0.f);

	const auto pRestartButton = pBackground->AddChild(new GameObject);
	m_pRestart = pRestartButton->AddComponent(new SpriteComponent(L"Textures/Button_Restart.png"));
	m_pRestart->GetTransform()->Translate(730.f, 700.f, 0.f);

	const auto pExitButton = pBackground->AddChild(new GameObject);
	m_pExit = pExitButton->AddComponent(new SpriteComponent(L"Textures/Button_Exit.png"));
	m_pExit->GetTransform()->Translate(1230.f, 700.f, 0.f);

	//Victory menu
	const auto pBackgroundVictory = new GameObject{};
	m_pVictoryMenu = pBackgroundVictory->AddComponent(new SpriteComponent(L"Textures/VictoryBackground.png"));

	const auto pRestartButtonVictoryMenu = pBackgroundVictory->AddChild(new GameObject);
	m_pRestartVictoryMenu = pRestartButtonVictoryMenu->AddComponent(new SpriteComponent(L"Textures/Button_Restart.png"));
	m_pRestartVictoryMenu->GetTransform()->Translate(730.f, 700.f, 0.f);
}

void HexBomberman::ClearPlayerStartingArea()
{
	if (m_IsPlayerStartingAreaCleared == true)
		return;

	//todo: change when players are dependant on nr of controllers
	const auto& pCellOne = m_pPlayers[0]->GetCurrentCell();
	const auto& pCellTwo = m_pPlayers[1]->GetCurrentCell();
	if( pCellOne && pCellTwo && pCellOne == pCellTwo)
		return;

	//For every players' current cell (+ remove the crate of this cell)
	//For every neighbor of that cell (+ remove  the crate of this cell)
	//For 1 neighbor of those neighbors (remove the crate of this cell)

	for (const auto& pPlayer : m_pPlayers)
	{
		if (const auto& pPlayerCell = pPlayer->GetCurrentCell())
		{
			m_IsPlayerStartingAreaCleared = true;
			if (pPlayerCell->HasCrate()) pPlayerCell->DestroyCrate(false);

			for (const auto& pNeighbors : pPlayerCell->GetNeighbors())
			{
				if (pNeighbors == nullptr)
					continue;
				if (pNeighbors->HasCrate()) pNeighbors->DestroyCrate(false);

				for (const auto& pNeighborsNeighbor : pNeighbors->GetNeighbors())
				{
					if (pNeighborsNeighbor == nullptr)
						continue;

					if (pNeighborsNeighbor->HasCrate())
					{
						pNeighborsNeighbor->DestroyCrate(false);
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
	if (ManageControllerScheme()) return;

	ManageControllerVibration();

	ManagerPlayerDeaths();

	ClearPlayerStartingArea();

	ManageInput();

	UpdateTextSprites();
}

void HexBomberman::TogglePause()
{
	m_SceneContext.settings.isGamePaused = !m_SceneContext.settings.isGamePaused;
	if(m_SceneContext.settings.isGamePaused)
	{
		AddChild(m_pPauseMenu->GetGameObject());
		m_pPostBloom->SetIsEnabled(false);
	}
	else
	{
		RemoveChild(m_pPauseMenu->GetGameObject());
		m_pPostBloom->SetIsEnabled(true);
	}
}

void HexBomberman::PlayerDied(PlayerPawn* pPlayerToDie)
{
	for(int i{}; i < m_pPlayers.size(); ++i)
	{
		if(m_pPlayers[i] == pPlayerToDie)
		{
			m_pPlayersToDie.push_back(pPlayerToDie);
			m_pPlayers[i] = nullptr;
			InputManager::SetVibration(true, true, static_cast<GamepadIndex>(i));
			m_IsVibrating = true;
		}
	}
}

void HexBomberman::CheckVictoryCondition()
{
	int nrPlayersAlive{ 0 };
	for(const auto& pPlayer : m_pPlayers)
	{
		if(pPlayer != nullptr)
		{
			++nrPlayersAlive;
		}
	}

	if(nrPlayersAlive < 2)
	{
		AddChild(m_pVictoryMenu->GetGameObject());
		m_ShowVictoryScreen = true;
		m_pPostBloom->SetIsEnabled(false);
	}
}

bool HexBomberman::IsOverlapping(const SpriteComponent* pSpriteComponent) const
{
	const auto posMouse{ InputManager::GetMousePosition() };
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
}

bool HexBomberman::ManageControllerScheme()
{
	if (m_ShowControllerScheme)
	{
		m_AccControllerScheme += m_SceneContext.pGameTime->GetElapsed();
		if (m_AccControllerScheme >= 2.f)
		{
			m_ShowControllerScheme = false;
			RemoveChild(m_pControllerScheme->GetGameObject());
			m_pPostBloom->SetIsEnabled(true);
			return true;
		}
	}
	return false;
}

void HexBomberman::ManageControllerVibration()
{
	if (m_IsVibrating)
	{
		m_AccVibTime += m_SceneContext.pGameTime->GetElapsed();
		if (m_AccVibTime >= m_MaxVibTime)
		{
			m_AccVibTime = 0.f;
			for (int i{}; i < 4; ++i)
			{
				InputManager::SetVibration(false, false, static_cast<GamepadIndex>(i));
				m_IsVibrating = false;
			}
		}
	}
}

void HexBomberman::ManagerPlayerDeaths()
{
	if (m_pPlayersToDie.size() >= 1)
	{
		for (const auto& pPlayer : m_pPlayersToDie)
		{
			RemoveChild(pPlayer, true);
		}
		CheckVictoryCondition();
		m_pPlayersToDie = std::vector<PlayerPawn*>{};
	}
}

void HexBomberman::ManageInput()
{
	if (InputManager::IsGamepadButton(InputState::pressed, XINPUT_GAMEPAD_START, GamepadIndex::playerOne)
		|| InputManager::IsGamepadButton(InputState::pressed, XINPUT_GAMEPAD_START, GamepadIndex::playerTwo)
		|| InputManager::IsGamepadButton(InputState::pressed, XINPUT_GAMEPAD_START, GamepadIndex::playerThree)
		|| InputManager::IsGamepadButton(InputState::pressed, XINPUT_GAMEPAD_START, GamepadIndex::playerFour))
	{
		TogglePause();
	}

	if (m_ShowVictoryScreen)
	{
		if ((InputManager::IsMouseButton(InputState::pressed, VK_LBUTTON) && IsOverlapping(m_pRestartVictoryMenu))
			|| InputManager::IsGamepadButton(InputState::pressed, XINPUT_GAMEPAD_X, GamepadIndex::playerOne)
			|| InputManager::IsGamepadButton(InputState::pressed, XINPUT_GAMEPAD_X, GamepadIndex::playerTwo)
			|| InputManager::IsGamepadButton(InputState::pressed, XINPUT_GAMEPAD_X, GamepadIndex::playerThree)
			|| InputManager::IsGamepadButton(InputState::pressed, XINPUT_GAMEPAD_X, GamepadIndex::playerFour))
		{
			SceneManager::Get()->AddGameScene(new HexBomberman());
			SceneManager::Get()->NextScene();
		}
	}

	if (m_SceneContext.settings.isGamePaused == true)
	{
		if ((InputManager::IsMouseButton(InputState::pressed, VK_LBUTTON) && IsOverlapping(m_pResume))
			|| InputManager::IsGamepadButton(InputState::pressed, XINPUT_GAMEPAD_A, GamepadIndex::playerOne)
			|| InputManager::IsGamepadButton(InputState::pressed, XINPUT_GAMEPAD_A, GamepadIndex::playerTwo)
			|| InputManager::IsGamepadButton(InputState::pressed, XINPUT_GAMEPAD_A, GamepadIndex::playerThree)
			|| InputManager::IsGamepadButton(InputState::pressed, XINPUT_GAMEPAD_A, GamepadIndex::playerFour))
		{
			TogglePause();
		}
		else if ((InputManager::IsMouseButton(InputState::pressed, VK_LBUTTON) && IsOverlapping(m_pRestart))
			|| InputManager::IsGamepadButton(InputState::pressed, XINPUT_GAMEPAD_X, GamepadIndex::playerOne)
			|| InputManager::IsGamepadButton(InputState::pressed, XINPUT_GAMEPAD_X, GamepadIndex::playerTwo)
			|| InputManager::IsGamepadButton(InputState::pressed, XINPUT_GAMEPAD_X, GamepadIndex::playerThree)
			|| InputManager::IsGamepadButton(InputState::pressed, XINPUT_GAMEPAD_X, GamepadIndex::playerFour))
		{
			SceneManager::Get()->AddGameScene(new HexBomberman());
			SceneManager::Get()->NextScene();
		}
		else if ((InputManager::IsMouseButton(InputState::pressed, VK_LBUTTON) && IsOverlapping(m_pExit))
			|| InputManager::IsGamepadButton(InputState::pressed, XINPUT_GAMEPAD_B, GamepadIndex::playerOne)
			|| InputManager::IsGamepadButton(InputState::pressed, XINPUT_GAMEPAD_B, GamepadIndex::playerTwo)
			|| InputManager::IsGamepadButton(InputState::pressed, XINPUT_GAMEPAD_B, GamepadIndex::playerThree)
			|| InputManager::IsGamepadButton(InputState::pressed, XINPUT_GAMEPAD_B, GamepadIndex::playerFour))
		{
			PostQuitMessage(0);
		}
	}
}

void HexBomberman::UpdateTextSprites() const
{
	if (m_SceneContext.settings.isGamePaused == false
		&& m_ShowVictoryScreen == false
		&& m_ShowControllerScheme == false)
	{
		XMFLOAT2 nrBombsPos{ m_TextPosition };
		XMFLOAT2 namePos{ 160.f, 190.f };
		for (int i{}; i < m_pPlayers.size(); ++i)
		{
			std::string text{};
			if (m_pPlayers[i] != nullptr)
				text = std::to_string((m_pPlayers[i]->GetNrBombs() - m_pPlayers[i]->GetNrBombsInPlay())) + "/" + std::to_string(m_pPlayers[i]->GetNrBombs());
			else
				text = "0/0";
			//Draw Nr Bombs available
			TextRenderer::Get()->DrawText(m_pFont, StringUtil::utf8_decode(text), nrBombsPos, m_TextColor);
			//Draw Player Name
			TextRenderer::Get()->DrawText(m_pFont, StringUtil::utf8_decode("Player " + std::to_string(i + 1)), namePos, XMFLOAT4(Colors::White));

			namePos.y += 150.f;
			nrBombsPos.y += 150.f;
		}
	}
}