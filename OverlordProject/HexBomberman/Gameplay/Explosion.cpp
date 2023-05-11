#include "stdafx.h"
#include "Explosion.h"

#include "Prefabs/CubePrefab.h"
#include "HexBomberman/HexGrid/HexCell.h"

Explosion::Explosion(HexCell* pOwnerTile)
	: m_pOwnerTile(pOwnerTile)
{
}


void Explosion::Initialize(const SceneContext&)
{
	m_pGameObject->AddChild(new CubePrefab{ 1.f, 1.f, 1.f, XMFLOAT4(Colors::OrangeRed) });
}