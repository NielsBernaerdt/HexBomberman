#include "stdafx.h"
#include "ModelAnimator.h"

ModelAnimator::ModelAnimator(MeshFilter* pMeshFilter):
	m_pMeshFilter{pMeshFilter}
{
	SetAnimation(0);
}

void ModelAnimator::Update(const SceneContext& sceneContext)
{
	//We only update the transforms if the animation is running and the clip is set
	if (m_IsPlaying && m_ClipSet)
	{
		//1.
		auto passedTicks = sceneContext.pGameTime->GetElapsed() * m_CurrentClip.ticksPerSecond * m_AnimationSpeed;
		passedTicks = std::max(std::min(passedTicks, m_CurrentClip.duration), 0.0f);

		//2.
		if (m_Reversed)
		{
			m_TickCount -= passedTicks;
			if (m_TickCount < 0)
			{
				m_TickCount += m_CurrentClip.duration;
			}
		}
		else
		{
			m_TickCount += passedTicks;
			if (m_TickCount > m_CurrentClip.duration)
			{
				m_TickCount -= m_CurrentClip.duration;
			}
		}

		//3.
		AnimationKey keyA, keyB;
		for (size_t i{}; i < m_CurrentClip.keys.size(); ++i) 
		{
			if (m_CurrentClip.keys[i].tick > m_TickCount) 
			{
				keyB = m_CurrentClip.keys[i];
				if (i == 0) keyA = m_CurrentClip.keys.back();
				else keyA = m_CurrentClip.keys[i - 1];
				break;
			}
		}

		//4.
		const float blendFactor = (m_TickCount - keyA.tick) / (keyB.tick - keyA.tick);
		m_Transforms.clear();
		for (auto i = 0; i < m_pMeshFilter->m_BoneCount; ++i)
		{
			const auto transformA = keyA.boneTransforms[i];
			const XMMATRIX transformAMat = XMLoadFloat4x4(&transformA);
			const auto transformB = keyB.boneTransforms[i];
			const XMMATRIX transformBMat = XMLoadFloat4x4(&transformB);

			//	Decompose transforms
			XMVECTOR scaleA, rotA, transA;
			XMMatrixDecompose(&scaleA, &rotA, &transA, transformAMat);
			XMVECTOR scaleB, rotB, transB;
			XMMatrixDecompose(&scaleB, &rotB, &transB, transformBMat);

			//	Lerp between all transformations
			const auto lerpTransform = XMVectorLerp(transA, transB, blendFactor);
			const auto lerpScale = XMVectorLerp(scaleA, scaleB, blendFactor);
			const auto lerpRotation = XMQuaternionSlerp(rotA, rotB, blendFactor);

			//	Compose transformation with lerp results
			const XMMATRIX transformMatrix = XMMatrixTranslationFromVector(lerpTransform);
			const XMMATRIX scaleMatrix = XMMatrixScalingFromVector(lerpScale);
			const XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(lerpRotation);

			//	Add the resulting matrix to the m_Transforms vector
			XMFLOAT4X4 resultingMatrix;
			XMStoreFloat4x4(&resultingMatrix, scaleMatrix * rotationMatrix * transformMatrix);
			m_Transforms.push_back(resultingMatrix);
		}
	}
}

void ModelAnimator::SetAnimation(const std::wstring& clipName)
{
	m_ClipSet = false;

	bool foundClip{ false };
	for (int i{}; i < m_pMeshFilter->m_AnimationClips.size(); ++i)
	{
		auto animationClip = m_pMeshFilter->m_AnimationClips[i];
		if (animationClip.name == clipName)
		{
			foundClip = true;
			SetAnimation(animationClip);
			break;
		}
	}
	if (foundClip == false)
	{
		Reset();
		Logger::LogWarning(L"ModelAnimator::SetAnimation clipName {} not found", clipName);
	}
}

void ModelAnimator::SetAnimation(UINT clipNumber)
{
	m_ClipSet = false;
	if (clipNumber < m_pMeshFilter->m_AnimationClips.size())
	{
		SetAnimation(m_pMeshFilter->m_AnimationClips[clipNumber]);
	}
	else
	{
		Reset();
		Logger::LogWarning(L"ModelAnimator::SetAnimation current clipnumber {} is not smaller than m_AnimationClips vector size", clipNumber);
	}
}

void ModelAnimator::SetAnimation(const AnimationClip& clip)
{
	m_ClipSet = true;
	m_CurrentClip = clip;
	Reset(false);
}

void ModelAnimator::Reset(bool pause)
{
	if (pause) m_IsPlaying = false;

	m_TickCount = 0;
	m_AnimationSpeed = 1.0f;

	if (m_ClipSet)
	{
		m_Transforms.assign(m_CurrentClip.keys[0].boneTransforms.begin(), m_CurrentClip.keys[0].boneTransforms.end());
	}
	else
	{
		XMFLOAT4X4 matrixIdentity;
		XMStoreFloat4x4(&matrixIdentity, XMMatrixIdentity());
		m_Transforms.assign(m_pMeshFilter->m_BoneCount, matrixIdentity);
	}
}