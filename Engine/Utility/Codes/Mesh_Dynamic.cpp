#include "..\Headers\Mesh_Dynamic.h"
#include "Hierarchy_Loader.h"
#include "Animation_Ctrl.h"
#include "Graphic_Device.h"
#include <stack>
#include <time.h>

USING(Engine)

CMesh_Dynamic::CMesh_Dynamic(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMesh(pGraphicDev)
	, m_pLoader(nullptr)
	, m_pFrame(nullptr)
	, m_pCloneFrame(nullptr)
	, m_pAnimation_Ctrl(nullptr)
	, m_dwNumMeshContainer(0)	
{
#ifndef _FINAL
	Tag(L"CMesh_Dynamic", std::bind(&CMesh_Dynamic::Debug_Render, this));
#endif

}

CMesh_Dynamic::CMesh_Dynamic(const CMesh_Dynamic & rhs)
	: CMesh(rhs)
	, m_pLoader(rhs.m_pLoader)
	, m_pFrame(rhs.m_pFrame)
	, m_pCloneFrame(D3DXFRAME_DERIVED::Create(rhs.m_pFrame))
	, m_dwNumMeshContainer(rhs.m_dwNumMeshContainer)
	, m_vecMeshContainer(rhs.m_vecMeshContainer)
	, m_pAnimation_Ctrl(CAnimation_Ctrl::Create(*rhs.m_pAnimation_Ctrl))
	, m_vecStartAngle(rhs.m_vecStartAngle)
	, m_matStartMatrix(rhs.m_matStartMatrix)
{
#ifndef _FINAL
	Tag(L"CMesh_Dynamic", std::bind(&CMesh_Dynamic::Debug_Render, this));
#endif

}

const _vec3 * CMesh_Dynamic::Get_Min(const _uint & iMeshContainerIdx)
{
	if (iMeshContainerIdx >= m_vecMeshContainer.size())
		return nullptr;

	return &m_vecMeshContainer[iMeshContainerIdx]->vMin;
}

const _vec3 * CMesh_Dynamic::Get_Max(const _uint & iMeshContainerIdx)
{
	if (iMeshContainerIdx >= m_vecMeshContainer.size())
		return nullptr;

	return &m_vecMeshContainer[iMeshContainerIdx]->vMax;
}

void CMesh_Dynamic::Update_Component(const _float & fTimeDelta)
{
	if (m_bInit)
	{
		m_bInit = false;
	}
	//Play_Animation(fTimeDelta);

	CMesh::Update_Component(fTimeDelta);
}

void CMesh_Dynamic::LastUpdate_Component(const _float & fTimeDelta)
{
	CMesh::LastUpdate_Component(fTimeDelta);
}

_bool CMesh_Dynamic::is_Finish_Animation(double fEndDelay)
{
	if (nullptr == m_pAnimation_Ctrl)
		return false;

	return m_pAnimation_Ctrl->is_Finish_Animation(fEndDelay);
}

int CMesh_Dynamic::is_Finish_Animation()
{
	if (nullptr == m_pAnimation_Ctrl)
		return false;

	return m_pAnimation_Ctrl->is_Finish_Animation();
}

HRESULT CMesh_Dynamic::Ready_Mesh(const _tchar * pFilePath, const _tchar * pFileName, _vec3 vecStartAngle)
{
	_tchar			szFullPath[128] = L"";

	lstrcpy(szFullPath, pFilePath);
	lstrcat(szFullPath, pFileName);

	m_pLoader = CHierarchy_Loader::Create(m_pGraphicDev, pFilePath);
	if (nullptr == m_pLoader)
		return E_FAIL;

	LPD3DXANIMATIONCONTROLLER			pAniCtrl = nullptr;

	// 에이치바를 올리기위한 행렬 * 플레이어의 행렬
	if (FAILED(D3DXLoadMeshHierarchyFromX(szFullPath, D3DXMESH_MANAGED, m_pGraphicDev, m_pLoader, nullptr,
		(D3DXFRAME**)&m_pFrame, &pAniCtrl)))
		return E_FAIL;

	
	szFullPath[lstrlen(szFullPath)-1] = _tchar('\0');
	lstrcat(szFullPath, L"Anidata");

	m_pAnimation_Ctrl = CAnimation_Ctrl::Create(m_pGraphicDev, pAniCtrl, szFullPath);
	if (nullptr == m_pAnimation_Ctrl)
		return E_FAIL;

	Safe_Release(pAniCtrl);

	Compute_NumMeshContainer(m_pFrame);

	m_vecStartAngle = vecStartAngle;
	D3DXMatrixRotationYawPitchRoll(&m_matStartMatrix, D3DXToRadian(m_vecStartAngle.y), D3DXToRadian(m_vecStartAngle.x), D3DXToRadian(m_vecStartAngle.z));
	
	Update_CombinedTransformaitionMatrix(m_pFrame, &m_matStartMatrix);

	m_vecMeshContainer.reserve(m_dwNumMeshContainer);

	if (FAILED(SetUp_BoneMatrixPointer(m_pFrame)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CMesh_Dynamic::Set_AnimationSet(const _uint & iAni_Index)
{
	if (nullptr == m_pAnimation_Ctrl)
		return E_FAIL;

	if (FAILED(m_pAnimation_Ctrl->Set_AnimationSet(iAni_Index)))
	{

	}
	else // 새로운 애니메이션 세팅이 성공한다면
	{
		if (m_pRootMotionFrame != nullptr) // 루트 모션 뼈가 존재한다면
		{
			m_matPreAniLocal = m_pRootMotionFrame->matCombinedTransformationMatrix;
			m_matCurAniLocal = m_pRootMotionFrame->matCombinedTransformationMatrix;

			m_vecDeltaPosition_Local = _vec3(0.f, 0.f, 0.f);
			m_vecDeltaAngle_Local = _vec3(0.f, 0.f, 0.f);

			m_bInit = true;
			m_iPreLoopCount = m_iCurLoopCount = 0;
			m_fInitDelay = m_pAnimation_Ctrl->Get_AniLerp(); // 보간 길이 반환
			//m_fInitDelay = m_pAnimation_Ctrl->Get_CurAnimationInfo();
		}
	}

	return NOERROR;
}

bool CMesh_Dynamic::Play_Animation(const _float & fTimeDelta)
{
	if (nullptr == m_pAnimation_Ctrl
		//|| nullptr == m_pRootMotionFrame
		)
		return false;

	//if (m_pCloneFrame != nullptr)
	//{
	//	D3DXFRAME_DERIVED::Clone_Frame_Hierachy(m_pFrame, m_pCloneFrame);
	//}

	m_pAnimation_Ctrl->Play_Animation(fTimeDelta);
	Update_CombinedTransformaitionMatrix(m_pFrame, &m_matStartMatrix);

	m_iCurLoopCount = m_pAnimation_Ctrl->is_Finish_Animation();

	if (m_iCurLoopCount == m_iPreLoopCount && m_fInitDelay < SMALL_NUMBER)
	{
		if (m_fInitDelay < SMALL_NUMBER)
		{
			m_matCurAniLocal = m_pRootMotionFrame->matCombinedTransformationMatrix;

			m_vecDeltaPosition_Local = *(_vec3*)m_matCurAniLocal.m[3] - *(_vec3*)m_matPreAniLocal.m[3];

			D3DXQUATERNION deltaQtRot,curQtRot, preQtRot;
			D3DXQuaternionRotationMatrix(&curQtRot, &m_matCurAniLocal);
			D3DXQuaternionRotationMatrix(&preQtRot, &m_matPreAniLocal);

			m_vecDeltaAngle_Local = JWMATH::QuaternionsToEuler_Degree(curQtRot) - JWMATH::QuaternionsToEuler_Degree(preQtRot);

			m_matPreAniLocal = m_matCurAniLocal;

		}
	}
	else
	{
		if (m_pRootMotionFrame != nullptr) // 루트 모션 뼈가 존재한다면
		{
			m_matPreAniLocal = m_pRootMotionFrame->matCombinedTransformationMatrix;
			m_matCurAniLocal = m_pRootMotionFrame->matCombinedTransformationMatrix;

			m_vecDeltaPosition_Local = _vec3(0.f, 0.f, 0.f);
			m_vecDeltaAngle_Local = _vec3(0.f, 0.f, 0.f);

			if (m_iCurLoopCount != m_iPreLoopCount)
			{
				m_iPreLoopCount = m_iCurLoopCount;
				m_fInitDelay = m_pAnimation_Ctrl->Get_AniLerp() + fTimeDelta; 
				//m_fInitDelay = 0.25f+ fTimeDelta;
			}
			m_bInit = true;
		}
	}

	if (m_pCloneFrame != nullptr)
	{
		D3DXFRAME_DERIVED::Clone_Frame_Hierachy(m_pFrame, m_pCloneFrame);
	}
	
	m_fInitDelay -= fTimeDelta;
	return true;
}

const ANI_STATUS & CMesh_Dynamic::Get_CurAnimationInfo() const
{
	return m_pAnimation_Ctrl->Get_CurAnimationInfo();
}

void CMesh_Dynamic::Render_Mesh(void)
{
	//m_pAnimation_Ctrl->SetUp_Animation();
	//m_pAnimation_Ctrl->Play_Animation(0.0f);
	//Update_CombinedTransformaitionMatrix(m_pFrame, &m_matStartMatrix);
	D3DXFRAME_DERIVED::Clone_Frame_Hierachy(m_pCloneFrame, m_pFrame);
	//D3DXFRAME_DERIVED::Clone_Frame_Hierachy(m_pFrame, m_pCloneFrame);

	for (auto& pMeshContainer : m_vecMeshContainer)
	{
		for (size_t i = 0; i < pMeshContainer->dwNumBones; i++)
		{
			pMeshContainer->pRenderingMatrices[i] = pMeshContainer->pOffsetMatrices[i] * *pMeshContainer->ppCombinedTransformationMatrices[i];
		}
		
		void*		pSrcVertex = nullptr, *pDestVertex = nullptr;
		pMeshContainer->pOriginal_Mesh->LockVertexBuffer(0, &pSrcVertex);
		pMeshContainer->MeshData.pMesh->LockVertexBuffer(0, &pDestVertex);

		// 메시를 이루는 정점을 뼈대들의 정보만큼 변환한다 + 스키닝 ㅐ
		pMeshContainer->pSkinInfo->UpdateSkinnedMesh(pMeshContainer->pRenderingMatrices, nullptr, pSrcVertex, pDestVertex);

		pMeshContainer->pOriginal_Mesh->UnlockVertexBuffer();
		pMeshContainer->MeshData.pMesh->UnlockVertexBuffer();

		// 변환된 정점들을 그린다.
		for (_ulong i = 0; i < pMeshContainer->NumMaterials; i++)
		{			
			//m_pGraphicDev->SetMaterial(&pMeshContainer->pMaterials[i].MatD3D);
			m_pGraphicDev->SetTexture(0, pMeshContainer->pMeshTexture[i].pTexture[TEXTUREDATA::TEX_DIFFUSE]);
			pMeshContainer->MeshData.pMesh->DrawSubset(i);
		}		
	}		
}

void CMesh_Dynamic::Render_Mesh(LPD3DXEFFECT pEffect, const char* pConstant_DiffuseTex_Name, const char* pConstant_NormalTex_Name, const char* pConstant_SpecularTex_Name)
{
	//m_pAnimation_Ctrl->SetUp_Animation();
	//m_pAnimation_Ctrl->Play_Animation(0.0f);
	//Update_CombinedTransformaitionMatrix(m_pFrame, &m_matStartMatrix);
	D3DXFRAME_DERIVED::Clone_Frame_Hierachy(m_pCloneFrame, m_pFrame);
	//D3DXFRAME_DERIVED::Clone_Frame_Hierachy(m_pFrame, m_pCloneFrame);

	_uint iPassIdx = 0;

	for (auto& pMeshContainer : m_vecMeshContainer)
	{
		for (size_t i = 0; i < pMeshContainer->dwNumBones; i++)
		{
			pMeshContainer->pRenderingMatrices[i] = pMeshContainer->pOffsetMatrices[i] * *pMeshContainer->ppCombinedTransformationMatrices[i];
		}

		void*		pSrcVertex = nullptr, *pDestVertex = nullptr;
		pMeshContainer->pOriginal_Mesh->LockVertexBuffer(0, &pSrcVertex);
		pMeshContainer->MeshData.pMesh->LockVertexBuffer(0, &pDestVertex);

		// 메시를 이루는 정점을 뼈대들의 정보만큼 변환한다 + 스키닝 ㅐ
		pMeshContainer->pSkinInfo->UpdateSkinnedMesh(pMeshContainer->pRenderingMatrices, nullptr, pSrcVertex, pDestVertex);

		pMeshContainer->pOriginal_Mesh->UnlockVertexBuffer();
		pMeshContainer->MeshData.pMesh->UnlockVertexBuffer();



		// 변환된 정점들을 그린다.
		for (_ulong i = 0; i < pMeshContainer->NumMaterials; i++)
		{
			//m_pGraphicDev->SetMaterial(&pMeshContainer->pMaterials[i].MatD3D);
			//m_pGraphicDev->SetTexture(0, pMeshContainer->pMeshTexture[i].pTexture[TEXTUREDATA::TEX_DIFFUSE]);
			iPassIdx = 0;
			
			if ( nullptr != pMeshContainer->pMeshTexture[i].pTexture[TEXTUREDATA::TEX_DIFFUSE] )
				pEffect->SetTexture( pConstant_DiffuseTex_Name, pMeshContainer->pMeshTexture[i].pTexture[TEXTUREDATA::TEX_DIFFUSE] );
			else
				continue;
			if ( nullptr != pMeshContainer->pMeshTexture[i].pTexture[TEXTUREDATA::TEX_NORMAL] )
			{
				iPassIdx |= 0x1;
				pEffect->SetTexture( pConstant_NormalTex_Name, pMeshContainer->pMeshTexture[i].pTexture[TEXTUREDATA::TEX_NORMAL] );
			}
			if ( nullptr != pMeshContainer->pMeshTexture[i].pTexture[TEXTUREDATA::TEX_SPECULAR] )
			{
				iPassIdx |= 0x2;
				pEffect->SetTexture( pConstant_SpecularTex_Name, pMeshContainer->pMeshTexture[i].pTexture[TEXTUREDATA::TEX_SPECULAR] );
			}

			pEffect->CommitChanges();
			pEffect->BeginPass( iPassIdx );

			pMeshContainer->MeshData.pMesh->DrawSubset(i);

			pEffect->EndPass();
		}
	}
}


D3DXTRACK_DESC * CMesh_Dynamic::Get_TrackInfo()
{
	if (m_pAnimation_Ctrl == nullptr) 
		return nullptr;
	return m_pAnimation_Ctrl->Get_TrackInfo();
}

_double CMesh_Dynamic::Get_Period()
{

	return m_pAnimation_Ctrl->Get_Period();
	
}

_double* CMesh_Dynamic::Get_Pointer_Period()
{
	return m_pAnimation_Ctrl->Get_Pointer_Period();
}

void CMesh_Dynamic::Set_Speed(float fSpeed)
{
	m_pAnimation_Ctrl->Set_Speed(fSpeed);
}

float CMesh_Dynamic::Get_Speed()
{
	return m_pAnimation_Ctrl->Get_Speed();
}

HRESULT CMesh_Dynamic::Compute_NumMeshContainer(D3DXFRAME_DERIVED * pFrame)
{
	if (nullptr != pFrame->pMeshContainer)
		++m_dwNumMeshContainer;

	if (nullptr != pFrame->pFrameFirstChild)
	{
		Compute_NumMeshContainer((D3DXFRAME_DERIVED*)pFrame->pFrameFirstChild);
	}

	if (nullptr != pFrame->pFrameSibling)
	{
		Compute_NumMeshContainer((D3DXFRAME_DERIVED*)pFrame->pFrameSibling);
	}
	return NOERROR;
}
//
//HRESULT CMesh_Dynamic::Update_CombinedTransformaitionMatrix(D3DXFRAME_DERIVED* pFrame, const _matrix* pParentCombinedMatrix)
//{
//	// 전위 표기법 비재귀 함수
//	stack<BONESTACK> stack;
//	BONESTACK tmp;
//
//	stack.push(BONESTACK(pFrame, pParentCombinedMatrix));
//
//	while (!stack.empty()) 
//	{
//		tmp = stack.top();
//		stack.pop();
//
//		tmp.pFrame->matCombinedTransformationMatrix = tmp.pFrame->TransformationMatrix * *tmp.pParentCombinedMatrix;             
//		if (nullptr != tmp.pFrame->pFrameFirstChild)
//			stack.push(BONESTACK((D3DXFRAME_DERIVED*)tmp.pFrame->pFrameFirstChild, &tmp.pFrame->matCombinedTransformationMatrix));      
//		if (nullptr != tmp.pFrame->pFrameSibling)
//			stack.push(BONESTACK((D3DXFRAME_DERIVED*)tmp.pFrame->pFrameSibling, tmp.pParentCombinedMatrix));       
//	}
//
//	return NOERROR;
//}


HRESULT CMesh_Dynamic::Update_CombinedTransformaitionMatrix(D3DXFRAME_DERIVED* pFrame, const _matrix* pParentCombinedMatrix)
{
	pFrame->matCombinedTransformationMatrix = pFrame->TransformationMatrix * *pParentCombinedMatrix;

	if (nullptr != pFrame->pFrameFirstChild)
	{
		Update_CombinedTransformaitionMatrix((D3DXFRAME_DERIVED*)pFrame->pFrameFirstChild, (_matrix*)&pFrame->matCombinedTransformationMatrix);
	}

	if (nullptr != pFrame->pFrameSibling)
	{
		Update_CombinedTransformaitionMatrix((D3DXFRAME_DERIVED*)pFrame->pFrameSibling, pParentCombinedMatrix);
	}

	return NOERROR;
}

HRESULT CMesh_Dynamic::Get_BoneList(vector<D3DXFRAME_DERIVED*>* pFrameList, D3DXFRAME_DERIVED * pFrame)
{
	if (pFrameList == nullptr)
		return E_FAIL;

	if (pFrame == nullptr)
		pFrame = m_pFrame;
	else
		pFrameList->push_back(pFrame);

	if (nullptr != pFrame->pFrameFirstChild)
	{
		Get_BoneList(pFrameList, (D3DXFRAME_DERIVED*)pFrame->pFrameFirstChild);
	}

	if (nullptr != pFrame->pFrameSibling)
	{
		Get_BoneList(pFrameList, (D3DXFRAME_DERIVED*)pFrame->pFrameSibling);
	}

	return NOERROR;
}

HRESULT CMesh_Dynamic::SetUp_BoneMatrixPointer(D3DXFRAME_DERIVED* pFrame)
{
	if (nullptr != pFrame->pMeshContainer)
	{
		D3DXMESHCONTAINER_DERIVED*		pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pFrame->pMeshContainer;		

		for (_ulong i = 0; i < pMeshContainer->dwNumBones; i++)
		{
			
			const char*		pTmp = pMeshContainer->pSkinInfo->GetBoneName(i);

			D3DXFRAME_DERIVED* pFrame_Find = (D3DXFRAME_DERIVED*)D3DXFrameFind(m_pFrame, pTmp);

			pMeshContainer->ppCombinedTransformationMatrices[i] = &pFrame_Find->matCombinedTransformationMatrix;
		}	

		m_vecMeshContainer.push_back(pMeshContainer);
	}

	if (nullptr != pFrame->pFrameFirstChild)
	{
		if (FAILED(SetUp_BoneMatrixPointer((D3DXFRAME_DERIVED*)pFrame->pFrameFirstChild)))
			return E_FAIL;
	}

	if (nullptr != pFrame->pFrameSibling)
	{
		if (FAILED(SetUp_BoneMatrixPointer((D3DXFRAME_DERIVED*)pFrame->pFrameSibling)))
			return E_FAIL;
	}

	return NOERROR;
}

CComponent * CMesh_Dynamic::Clone(void)
{
	return new CMesh_Dynamic(*this);
}

CMesh_Dynamic * CMesh_Dynamic::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar * pFilePath, const _tchar * pFileName, _vec3 vecStartAngle)
{
	CMesh_Dynamic *	pInstance = new CMesh_Dynamic(pGraphicDev);

	if (FAILED(pInstance->Ready_Mesh(pFilePath, pFileName, vecStartAngle)))
	{
		MessageBox(0, L"CMesh_Dynamic Created Failed", nullptr, MB_OK);
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}

#ifndef _FINAL
void CMesh_Dynamic::Debug_Render()
{
	D3DXTRACK_DESC			Track_Desc;
	ZeroMemory(&Track_Desc, sizeof(D3DXTRACK_DESC));
	Track_Desc = *m_pAnimation_Ctrl->Get_TrackInfo();

	CDebug::GetInstance()->Add_DebugText("CMesh_Dynamic");

	CDebug::GetInstance()->Add_DebugText("m_matStartMatrix.m[0]: %f %f %f", m_matStartMatrix.m[0][0], m_matStartMatrix.m[0][1], m_matStartMatrix.m[0][2]);
	CDebug::GetInstance()->Add_DebugText("m_matStartMatrix.m[1]: %f %f %f", m_matStartMatrix.m[1][0], m_matStartMatrix.m[1][1], m_matStartMatrix.m[1][2]);
	CDebug::GetInstance()->Add_DebugText("m_matStartMatrix.m[2]: %f %f %f", m_matStartMatrix.m[2][0], m_matStartMatrix.m[2][1], m_matStartMatrix.m[2][2]);
	CDebug::GetInstance()->Add_DebugText("m_matStartMatrix.m[3]: %f %f %f", m_matStartMatrix.m[3][0], m_matStartMatrix.m[3][1], m_matStartMatrix.m[3][2]);

	CDebug::GetInstance()->Add_DebugText("m_matCurAniLocal.m[0]: %f %f %f", m_matCurAniLocal.m[0][0], m_matCurAniLocal.m[0][1], m_matCurAniLocal.m[0][2]);
	CDebug::GetInstance()->Add_DebugText("m_matCurAniLocal.m[1]: %f %f %f", m_matCurAniLocal.m[1][0], m_matCurAniLocal.m[1][1], m_matCurAniLocal.m[1][2]);
	CDebug::GetInstance()->Add_DebugText("m_matCurAniLocal.m[2]: %f %f %f", m_matCurAniLocal.m[2][0], m_matCurAniLocal.m[2][1], m_matCurAniLocal.m[2][2]);
	CDebug::GetInstance()->Add_DebugText("m_matCurAniLocal.m[3]: %f %f %f", m_matCurAniLocal.m[3][0], m_matCurAniLocal.m[3][1], m_matCurAniLocal.m[3][2]);

	CDebug::GetInstance()->Add_DebugText("m_matPreAniLocal.m[0]: %f %f %f", m_matPreAniLocal.m[0][0], m_matPreAniLocal.m[0][1], m_matPreAniLocal.m[0][2]);
	CDebug::GetInstance()->Add_DebugText("m_matPreAniLocal.m[1]: %f %f %f", m_matPreAniLocal.m[1][0], m_matPreAniLocal.m[1][1], m_matPreAniLocal.m[1][2]);
	CDebug::GetInstance()->Add_DebugText("m_matPreAniLocal.m[2]: %f %f %f", m_matPreAniLocal.m[2][0], m_matPreAniLocal.m[2][1], m_matPreAniLocal.m[2][2]);
	CDebug::GetInstance()->Add_DebugText("m_matPreAniLocal.m[3]: %f %f %f", m_matPreAniLocal.m[3][0], m_matPreAniLocal.m[3][1], m_matPreAniLocal.m[3][2]);

	CDebug::GetInstance()->Add_DebugText("m_vecStartAngle: %f %f %f", m_vecStartAngle.x, m_vecStartAngle.y, m_vecStartAngle.z);
	CDebug::GetInstance()->Add_DebugText("m_vecDeltaPosition_Local: %f %f %f", m_vecDeltaPosition_Local.x, m_vecDeltaPosition_Local.y, m_vecDeltaPosition_Local.z);
	CDebug::GetInstance()->Add_DebugText("m_vecDeltaAngle_Local: %f %f %f", m_vecDeltaAngle_Local.x, m_vecDeltaAngle_Local.y, m_vecDeltaAngle_Local.z);

	CDebug::GetInstance()->Add_DebugText("m_iCurLoopCount: %d", m_iCurLoopCount);
	CDebug::GetInstance()->Add_DebugText("m_iPreLoopCount: %d", m_iCurLoopCount);

	CDebug::GetInstance()->Add_DebugText("m_fInitDelay: %f", m_fInitDelay);

	
	CDebug::GetInstance()->Add_DebugText("");
	CDebug::GetInstance()->Add_DebugText("--------- Animation --------- ");
	CDebug::GetInstance()->Add_DebugText("Weight: %f", Track_Desc.Weight);
	CDebug::GetInstance()->Add_DebugText("Speed: %f", Track_Desc.Speed);
	CDebug::GetInstance()->Add_DebugText("Position: %f", (float)Track_Desc.Position);
	CDebug::GetInstance()->Add_DebugText("Period: %f", m_pAnimation_Ctrl->Get_Period());
	CDebug::GetInstance()->Add_DebugText("AniLerp: %f", m_pAnimation_Ctrl->Get_AniLerp());
	CDebug::GetInstance()->Add_DebugText("AniSpeed: %f", m_pAnimation_Ctrl->Get_Speed());
	CDebug::GetInstance()->Add_DebugText("Period: %f", m_pAnimation_Ctrl->Get_Period());
	CDebug::GetInstance()->Add_DebugText("AniSet_MaxSize: %d", (int)m_pAnimation_Ctrl->Get_MaxAniSet());
	CDebug::GetInstance()->Add_DebugText("AniSet_Cur: %d", (int)m_pAnimation_Ctrl->Get_CurAniSet());
	CDebug::GetInstance()->Add_DebugText("--------- Custom Animation Info --------- ");
	ANI_STATUS tInfo = m_pAnimation_Ctrl->Get_CurAnimationInfo();
	CDebug::GetInstance()->Add_DebugText("Ani_Index: %d", tInfo.iAni_Index);
	CDebug::GetInstance()->Add_DebugText("Ani_Speed: %f", tInfo.fAni_Speed);
	CDebug::GetInstance()->Add_DebugText("Cut_Period: %f", (float)tInfo.fPeriod_Minus);
	if (tInfo.bRoot_Move)
	{
		CDebug::GetInstance()->Add_DebugText("Root_Move: true");
		if (m_pRootMotionFrame != nullptr)
			CDebug::GetInstance()->Add_DebugText(m_pRootMotionFrame->Name);
	}
	else
		CDebug::GetInstance()->Add_DebugText("Root_Move: false");
	
	CDebug::GetInstance()->Add_DebugText("EventSize: %d", tInfo.iArrSize);

	CDebug::GetInstance()->Add_DebugText("m_dwNumMeshContainer: %d", m_dwNumMeshContainer);
	for (auto& pMeshContainer : m_vecMeshContainer)
	{
		if(pMeshContainer->Name!=nullptr)
			CDebug::GetInstance()->Add_DebugText("-- %s -- ", pMeshContainer->Name);
				
		if (pMeshContainer->Name != nullptr)
			CDebug::GetInstance()->Add_DebugText("-- bone : %d -- ", pMeshContainer->dwNumBones);

		for (int i = 0; i<pMeshContainer->NumMaterials; ++i)
		{
			CDebug::GetInstance()->Add_DebugText("---- mat : %s ---- ", pMeshContainer->pMaterials[i].pTextureFilename);

			if (nullptr != pMeshContainer->pMeshTexture[i].pTexture[TEXTUREDATA::TEX_DIFFUSE])
				CDebug::GetInstance()->Add_DebugText("OK_DIFFUSE");
			else
				CDebug::GetInstance()->Add_DebugText("NO_DIFFUSE");

			if (nullptr != pMeshContainer->pMeshTexture[i].pTexture[TEXTUREDATA::TEX_NORMAL])
				CDebug::GetInstance()->Add_DebugText("OK_NORMAL");
			else
				CDebug::GetInstance()->Add_DebugText("NO_NORMAL");

			if (nullptr != pMeshContainer->pMeshTexture[i].pTexture[TEXTUREDATA::TEX_SPECULAR])
				CDebug::GetInstance()->Add_DebugText("OK_SPECULAR");
			else
				CDebug::GetInstance()->Add_DebugText("NO_SPECULAR");
		}
	}
}
#endif
_ulong CMesh_Dynamic::Free(void)
{
	if(false == m_isClone)
	{
		m_pLoader->DestroyFrame(m_pFrame);	

		Safe_Delete(m_pLoader);
	}
	else
	{
		Frame_Derived::DestroyFrame(m_pCloneFrame);
		m_pCloneFrame = nullptr;
		//Safe_Delete(m_pCloneFrame);
	}

	m_vecMeshContainer.clear();

	Safe_Release(m_pAnimation_Ctrl);

	return CMesh::Free();
}
