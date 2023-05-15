#include "stdafx.h"
#include "ParticleEmitterComponent.h"
#include "Misc/ParticleMaterial.h"

ParticleMaterial* ParticleEmitterComponent::m_pParticleMaterial{};

ParticleEmitterComponent::ParticleEmitterComponent(const std::wstring& assetFile, const ParticleEmitterSettings& emitterSettings, UINT particleCount):
	m_ParticlesArray(new Particle[particleCount]),
	m_ParticleCount(particleCount), //How big is our particle buffer?
	m_MaxParticles(particleCount), //How many particles to draw (max == particleCount)
	m_AssetFile(assetFile),
	m_EmitterSettings(emitterSettings)
{
	m_enablePostDraw = true; //This enables the PostDraw function for the component
}

ParticleEmitterComponent::~ParticleEmitterComponent()
{
	delete[] m_ParticlesArray;
	SafeRelease(m_pVertexBuffer);
}

void ParticleEmitterComponent::Initialize(const SceneContext& sceneContext)
{
	if (m_pParticleMaterial == nullptr)
	{
		m_pParticleMaterial = MaterialManager::Get()->CreateMaterial<ParticleMaterial>();
	}
	CreateVertexBuffer(sceneContext);
	m_pParticleTexture = ContentManager::Load<TextureData>(m_AssetFile);
}

void ParticleEmitterComponent::CreateVertexBuffer(const SceneContext& sceneContext)
{
	if (m_pVertexBuffer)
	{
		SafeRelease(m_pVertexBuffer);
	}
	D3D11_BUFFER_DESC buffDesc;
	buffDesc.Usage = D3D11_USAGE_DYNAMIC;
	buffDesc.ByteWidth = m_ParticleCount * sizeof(VertexParticle);
	buffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buffDesc.MiscFlags = 0;

	const auto hResult{ sceneContext.d3dContext.pDevice->CreateBuffer(&buffDesc, nullptr, &m_pVertexBuffer) };
	HANDLE_ERROR(hResult);
}

void ParticleEmitterComponent::Update(const SceneContext& sceneContext)
{
	const float particleInterval{ ((m_EmitterSettings.maxEnergy + m_EmitterSettings.minEnergy) / 2.0f) / m_ParticleCount };
	const float elapsedSec{ sceneContext.pGameTime->GetElapsed() };
	m_LastParticleSpawn += elapsedSec;
	m_ActiveParticles = 0;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	sceneContext.d3dContext.pDeviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	const auto pBuffer{ static_cast<VertexParticle*>(mappedResource.pData) };

	for (unsigned int i = 0; i < m_ParticleCount; ++i)
	{
		if (m_ParticlesArray[i].isActive)
		{
			UpdateParticle(m_ParticlesArray[i], elapsedSec);
		}
		if (m_ParticlesArray[i].isActive == false && m_LastParticleSpawn >= particleInterval)
		{
			SpawnParticle(m_ParticlesArray[i]);
		}
		if (m_ParticlesArray[i].isActive)
		{
			pBuffer[m_ActiveParticles] = m_ParticlesArray[i].vertexInfo;
			++m_ActiveParticles;
		}
	}
	sceneContext.d3dContext.pDeviceContext->Unmap(m_pVertexBuffer, 0);
}

void ParticleEmitterComponent::UpdateParticle(Particle& particle, float elapsedTime) const
{
	if (!particle.isActive)
	{
		return;
	}
	particle.currentEnergy -= elapsedTime;
	if (particle.currentEnergy <= 0.f)
	{
		particle.isActive = false;
		return;
	}

	particle.vertexInfo.Position.x += m_EmitterSettings.velocity.x * elapsedTime;
	particle.vertexInfo.Position.y += m_EmitterSettings.velocity.y * elapsedTime;
	particle.vertexInfo.Position.z += m_EmitterSettings.velocity.z * elapsedTime;

	const float lifePercent{ particle.currentEnergy / particle.totalEnergy };

	particle.vertexInfo.Color = m_EmitterSettings.color;
	particle.vertexInfo.Color.w = lifePercent;

	particle.vertexInfo.Size = particle.initialSize * (lifePercent + particle.sizeChange * (1 - lifePercent));
}

void ParticleEmitterComponent::SpawnParticle(Particle& particle)
{
	particle.isActive = true;
	particle.totalEnergy = MathHelper::randF(m_EmitterSettings.minEnergy, m_EmitterSettings.maxEnergy);
	particle.currentEnergy = MathHelper::randF(m_EmitterSettings.minEnergy, m_EmitterSettings.maxEnergy);

	const XMFLOAT3 unitFloat3{ XMFLOAT3(1, 0, 0) };
	const auto unitVector{ XMLoadFloat3(&unitFloat3) };
	const auto rotationMatrix{
		XMMatrixRotationRollPitchYaw(MathHelper::randF(-XM_PI, XM_PI),
								 MathHelper::randF(-XM_PI, XM_PI),
								MathHelper::randF(-XM_PI, XM_PI))
	};
	const auto directionVector{ XMVector3TransformNormal(unitVector, rotationMatrix) };
	XMFLOAT3 dirFloat3;
	XMStoreFloat3(&dirFloat3, directionVector);

	const float distance{ MathHelper::randF(m_EmitterSettings.minEmitterRadius, m_EmitterSettings.maxEmitterRadius) };
	particle.vertexInfo.Position.x = GetTransform()->GetPosition().x + dirFloat3.x * distance;
	particle.vertexInfo.Position.y = GetTransform()->GetPosition().y + dirFloat3.y * distance;
	particle.vertexInfo.Position.z = GetTransform()->GetPosition().z + dirFloat3.z * distance;

	particle.vertexInfo.Size = MathHelper::randF(m_EmitterSettings.minSize, m_EmitterSettings.maxSize);
	particle.initialSize = MathHelper::randF(m_EmitterSettings.minSize, m_EmitterSettings.maxSize);
	particle.sizeChange = MathHelper::randF(m_EmitterSettings.minScale, m_EmitterSettings.maxScale);
	particle.vertexInfo.Rotation = MathHelper::randF(-XM_PI, XM_PI);
	particle.vertexInfo.Color = m_EmitterSettings.color;
}

void ParticleEmitterComponent::PostDraw(const SceneContext& sceneContext)
{
	m_pParticleMaterial->SetVariable_Matrix(L"gWorldViewProj", sceneContext.pCamera->GetViewProjection());
	m_pParticleMaterial->SetVariable_Matrix(L"gViewInverse", sceneContext.pCamera->GetViewInverse());
	m_pParticleMaterial->SetVariable_Texture(L"gParticleTexture", m_pParticleTexture);

	const auto context{ m_pParticleMaterial->GetTechniqueContext() };
	sceneContext.d3dContext.pDeviceContext->IASetInputLayout(context.pInputLayout);
	sceneContext.d3dContext.pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	const UINT stride{ sizeof(VertexParticle) };
	const UINT offset{ 0 };
	sceneContext.d3dContext.pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	D3DX11_TECHNIQUE_DESC techDesc;
	context.pTechnique->GetDesc(&techDesc);
	for (UINT i = 0; i < techDesc.Passes; ++i)
	{
		context.pTechnique->GetPassByIndex(i)->Apply(0, sceneContext.d3dContext.pDeviceContext);
		sceneContext.d3dContext.pDeviceContext->Draw(m_ActiveParticles, 0);
	}
}

void ParticleEmitterComponent::DrawImGui()
{
	if(ImGui::CollapsingHeader("Particle System"))
	{
		ImGui::SliderUInt("Count", &m_ParticleCount, 0, m_MaxParticles);
		ImGui::InputFloatRange("Energy Bounds", &m_EmitterSettings.minEnergy, &m_EmitterSettings.maxEnergy);
		ImGui::InputFloatRange("Size Bounds", &m_EmitterSettings.minSize, &m_EmitterSettings.maxSize);
		ImGui::InputFloatRange("Scale Bounds", &m_EmitterSettings.minScale, &m_EmitterSettings.maxScale);
		ImGui::InputFloatRange("Radius Bounds", &m_EmitterSettings.minEmitterRadius, &m_EmitterSettings.maxEmitterRadius);
		ImGui::InputFloat3("Velocity", &m_EmitterSettings.velocity.x);
		ImGui::ColorEdit4("Color", &m_EmitterSettings.color.x, ImGuiColorEditFlags_NoInputs);
	}
}