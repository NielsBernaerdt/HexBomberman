#include "stdafx.h"
#include "PlayerPawn.h"

#include "HexBomberman/HexGrid/HexCell.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow_Skinned.h"

PlayerPawn::PlayerPawn(const CharacterDesc& characterDesc) :
	m_CharacterDesc{ characterDesc },
	m_MoveAcceleration(characterDesc.maxMoveSpeed / characterDesc.moveAccelerationTime),
	m_FallAcceleration(characterDesc.maxFallSpeed / characterDesc.fallAccelerationTime)
{}

void PlayerPawn::Initialize(const SceneContext& /*sceneContext*/)
{
	//Controller
	m_pControllerComponent = AddComponent(new ControllerComponent(m_CharacterDesc.controller));

	//Animations
	const auto pBomberMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow_Skinned>();
	pBomberMaterial->SetDiffuseTexture(L"Textures/Bomberman_Diffuse" + std::to_wstring(m_CharacterDesc.playerIdx) + L".png");

	const auto pObject = AddChild(new GameObject);
	const auto pModel = pObject->AddComponent(new ModelComponent(L"Meshes/Bomberman.ovm"));
	pModel->SetMaterial(pBomberMaterial);

	pObject->GetTransform()->Scale(0.008f);
	pObject->GetTransform()->Translate(0.f, -1.f, 0.f);

	pAnimator = pModel->GetAnimator();
	pAnimator->SetAnimation(0);
	pAnimator->SetAnimationSpeed(m_AnimationSpeed);
	pAnimator->Play();
}

void PlayerPawn::SetCurrentTile(HexCell* pHexCell)
{
	m_pCurrentCell = pHexCell;
	if(pHexCell->HasPowerUp() == true)
	{
		pHexCell->CollectPowerUp(this);
	}
}

void PlayerPawn::IncreaseBlastRange()
{
	++m_BlastRange;
}

void PlayerPawn::IncreaseNrBombs()
{
	++m_NrBombs;
}

void PlayerPawn::BombExploded()
{
	--m_NrBombsInPlay;
}

void PlayerPawn::PlaceBomb()
{
	if (m_pCurrentCell
		&& m_NrBombsInPlay < m_NrBombs)
	{
		++m_NrBombsInPlay;
		m_pCurrentCell->PlaceBomb(this, m_BlastRange);
	}
}

void PlayerPawn::Update(const SceneContext& sceneContext)
{
	if (sceneContext.settings.isGamePaused)
		return;

	constexpr float epsilon{ 0.01f }; //Constant that can be used to compare if a float is near zero

	//***************
	//HANDLE INPUT
	XMFLOAT2 move{};
	if (sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveForward))
	{
		move.y = 1.0f;
	}
	else if (sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveBackward))
	{
		move.y = -1.0f;
	}
	if (abs(move.y) <= epsilon)
	{
		move.y = sceneContext.pInput->GetThumbstickPosition(true, static_cast<GamepadIndex>(m_CharacterDesc.playerIdx)).y;
	}
	if (sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveRight))
	{
		move.x = 1.0f;
	}
	else if (sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveLeft))
	{
		move.x = -1.0f;
	}
	if (abs(move.x) <= epsilon)
	{
		move.x = sceneContext.pInput->GetThumbstickPosition(true, static_cast<GamepadIndex>(m_CharacterDesc.playerIdx)).x;
	}

	if(sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_PlaceBomb))
	{
		PlaceBomb();
	}

	//************************
	//GATHERING TRANSFORM INFO
	const TransformComponent* pTransform = GetTransform();
	XMVECTOR forward = XMLoadFloat3(&pTransform->GetForward());
	XMVECTOR right = XMLoadFloat3(&pTransform->GetRight());

	//***************
	//CAMERA ROTATION
	const float elapsedTime{ sceneContext.pGameTime->GetElapsed() };

	//********
	//MOVEMENT
	//HORIZONTAL VELOCITY
	if (abs(move.x) >= epsilon
		|| abs(move.y) >= epsilon)
	{
		XMVECTOR currentDirection{};
		currentDirection += forward * move.y;
		currentDirection += right * move.x;
		currentDirection = XMVector3Normalize(currentDirection);
		XMStoreFloat3(&m_CurrentDirection, currentDirection);

		const float currentAcceleration = m_MoveAcceleration * elapsedTime;
		m_MoveSpeed += currentAcceleration;
		if (m_MoveSpeed > m_CharacterDesc.maxMoveSpeed)
		{
			m_MoveSpeed = m_CharacterDesc.maxMoveSpeed;
		}

		//Update Animation
		if (m_CurrentAnimationIdx != 1)
		{
			pAnimator->SetAnimation(1); // 1 = running
			m_CurrentAnimationIdx = 1;
		}
	}
	else
	{
		m_MoveSpeed -= m_MoveAcceleration;
		if (m_MoveSpeed < 0)
		{
			m_MoveSpeed = 0;
		}

		//Update Animation
		if (m_CurrentAnimationIdx != 0)
		{
			pAnimator->SetAnimation(0); // 0 = idle
			m_CurrentAnimationIdx = 0;
		}
	}

	XMVECTOR currDir = XMLoadFloat3(&m_CurrentDirection);
	currDir = currDir * m_MoveSpeed;
	XMFLOAT3 velocity{};
	XMStoreFloat3(&velocity, currDir);
	m_TotalVelocity = velocity;

	//************
	//DISPLACEMENT
	const auto totalVelocity = XMLoadFloat3(&m_TotalVelocity);
	XMFLOAT3 displacement{};
	XMStoreFloat3(&displacement, totalVelocity * sceneContext.pGameTime->GetElapsed());
	m_pControllerComponent->Move(displacement);

	//************
	//ORIENTATION OF MODELCOMPONENT OBJECT
	auto angleFromDisplacement{ -atan2(displacement.z, displacement.x) - XM_PIDIV2 };
	GetChild<GameObject>()->GetTransform()->Rotate(0, angleFromDisplacement, 0, false);
}

void PlayerPawn::DrawImGui()
{
	//CharacterComponent
	if (ImGui::CollapsingHeader("Character"))
	{
		ImGui::Text(std::format("Move Speed: {:0.1f} m/s", m_MoveSpeed).c_str());
		ImGui::Text(std::format("Fall Speed: {:0.1f} m/s", m_TotalVelocity.y).c_str());

		ImGui::Text(std::format("Move Acceleration: {:0.1f} m/s2", m_MoveAcceleration).c_str());
		ImGui::Text(std::format("Fall Acceleration: {:0.1f} m/s2", m_FallAcceleration).c_str());

		const float jumpMaxTime = m_CharacterDesc.JumpSpeed / m_FallAcceleration;
		const float jumpMaxHeight = (m_CharacterDesc.JumpSpeed * jumpMaxTime) - (0.5f * (m_FallAcceleration * powf(jumpMaxTime, 2)));
		ImGui::Text(std::format("Jump Height: {:0.1f} m", jumpMaxHeight).c_str());

		ImGui::Dummy({ 0.f,5.f });
		if (ImGui::DragFloat("Max Move Speed (m/s)", &m_CharacterDesc.maxMoveSpeed, 0.1f, 0.f, 0.f, "%.1f") ||
			ImGui::DragFloat("Move Acceleration Time (s)", &m_CharacterDesc.moveAccelerationTime, 0.1f, 0.f, 0.f, "%.1f"))
		{
			m_MoveAcceleration = m_CharacterDesc.maxMoveSpeed / m_CharacterDesc.moveAccelerationTime;
		}

		ImGui::Dummy({ 0.f,5.f });
		if (ImGui::DragFloat("Max Fall Speed (m/s)", &m_CharacterDesc.maxFallSpeed, 0.1f, 0.f, 0.f, "%.1f") ||
			ImGui::DragFloat("Fall Acceleration Time (s)", &m_CharacterDesc.fallAccelerationTime, 0.1f, 0.f, 0.f, "%.1f"))
		{
			m_FallAcceleration = m_CharacterDesc.maxFallSpeed / m_CharacterDesc.fallAccelerationTime;
		}

		ImGui::Dummy({ 0.f,5.f });
		ImGui::DragFloat("Jump Speed", &m_CharacterDesc.JumpSpeed, 0.1f, 0.f, 0.f, "%.1f");
		ImGui::DragFloat("Rotation Speed (deg/s)", &m_CharacterDesc.rotationSpeed, 0.1f, 0.f, 0.f, "%.1f");
	}
}