#include "stdafx.h"
#include "PostProcessingMaterial.h"

ID3D11InputLayout* PostProcessingMaterial::m_pDefaultInputLayout = {};
ID3D11Buffer* PostProcessingMaterial::m_pDefaultVertexBuffer = {};
UINT PostProcessingMaterial::m_References = {};

PostProcessingMaterial::PostProcessingMaterial(const std::wstring& effectFile):
m_EffectFile(effectFile)
{
	++m_References;
}

PostProcessingMaterial::~PostProcessingMaterial()
{
	SafeDelete(m_pOutputTarget);

	--m_References;
	if(m_References == 0)
	{
		SafeRelease(m_pDefaultInputLayout);
		SafeRelease(m_pDefaultVertexBuffer);
	}
}

void PostProcessingMaterial::InitializeBase(const GameContext& gameContext, UINT materialId)
{
	//Store GameContext & ID
	m_MaterialId = materialId;
	m_GameContext = gameContext;

	//Load Base Effect
	m_pBaseEffect = ContentManager::Load<ID3DX11Effect>(m_EffectFile);
	m_pBaseTechnique = m_pBaseEffect->GetTechniqueByIndex(0);

	//Create Output RenderTarget
	RENDERTARGET_DESC rtDesc{};
	rtDesc.width = gameContext.windowWidth;
	rtDesc.height = gameContext.windowHeight;
	rtDesc.enableColorSRV = true;
	rtDesc.enableColorBuffer = true;

	m_pOutputTarget = new RenderTarget(gameContext.d3dContext);
	m_pOutputTarget->Create(rtDesc);

	//Create Default Vertexbuffer if nullptr
	if (!m_pDefaultVertexBuffer)
	{
		//QUAD vertices
		const VertexPosTex vertices[m_VertexCount]
		{
			{{-1.f,-1.f,0.f},{0.f,1.f}},
			{{-1.f,1.f,0.f},{0.f,0.f}},
			{{1.f,-1.f,0.f},{1.f,1.f}},
			{{1.f,1.f,0.f},{1.f,0.f}}
		};

		//Create VertexBuffer
		D3D11_BUFFER_DESC buffDesc{};
		buffDesc.Usage = D3D11_USAGE_DEFAULT;
		buffDesc.ByteWidth = sizeof(VertexPosTex) * m_VertexCount;
		buffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		buffDesc.CPUAccessFlags = 0;
		buffDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA initData{};
		initData.pSysMem = &vertices[0];

		//Create Buffer
		HANDLE_ERROR(m_GameContext.d3dContext.pDevice->CreateBuffer(&buffDesc, &initData, &m_pDefaultVertexBuffer));
	}

	//Create Default InputLayout if nullptr
	if (!m_pDefaultInputLayout)
	{
		//Assuming the InputLayout for all PostProcessing Effects is the same...
		EffectHelper::BuildInputLayout(m_GameContext.d3dContext.pDevice, m_pBaseTechnique, &m_pDefaultInputLayout);
	}

	//Custom Initialize
	Initialize(gameContext);
}

void PostProcessingMaterial::Draw(const SceneContext& sceneContext, RenderTarget* pSource)
{
	//This is the function you want to override in case you want to implement a more complex post processing effect.
	//Such an effect could include multiple RenderTargets to store intermediate results, multiple passes, multiple effects and techniques, ...

	//Default Implementation > DrawPass with BaseEffect
	UpdateBaseEffectVariables(sceneContext, pSource); //Update Base Effect variables
	DrawPass(sceneContext, m_pBaseTechnique, m_pOutputTarget); //Draw with Base Technique to Output RT

	//Release Source SRV from pipeline
	constexpr ID3D11ShaderResourceView* const pSRV[] = { nullptr };
	sceneContext.d3dContext.pDeviceContext->PSSetShaderResources(0, 1, pSRV);
}

//Default Implementation of Update Effect Variables (assuming there is a gTexture variable)
//This can be overriden in case you have a simple effect with more variables you want to control
void PostProcessingMaterial::UpdateBaseEffectVariables(const SceneContext& /*sceneContext*/, RenderTarget* pSource)
{
	//Set Source Texture
	const auto pSourceSRV = pSource->GetColorShaderResourceView();
	m_pBaseEffect->GetVariableByName("gTexture")->AsShaderResource()->SetResource(pSourceSRV);
	//From this point on, pSource (RenderTarget) is bound as an SRV (ShaderResourceView, read from) to the pipeline
	//In case we want to use pSource as a RTV (RenderTargetView, render to) we have to unbind it first as an SRV
}

void PostProcessingMaterial::DrawPass(const SceneContext& sceneContext, ID3DX11EffectTechnique* pTechnique, RenderTarget* pDestination)
{
	m_GameContext.pGame->SetRenderTarget(pDestination);
	pDestination->Clear(XMFLOAT4{ Colors::Purple });

	sceneContext.d3dContext.pDeviceContext->IASetInputLayout(m_pDefaultInputLayout);
	sceneContext.d3dContext.pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	const UINT stride{ sizeof(VertexPosTex) };
	const UINT offset{ 0 };
	sceneContext.d3dContext.pDeviceContext->IASetVertexBuffers(0, 1, &m_pDefaultVertexBuffer, &stride, &offset);

	D3DX11_TECHNIQUE_DESC techDesc{};
	pTechnique->GetDesc(&techDesc);
	for (UINT i = 0; i < techDesc.Passes; ++i)
	{
		pTechnique->GetPassByIndex(i)->Apply(0, sceneContext.d3dContext.pDeviceContext);
		sceneContext.d3dContext.pDeviceContext->Draw(m_VertexCount, 0);
	}
}