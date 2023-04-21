#include "stdafx.h"
#include "Explosion.h"

#include "Prefabs/CubePrefab.h"

void Explosion::Initialize(const SceneContext&)
{
	m_pGameObject->AddChild(new CubePrefab{ 1.f, 1.f, 1.f, XMFLOAT4(Colors::OrangeRed) });
}