#include "stdafx.h"
#include "ShadowMapRenderer.h"
#include "Misc/ShadowMapMaterial.h"

ShadowMapRenderer::~ShadowMapRenderer()
{
	SafeDelete(m_pShadowRenderTarget)
}

void ShadowMapRenderer::Initialize()
{
	//1. Create a separate RenderTarget (see RenderTarget class), store in m_pShadowRenderTarget
	m_pShadowRenderTarget = new RenderTarget(m_GameContext.d3dContext);
	RENDERTARGET_DESC desc;
	desc.enableColorBuffer = false;
	desc.enableDepthSRV = true;
	desc.width = m_GameContext.windowWidth;
	desc.height = m_GameContext.windowHeight;
	m_pShadowRenderTarget->Create(desc);

	//2. Create a new ShadowMapMaterial, this will be the material that 'generated' the ShadowMap, store in m_pShadowMapGenerator
	m_pShadowMapGenerator = MaterialManager::Get()->CreateMaterial<ShadowMapMaterial>();
	m_GeneratorTechniqueContexts[(int)ShadowGeneratorType::Static] = m_pShadowMapGenerator->GetTechniqueContext((int)ShadowGeneratorType::Static);
	m_GeneratorTechniqueContexts[(int)ShadowGeneratorType::Skinned] = m_pShadowMapGenerator->GetTechniqueContext((int)ShadowGeneratorType::Skinned);
}

void ShadowMapRenderer::UpdateMeshFilter(const SceneContext& sceneContext, MeshFilter* pMeshFilter) const
{
	//1. Figure out the correct ShadowGeneratorType (either Static, or Skinned) with information from the incoming MeshFilter
	bool isMeshSkinned{ pMeshFilter->HasAnimations() };
	//2. Retrieve the corresponding TechniqueContext from m_GeneratorTechniqueContexts array (Static/Skinned)
	MaterialTechniqueContext techniqueContext;
	if (isMeshSkinned)
	{
		techniqueContext = m_GeneratorTechniqueContexts[(int)ShadowGeneratorType::Skinned];
	}
	else
	{
		techniqueContext = m_GeneratorTechniqueContexts[(int)ShadowGeneratorType::Static];
	}

	//3. Build a corresponding VertexBuffer with data from the relevant TechniqueContext you retrieved in Step2 (MeshFilter::BuildVertexBuffer)
	pMeshFilter->BuildVertexBuffer(sceneContext, techniqueContext.inputLayoutID, techniqueContext.inputLayoutSize, techniqueContext.pInputLayoutDescriptions);
}

void ShadowMapRenderer::Begin(const SceneContext& sceneContext)
{
	//1. Making sure that the ShadowMap is unbound from the pipeline as ShaderResourceView (SRV) is important
	constexpr ID3D11ShaderResourceView* const pSRV[] = { nullptr };
	sceneContext.d3dContext.pDeviceContext->PSSetShaderResources(1, 1, pSRV);

	//2. Calculate the Light ViewProjection and store in m_LightVP
	// - Use XMMatrixOrtographicLH to create Projection Matrix (constants used for the demo below - feel free to change)
	//		*viewWidth> 100.f * aspectRatio
	//		*viewHeight> 100.f
	//		*nearZ>0.1f
	//		*farZ>500.f
	//- Use XMMatrixLookAtLH to create a View Matrix
	//		*eyePosition: Position of the Direction Light (SceneContext::pLights > Retrieve Directional Light)
	//		*focusPosition: Calculate using the Direction Light position and direction
	//- Use the Projection & View Matrix to calculate the ViewProjection of this Light, store in m_LightVP
	const float nearZ{ 0.1f };
	const float farZ{ 500.0f };

	const float shadowScale{ 1.f };
	const float viewWidth { shadowScale * 100.0f * (m_GameContext.windowWidth / m_GameContext.windowHeight) };
	const float viewHeight{ shadowScale * 100.0f };
	const XMMATRIX projection{ XMMatrixOrthographicLH(viewWidth, viewHeight, nearZ, farZ) };

	const XMVECTOR pos{ XMLoadFloat4(&sceneContext.pLights->GetDirectionalLight().position) };
	const XMVECTOR dir{ XMLoadFloat4(&sceneContext.pLights->GetDirectionalLight().direction) };
	const XMFLOAT3 up { 0,1,0 };
	const XMVECTOR upXM{ XMLoadFloat3(&up) };
	const XMMATRIX view{ XMMatrixLookAtLH(pos, pos + dir, upXM) };

	const XMMATRIX viewProj{ view * projection };
	XMStoreFloat4x4(&m_LightVP, viewProj);

	//3. Update this matrix (m_LightVP) on the ShadowMapMaterial effect
	m_pShadowMapGenerator->SetVariable_Matrix(L"gLightViewProj", m_LightVP);

	//4. Set the Main Game RenderTarget to m_pShadowRenderTarget (OverlordGame::SetRenderTarget) - Hint: every Singleton object has access to the GameContext...
	m_GameContext.pGame->SetRenderTarget(m_pShadowRenderTarget);

	//5. Clear the ShadowMap rendertarget (RenderTarget::Clear)
	m_pShadowRenderTarget->Clear();
}

void ShadowMapRenderer::DrawMesh(const SceneContext& sceneContext, MeshFilter* pMeshFilter, const XMFLOAT4X4& meshWorld, const std::vector<XMFLOAT4X4>& meshBones)
{
	bool isMeshSkinned{ pMeshFilter->HasAnimations() };
	if (isMeshSkinned)
	{
		m_pShadowMapGenerator->SetTechnique((int)ShadowGeneratorType::Skinned);
	}
	else
	{
		m_pShadowMapGenerator->SetTechnique((int)ShadowGeneratorType::Static);
	}

	//2. Retrieve the correct TechniqueContext for m_GeneratorTechniqueContexts
	MaterialTechniqueContext techniqueContext;
	if (isMeshSkinned)
	{
		techniqueContext = m_GeneratorTechniqueContexts[(int)ShadowGeneratorType::Skinned];
	}
	else
	{
		techniqueContext = m_GeneratorTechniqueContexts[(int)ShadowGeneratorType::Static];
	}

	//3. Set the relevant variables on the ShadowMapMaterial
	m_pShadowMapGenerator->SetVariable_Matrix(L"gWorld", meshWorld);
	if (isMeshSkinned)
	{
		m_pShadowMapGenerator->SetVariable_MatrixArray(L"gBones", &meshBones[0]._11, (UINT)meshBones.size());
	}

	//4. Setup Pipeline for Drawing (Similar to ModelComponent::Draw, but for our ShadowMapMaterial)
	sceneContext.d3dContext.pDeviceContext->IASetInputLayout(m_pShadowMapGenerator->GetTechniqueContext().pInputLayout);
	sceneContext.d3dContext.pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	for (const auto& subMesh : pMeshFilter->GetMeshes())
	{
		//Vertex Buffer
		const UINT offset{ 0 };
		const auto& vertexBufferData = pMeshFilter->GetVertexBufferData(sceneContext, m_pShadowMapGenerator, subMesh.id);
		sceneContext.d3dContext.pDeviceContext->IASetVertexBuffers(0, 1, &vertexBufferData.pVertexBuffer, &vertexBufferData.VertexStride, &offset);
		//Index Buffer
		sceneContext.d3dContext.pDeviceContext->IASetIndexBuffer(subMesh.buffers.pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		//Perform Draw Call (same as usual, iterate Technique Passes, Apply, Draw - See ModelComponent::Draw for reference)
		auto effectTechnique{ m_pShadowMapGenerator->GetTechniqueContext().pTechnique };
		D3DX11_TECHNIQUE_DESC techniqueDesc{};
		effectTechnique->GetDesc(&techniqueDesc);
		for (UINT p = 0; p < techniqueDesc.Passes; ++p)
		{
			effectTechnique->GetPassByIndex(p)->Apply(0, sceneContext.d3dContext.pDeviceContext);
			sceneContext.d3dContext.pDeviceContext->DrawIndexed(subMesh.indexCount, 0, 0);
		}
	}
}

void ShadowMapRenderer::End(const SceneContext&) const
{
	m_GameContext.pGame->SetRenderTarget(nullptr);
}

ID3D11ShaderResourceView* ShadowMapRenderer::GetShadowMap() const
{
	return m_pShadowRenderTarget->GetDepthShaderResourceView();
}

void ShadowMapRenderer::Debug_DrawDepthSRV(const XMFLOAT2& position, const XMFLOAT2& scale, const XMFLOAT2& pivot) const
{
	if (m_pShadowRenderTarget->HasDepthSRV())
	{
		SpriteRenderer::Get()->DrawImmediate(m_GameContext.d3dContext, m_pShadowRenderTarget->GetDepthShaderResourceView(), position, XMFLOAT4{ Colors::White }, pivot, scale);

		//Remove from Pipeline
		constexpr ID3D11ShaderResourceView* const pSRV[] = { nullptr };
		m_GameContext.d3dContext.pDeviceContext->PSSetShaderResources(0, 1, pSRV);
	}
}
