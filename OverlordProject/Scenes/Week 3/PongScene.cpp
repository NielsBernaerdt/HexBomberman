#include "stdafx.h"
#include "PongScene.h"

#include "Prefabs/SpherePrefab.h"
#include "Prefabs/CubePrefab.h"


void PongScene::Initialize()
{
	//Settings
	m_SceneContext.settings.drawPhysXDebug = false;
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.settings.enableOnGUI = true;
	m_SceneContext.settings.clearColor = XMFLOAT4(0, 0, 0, 1);

	//Materials
	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.f, 0.f, 1.f);

	//Ground Plane
	GameSceneExt::CreatePhysXGroundPlane(*this, pDefaultMaterial);

	//Camera
	const auto pCamera = AddChild(new FixedCamera());
	pCamera->GetTransform()->Rotate(0, 0, 0);
	pCamera->GetTransform()->Translate(0, 10, -20);
	SetActiveCamera(pCamera->GetComponent<CameraComponent>());

	//Ball
	m_pBall = AddChild(new SpherePrefab(.6f, 10, XMFLOAT4{ Colors::Red }));
	auto pRigidBodyComponent = m_pBall->AddComponent(new RigidBodyComponent());
	pRigidBodyComponent->AddCollider(PxSphereGeometry(.5f), *pDefaultMaterial);
	pRigidBodyComponent->SetConstraint(RigidBodyConstraint::TransZ, false);
	pRigidBodyComponent->GetPxRigidActor()->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);

	//Peddles
	m_pPeddleLeft = AddChild(new CubePrefab(.5f, 4, 1, XMFLOAT4{ Colors::White }));
	pRigidBodyComponent = m_pPeddleLeft->AddComponent(new RigidBodyComponent(true));
	pRigidBodyComponent->AddCollider(PxBoxGeometry(.25f, 2.f, .5f), *pDefaultMaterial);

	m_pPeddleRight = AddChild(new CubePrefab(.5f, 4, 1, XMFLOAT4{ Colors::White }));
	pRigidBodyComponent = m_pPeddleRight->AddComponent(new RigidBodyComponent(true));
	pRigidBodyComponent->AddCollider(PxBoxGeometry(.25f, 2.f, .5f), *pDefaultMaterial);

	//Borders
	m_pUpperBoundary = AddChild(new CubePrefab(30, 2, 2, XMFLOAT4{ Colors::Transparent }));
	pRigidBodyComponent = m_pUpperBoundary->AddComponent(new RigidBodyComponent(true));
	m_pUpperBoundary->GetTransform()->Translate(0, 18, 0);
	pRigidBodyComponent->AddCollider(PxBoxGeometry(15, 1, 1), *pDefaultMaterial);

	m_pLowerBoundary = AddChild(new CubePrefab(30, 2, 2, XMFLOAT4{ Colors::Transparent }));
	pRigidBodyComponent = m_pLowerBoundary->AddComponent(new RigidBodyComponent(true));
	m_pLowerBoundary->GetTransform()->Translate(0, 1, 0);
	pRigidBodyComponent->AddCollider(PxBoxGeometry(15, 1, 1), *pDefaultMaterial);

	// Triggers
	m_pTriggerLeft = AddChild(new CubePrefab(2, 30, 2, XMFLOAT4{ Colors::Transparent }));
	pRigidBodyComponent = m_pTriggerLeft->AddComponent(new RigidBodyComponent(true));
	m_pTriggerLeft->GetTransform()->Translate(-18, 10, 0);
	pRigidBodyComponent->AddCollider(PxBoxGeometry(1, 15, 1), *pDefaultMaterial, true);
	
	m_pTriggerRight = AddChild(new CubePrefab(2, 30, 2, XMFLOAT4{ Colors::Transparent }));
	pRigidBodyComponent = m_pTriggerRight->AddComponent(new RigidBodyComponent(true));
	m_pTriggerRight->GetTransform()->Translate(18, 10, 0);
	pRigidBodyComponent->AddCollider(PxBoxGeometry(1, 15, 1), *pDefaultMaterial, true);

	//Set Trigger Callback functions
	auto callback = [=](GameObject* /*pTriggerObject*/, GameObject* pOtherObject, PxTriggerAction triggerAction)
	{
		if (m_pBall == pOtherObject && triggerAction == PxTriggerAction::ENTER)
		{
			Reset();
		}
	};
	m_pTriggerLeft->SetOnTriggerCallBack(callback);
	m_pTriggerRight->SetOnTriggerCallBack(callback);

	//Set GameObjects To Correct Transforms;
	Reset();
}

void PongScene::Update()
{
	//Input
	const auto pInput = m_SceneContext.pInput;
	const float deltaTime = m_SceneContext.pGameTime->GetElapsed();
	const float movementSpeed = 10.0f;
	const float displacement = movementSpeed * deltaTime;

	if (pInput->IsKeyboardKey(InputState::down, 'W'))
	{
		const XMFLOAT3 currPos = m_pPeddleLeft->GetTransform()->GetPosition();
		if (currPos.y <= 15.17f)
		{
			m_pPeddleLeft->GetTransform()->Translate(currPos.x, currPos.y + displacement, currPos.z);
		}
	}
	if (pInput->IsKeyboardKey(InputState::down, 'S'))
	{
		const XMFLOAT3 currPos = m_pPeddleLeft->GetTransform()->GetPosition();
		if (currPos.y >= 4.17f)
		{
			m_pPeddleLeft->GetTransform()->Translate(currPos.x, currPos.y - displacement, currPos.z);
		}
	}
	if (pInput->IsKeyboardKey(InputState::down, VK_UP))
	{
		const XMFLOAT3 currPos = m_pPeddleRight->GetTransform()->GetPosition();
		if (currPos.y <= 15.17f)
		{
			m_pPeddleRight->GetTransform()->Translate(currPos.x, currPos.y + displacement, currPos.z);
		}
	}
	if (pInput->IsKeyboardKey(InputState::down, VK_DOWN))
	{
		const XMFLOAT3 currPos = m_pPeddleRight->GetTransform()->GetPosition();
		if (currPos.y >= 4.17f)
		{
			m_pPeddleRight->GetTransform()->Translate(currPos.x, currPos.y - displacement, currPos.z);
		}
	}
	if (pInput->IsKeyboardKey(InputState::pressed, VK_SPACE))
	{
		if (!m_IsStarted) Start();
	}
	if (pInput->IsKeyboardKey(InputState::down, 'R'))
	{
		Reset();
	}
}

void PongScene::Start()
{
	const XMFLOAT3 force{ 10,10,0 };
	m_pBall->GetComponent<RigidBodyComponent>()->AddForce(force, PxForceMode::eIMPULSE);
	m_IsStarted = true;
}

void PongScene::Reset()
{
	m_pBall->GetTransform()->Translate(0, 10, 0);
	m_pBall->GetTransform()->Rotate(0, 0, 0);

	m_pPeddleLeft->GetTransform()->Translate(-10, 10, 0);
	m_pPeddleRight->GetTransform()->Translate(10, 10, 0);

	m_IsStarted = false;
}

void PongScene::OnGUI()
{
	ImGui::Text("START game: \t\t\t  'SPACE'");
	ImGui::Text("Move LEFT Peddle:\t 'W' & 'S'");
	ImGui::Text("Move RIGHT Peddle:\t'UP' & 'DOWN'");
	ImGui::Text("RESET scene: \t\t\t  'R'");
}