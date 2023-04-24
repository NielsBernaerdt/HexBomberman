#include "stdafx.h"
#include "Crate.h"

#include "ExtraBomb.h"
#include "IncreaseExplosion.h"
#include "Prefabs/CubePrefab.h"

void Crate::Initialize(const SceneContext&)
{
	m_pGameObject->AddChild(new CubePrefab{ 1.f, 1.f, 1.f, XMFLOAT4(Colors::SlateGray) });
}

void Crate::SpawnPowerUp(const BaseComponent* pHexCellParent) const
{
	const float randomPercentage{ rand() % 101 / 100.f };
	if(randomPercentage <= m_SpawnChance)
	{
        GameObject* pPowerUp{ nullptr };
        switch (const int randomPowerUp{ rand() % 2 })
        {
        case 0:
            pPowerUp = pHexCellParent->GetGameObject()->AddChild(new GameObject{});
            pPowerUp->AddComponent(new ExtraBomb{});
            pPowerUp->GetTransform()->Translate(0.f, 0.5f, 0.f);
            break;
        case 1:
        	pPowerUp = pHexCellParent->GetGameObject()->AddChild(new GameObject{});
            pPowerUp->AddComponent(new IncreaseExplosion{});
            pPowerUp->GetTransform()->Translate(0.f, 0.5f, 0.f);
            break;
        default:
            Logger::LogError(L"Crate::SpawnPowerUp - Invalid random number.\n");
            break;
        }
	}
}