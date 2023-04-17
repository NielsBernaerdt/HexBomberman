#include "stdafx.h"
#include "Character.h"

Character::Character(const CharacterDesc& characterDesc) :
	m_CharacterDesc{ characterDesc },
	m_MoveAcceleration(characterDesc.maxMoveSpeed / characterDesc.moveAccelerationTime),
	m_FallAcceleration(characterDesc.maxFallSpeed / characterDesc.fallAccelerationTime)
{}

void Character::Initialize(const SceneContext& /*sceneContext*/)
{
	//Controller
	m_pControllerComponent = AddComponent(new ControllerComponent(m_CharacterDesc.controller));

	//Camera
	const auto pCamera = AddChild(new FixedCamera());
	m_pCameraComponent = pCamera->GetComponent<CameraComponent>();
	m_pCameraComponent->SetActive(true);

	pCamera->GetTransform()->Translate(0.f, m_CharacterDesc.controller.height * .5f, 0.f);
}

void Character::Update(const SceneContext& sceneContext)
{
	if (m_pCameraComponent->IsActive())
	{
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
		//todo: Optional: if move.y is near zero (abs(move.y) < epsilon), you could use the ThumbStickPosition.y for movement
		if (sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveRight))
		{
			move.x = 1.0f;
		}
		else if (sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveLeft))
		{
			move.x = -1.0f;
		}
		//todo: Optional: if move.x is near zero (abs(move.x) < epsilon), you could use the Left ThumbStickPosition.x for movement

		XMFLOAT2 look{ 0.f, 0.f };
		if (sceneContext.pInput->IsMouseButton(InputState::down, VK_LBUTTON))
		{
			const auto mouseMovement = sceneContext.pInput->GetMouseMovement();
			look.x = static_cast<float>(mouseMovement.x);
			look.y = static_cast<float>(mouseMovement.y);
		}
		//todo: Optional: in case look.x AND look.y are near zero, you could use the Right ThumbStickPosition for look

		//************************
		//GATHERING TRANSFORM INFO
		const TransformComponent* pTransform = GetTransform();
		XMVECTOR forward = XMLoadFloat3(&pTransform->GetForward());
		XMVECTOR right = XMLoadFloat3(&pTransform->GetRight());

		//***************
		//CAMERA ROTATION
		const float elapsedTime{ sceneContext.pGameTime->GetElapsed() };
		m_TotalYaw += look.x * m_CharacterDesc.rotationSpeed * elapsedTime;
		m_TotalPitch += look.y * m_CharacterDesc.rotationSpeed * elapsedTime;
		pTransform->GetTransform()->Rotate(m_TotalPitch, m_TotalYaw, 0.f);

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
		}
		else
		{
			m_MoveSpeed -= m_MoveAcceleration;
			if (m_MoveSpeed < 0)
			{
				m_MoveSpeed = 0;
			}
		}

		XMVECTOR currDir = XMLoadFloat3(&m_CurrentDirection);
		currDir = currDir * m_MoveSpeed;
		XMFLOAT3 velocity{};
		XMStoreFloat3(&velocity, currDir);
		const float velocityY = m_TotalVelocity.y;
		m_TotalVelocity = velocity;
		m_TotalVelocity.y = velocityY;

		//VERTICAL MOVEMENT
		if (!m_pControllerComponent->GetCollisionFlags().isSet(physx::PxControllerCollisionFlag::eCOLLISION_DOWN))
		{
			m_TotalVelocity.y -= m_FallAcceleration * elapsedTime;
			if (m_TotalVelocity.y < -m_CharacterDesc.maxFallSpeed)
			{
				m_TotalVelocity.y = -m_CharacterDesc.maxFallSpeed;
			}
		}
		else if (sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_Jump))
		{
			m_TotalVelocity.y = m_CharacterDesc.JumpSpeed;
		}
		else
		{
			m_TotalVelocity.y = 0;
		}

		//************
		//DISPLACEMENT
		const auto totalVelocity = XMLoadFloat3(&m_TotalVelocity);
		XMFLOAT3 displacement{};
		XMStoreFloat3(&displacement, totalVelocity * sceneContext.pGameTime->GetElapsed());
		m_pControllerComponent->Move(displacement);
	}
}

void Character::DrawImGui()
{
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

		bool isActive = m_pCameraComponent->IsActive();
		if(ImGui::Checkbox("Character Camera", &isActive))
		{
			m_pCameraComponent->SetActive(isActive);
		}
	}
}