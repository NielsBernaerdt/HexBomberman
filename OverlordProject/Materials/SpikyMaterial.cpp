#include "stdafx.h"
#include "SpikyMaterial.h"

SpikyMaterial::~SpikyMaterial()
{
	delete m_pSpikeLength;
	delete[] m_pColorDiffuse;
}

void SpikyMaterial::OnGUI() const
{
	if (ImGui::InputFloat("gSpikeLength", m_pSpikeLength, 0.01f, 0.1f))
	{
		SetVariable_Scalar(L"gSpikeLength", *m_pSpikeLength);
	}
	if (ImGui::ColorEdit3("gColorDiffuse", m_pColorDiffuse))
	{
		SetVariable_Vector(L"gColorDiffuse", XMFLOAT3(m_pColorDiffuse[0], m_pColorDiffuse[1], m_pColorDiffuse[2]));
	}
}

void SpikyMaterial::InitializeEffectVariables()
{
	SetVariable_Scalar(L"gSpikeLength", *m_pSpikeLength);
	SetVariable_Vector(L"gColorDiffuse", XMFLOAT3(m_pColorDiffuse[0], m_pColorDiffuse[1], m_pColorDiffuse[2]));
}