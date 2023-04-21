#include "stdafx.h"
#include "ExtraBomb.h"

#include "Prefabs/CubePrefab.h"

void ExtraBomb::Initialize(const SceneContext&)
{
	m_pGameObject->AddChild(new CubePrefab{ 1.f, 1.f, 1.f, XMFLOAT4(Colors::Black) });
}
