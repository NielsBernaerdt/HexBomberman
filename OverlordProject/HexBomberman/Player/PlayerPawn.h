#pragma once

struct CharacterDesc
{
	CharacterDesc(
		PxMaterial* pMaterial,
		float radius = .5f,
		float height = 2.f)
	{
		controller.setToDefault();
		controller.radius = radius;
		controller.height = height;
		controller.material = pMaterial;
	}

	float maxMoveSpeed{ 15.f };
	float maxFallSpeed{ 15.f };

	float JumpSpeed{ 15.f };

	float moveAccelerationTime{ .3f };
	float fallAccelerationTime{ .3f };

	PxCapsuleControllerDesc controller{};

	float rotationSpeed{ 60.f };

	int actionId_MoveLeft{ -1 };
	int actionId_MoveRight{ -1 };
	int actionId_MoveForward{ -1 };
	int actionId_MoveBackward{ -1 };
	int actionId_PlaceBomb{ -1 };
};

class HexCell;

class PlayerPawn : public GameObject
{
public:
	PlayerPawn(const CharacterDesc& characterDesc);
	~PlayerPawn() override = default;

	PlayerPawn(const PlayerPawn& other) = delete;
	PlayerPawn(PlayerPawn&& other) noexcept = delete;
	PlayerPawn& operator=(const PlayerPawn& other) = delete;
	PlayerPawn& operator=(PlayerPawn&& other) noexcept = delete;

	void SetCurrentTile(HexCell* pHexCell);
	void IncreaseBlastRange();
	void IncreaseNrBombs();
	void BombExploded();
	void DrawImGui();

protected:
	void Initialize(const SceneContext&) override;
	void Update(const SceneContext&) override;

private:
	ControllerComponent* m_pControllerComponent{};
	HexCell* m_pCurrentCell{ nullptr };

	CharacterDesc m_CharacterDesc;
	float m_TotalPitch{}, m_TotalYaw{};				//Total camera Pitch(X) and Yaw(Y) rotation
	float m_MoveAcceleration{},						//Acceleration required to reach maxMoveVelocity after 1 second (maxMoveVelocity / moveAccelerationTime)
		m_FallAcceleration{},						//Acceleration required to reach maxFallVelocity after 1 second (maxFallVelocity / fallAccelerationTime)
		m_MoveSpeed{};								//MoveSpeed > Horizontal Velocity = MoveDirection * MoveVelocity (= TotalVelocity.xz)

	XMFLOAT3 m_TotalVelocity{};						//TotalVelocity with X/Z for Horizontal Movement AND Y for Vertical Movement (fall/jump)
	XMFLOAT3 m_CurrentDirection{};					//Current/Last Direction based on Camera forward/right (Stored for deacceleration)

	void PlaceBomb();

	int m_BlastRange{ 1 };
	int m_NrBombs{ 1 };
	int m_NrBombsInPlay{ 0 };
};