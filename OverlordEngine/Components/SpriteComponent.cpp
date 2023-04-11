#include "stdafx.h"
#include "SpriteComponent.h"

SpriteComponent::SpriteComponent(const std::wstring& spriteAsset, const XMFLOAT2& pivot, const XMFLOAT4& color):
	m_SpriteAsset(spriteAsset),
	m_Pivot(pivot),
	m_Color(color)
{}

void SpriteComponent::Initialize(const SceneContext& /*sceneContext*/)
{
	m_pTexture = ContentManager::Load<TextureData>(m_SpriteAsset);
}

void SpriteComponent::SetTexture(const std::wstring& spriteAsset)
{
	m_SpriteAsset = spriteAsset;
	m_pTexture = ContentManager::Load<TextureData>(m_SpriteAsset);
}

void SpriteComponent::Draw(const SceneContext& /*sceneContext*/)
{
	if (!m_pTexture)
		return;

	const XMFLOAT2 pos{ GetTransform()->GetWorldPosition().x, GetTransform()->GetWorldPosition().y };
	const XMFLOAT2 scale{ GetTransform()->GetScale().x, GetTransform()->GetScale().y };
	const float rotZ = MathHelper::QuaternionToEuler(GetTransform()->GetWorldRotation()).z;

	SpriteRenderer::Get()->AppendSprite(m_pTexture, pos, m_Color, m_Pivot, scale, rotZ, GetTransform()->GetWorldPosition().z);
}