#include "..\Headers\Animation_Ctrl.h"
#include "Debug_Manager.h"

USING(Engine)

CAnimation_Ctrl::CAnimation_Ctrl(LPDIRECT3DDEVICE9 pGraphicDev, LPD3DXANIMATIONCONTROLLER	pAnimation_Ctrl)
	: m_pGraphic_Device(pGraphicDev)
	, m_pAnimation_Ctrl(pAnimation_Ctrl)
	, m_iMaxNumAniSet(0)
	, m_Period(0.0)
	, m_pCurAniInfo(nullptr)
	, m_bIsClone(false)
{
	m_pGraphic_Device->AddRef();
	m_pAnimation_Ctrl->AddRef();
#ifndef _FINAL
	Tag(L"CAnimation_Ctrl", std::bind(&CAnimation_Ctrl::Debug_Render, this));
#endif

}

CAnimation_Ctrl::CAnimation_Ctrl(const CAnimation_Ctrl & rhs)
	: m_pGraphic_Device(rhs.m_pGraphic_Device)
	, m_pAnimation_Ctrl(nullptr)
	, m_iCurrentTrack(rhs.m_iCurrentTrack)
	, m_iNewTrack(rhs.m_iNewTrack)
	, m_Period(rhs.m_Period)
	, m_pCurAniInfo(rhs.m_pCurAniInfo)
	, m_bIsClone(true)
{
	rhs.m_pAnimation_Ctrl->CloneAnimationController(rhs.m_pAnimation_Ctrl->GetMaxNumAnimationOutputs()
		, m_iMaxNumAniSet = rhs.m_pAnimation_Ctrl->GetNumAnimationSets(), rhs.m_pAnimation_Ctrl->GetMaxNumTracks(), rhs.m_pAnimation_Ctrl->GetMaxNumEvents(), &m_pAnimation_Ctrl);

	m_pGraphic_Device->AddRef();
#ifndef _FINAL
	Tag(L"CAnimation_Ctrl", std::bind(&CAnimation_Ctrl::Debug_Render, this));
#endif

}

HRESULT CAnimation_Ctrl::Set_AnimationSet(const _uint & iAni_Index)
{
	if (iAni_Index == m_iOldAniSetIdx)
		return E_FAIL;

	if (iAni_Index >= m_iMaxNumAniSet)
		return E_FAIL;

	if (nullptr == m_pAnimation_Ctrl)
		return E_FAIL;

	// 애니메이션이 바뀐다.
	m_iNewTrack = m_iCurrentTrack == 0 ? 1 : 0;

	LPD3DXANIMATIONSET		pAniSet = nullptr;

	if (FAILED(m_pAnimation_Ctrl->GetAnimationSet(iAni_Index, &pAniSet)))
		return E_FAIL;

	m_Period = pAniSet->GetPeriod();


	if (FAILED(m_pAnimation_Ctrl->SetTrackAnimationSet(m_iNewTrack, pAniSet)))
		return E_FAIL;

	m_pAnimation_Ctrl->UnkeyAllTrackEvents(m_iCurrentTrack);
	m_pAnimation_Ctrl->UnkeyAllTrackEvents(m_iNewTrack);

	m_pAnimation_Ctrl->KeyTrackEnable(m_iCurrentTrack, FALSE, m_fTimeAcc + m_fAniLerp);
	m_pAnimation_Ctrl->KeyTrackSpeed(m_iCurrentTrack, 1.0f, m_fTimeAcc, m_fAniLerp, D3DXTRANSITION_LINEAR);
	m_pAnimation_Ctrl->KeyTrackWeight(m_iCurrentTrack, 0.1f, m_fTimeAcc, m_fAniLerp, D3DXTRANSITION_LINEAR);

	if (FAILED(m_pAnimation_Ctrl->SetTrackEnable(m_iNewTrack, TRUE)))
		return E_FAIL;	

	m_pAnimation_Ctrl->KeyTrackSpeed(m_iNewTrack, 1.0f, m_fTimeAcc, m_fAniLerp, D3DXTRANSITION_LINEAR);
	m_pAnimation_Ctrl->KeyTrackWeight(m_iNewTrack, 0.9f, m_fTimeAcc, m_fAniLerp, D3DXTRANSITION_LINEAR);
	
	m_pAnimation_Ctrl->SetTrackPosition(m_iNewTrack, 0.0);

	//m_pAnimation_Ctrl->SetTrackSpeed();

	m_pAnimation_Ctrl->ResetTime();	
	m_fTimeAcc = 0.f;

	m_iOldAniSetIdx = iAni_Index;
	
	m_iCurrentTrack = m_iNewTrack;
	
	return NOERROR;
}

_bool CAnimation_Ctrl::is_Finish_Animation(double fEndDelay)
{
	if (m_Track_Desc.Position >= m_Period - fEndDelay)
	{
		return true;
	}
	else
	{
		return false;
	}
}

int CAnimation_Ctrl::is_Finish_Animation()
{

	return int(m_Track_Desc.Position / (m_Period - ((*m_pCurAniInfo)[m_iOldAniSetIdx].fPeriod_Minus)));

}

HRESULT CAnimation_Ctrl::Ready_Animation_Ctrl(const _tchar * pFileFullPath)
{
	if (pFileFullPath != nullptr)
	{
		if (FAILED(Load_AnimationData(pFileFullPath)))
		{
			return E_FAIL;
		}
	}

	return NOERROR;
}

void CAnimation_Ctrl::Play_Animation(const _float & fTimeDelta)
{
	if (nullptr == m_pAnimation_Ctrl)
		return;

	m_fAniSpeed = (*m_pCurAniInfo)[m_iOldAniSetIdx].fAni_Speed;
	m_pAnimation_Ctrl->AdvanceTime(fTimeDelta * m_fAniSpeed , nullptr);

	m_fTimeAcc += fTimeDelta * m_fAniSpeed;

	ZeroMemory(&m_Track_Desc, sizeof(D3DXTRACK_DESC));
	m_pAnimation_Ctrl->GetTrackDesc(m_iCurrentTrack, &m_Track_Desc);

	
}

void CAnimation_Ctrl::SetUp_Animation()
{
	if (nullptr == m_pAnimation_Ctrl)
		return;

	m_pAnimation_Ctrl->AdvanceTime(0.0, nullptr);
}


HRESULT CAnimation_Ctrl::Load_AnimationData(const _tchar * pFileFullPath)
{

	int iAniSetSize = m_iMaxNumAniSet = m_pAnimation_Ctrl->GetNumAnimationSets();
	
	if (m_pCurAniInfo == nullptr)
	{
		m_pCurAniInfo = new vector<ANI_STATUS>;
		m_pCurAniInfo->reserve((size_t)iAniSetSize);
	}

	//// %%%%%%%%%%%%%%%    실제로 파싱해서 데이터 얻어올 것 %%%%%%%%%%%%%%%%%
	//for (int i = 0; i < iAniSetSize; ++i)
	//{
	//	ANI_STATUS tmpInfo;
	//	tmpInfo.iArrSize = 0;
	//	tmpInfo.m_AniEvent = nullptr;
	//	tmpInfo.iAni_Index = i;
	//	tmpInfo.fAni_Speed = 1.0f;
	//	tmpInfo.fPeriod_Minus = 0.25f;
	//	tmpInfo.bRoot_Move = true;

	//	m_pCurAniInfo->push_back(tmpInfo);
	//}
	//// %%%%%%%%%%%%%%%    실제로 파싱해서 데이터 얻어올 것 %%%%%%%%%%%%%%%%%




	HANDLE hFile = CreateFile(
		pFileFullPath,		//파일 경로.
		GENERIC_READ,	//어떤 모드로? : 쓰기전용
		NULL,		//공유모드
		NULL,
		OPEN_EXISTING,	//파일옵션 : 파일이 있으면 덮어쓰기 -> 없으면 새로 만들라는 인자값.
		FILE_ATTRIBUTE_NORMAL,		//기본값으로 파일을 생성하라는옵션.
		NULL
	);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);

		for (int i = 0; i < iAniSetSize; ++i)
		{
			ANI_STATUS tmpInfo;
			tmpInfo.iArrSize = 0;
			tmpInfo.m_AniEvent = nullptr;
			tmpInfo.iAni_Index = i;
			tmpInfo.fAni_Speed = 1.0f;
			tmpInfo.fPeriod_Minus = 0.25f;
			tmpInfo.bRoot_Move = true;

			m_pCurAniInfo->push_back(tmpInfo);
		}
	}
	else
	{
		vector<ANI_STATUS> TempAnimation;

		DWORD dwByte = 0;

		int iAni_Full_Size = 0;
		ReadFile(hFile, &iAni_Full_Size, sizeof(int), &dwByte, NULL);


		if (m_pCurAniInfo == nullptr)
		{
			m_pCurAniInfo = new vector<ANI_STATUS>;
			m_pCurAniInfo->reserve((size_t)iAni_Full_Size);
		}


		for (int i = 0; i < iAni_Full_Size; ++i)
		{
			ANI_STATUS* pAni_Status = new ANI_STATUS;

			ReadFile(hFile, pAni_Status, sizeof(ANI_STATUS), &dwByte, NULL);
			pAni_Status->fAni_Speed = 1.0f;

			int iArrSize = pAni_Status->iArrSize;

			if (0 == iArrSize)
			{
				pAni_Status->m_AniEvent = nullptr;
			}
			else
			{
				ANI_EVENT* pAni_Event = new ANI_EVENT[iArrSize];

				for (int j = 0; j < iArrSize; ++j)
				{
					ReadFile(hFile, &pAni_Event[j], sizeof(ANI_EVENT), &dwByte, NULL);
				}
				pAni_Status->m_AniEvent = pAni_Event;
			}


			TempAnimation.push_back(*pAni_Status);
			Safe_Delete(pAni_Status);
		}
		CloseHandle(hFile);


		m_pCurAniInfo->clear();
		m_pCurAniInfo->swap(TempAnimation);
	}

	return NOERROR;
}

HRESULT CAnimation_Ctrl::Save_AnimationData(const _tchar * pFileFullPath)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	DWORD dwByte = 0;


	HANDLE hFile = CreateFile(
		pFileFullPath,		//파일 경로.
		GENERIC_WRITE,	//어떤 모드로? : 쓰기전용
		NULL,		//공유모드
		NULL,
		CREATE_ALWAYS,	//파일옵션 : 파일이 있으면 덮어쓰기 -> 없으면 새로 만들라는 인자값.
		FILE_ATTRIBUTE_NORMAL,		//기본값으로 파일을 생성하라는옵션.
		NULL
	);

	WriteFile(hFile, &m_iMaxNumAniSet, sizeof(int), &dwByte, NULL);

	for (size_t i = 0; i < m_iMaxNumAniSet; ++i)
	{
		WriteFile(hFile, &(*m_pCurAniInfo)[i], sizeof(ANI_STATUS), &dwByte, NULL);

		int iEvent_Size = (*m_pCurAniInfo)[i].iArrSize;

		if (0 < iEvent_Size)
		{
			for (int j = 0; j < iEvent_Size; ++j)
			{
				WriteFile(hFile, &(*m_pCurAniInfo)[i].m_AniEvent[j], sizeof(ANI_EVENT), &dwByte, NULL);
			}
		}
	}

	CloseHandle(hFile);


	return NOERROR;
}

CAnimation_Ctrl * CAnimation_Ctrl::Create(LPDIRECT3DDEVICE9 pGraphicDev, LPD3DXANIMATIONCONTROLLER pAnimation_Ctrl, const _tchar * pFileFullPath)
{
	CAnimation_Ctrl *	pInstance = new CAnimation_Ctrl(pGraphicDev, pAnimation_Ctrl);

	if (FAILED(pInstance->Ready_Animation_Ctrl(pFileFullPath)))
	{
		MessageBox(0, L"CAnimation_Ctrl Created Failed", nullptr, MB_OK);
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}


CAnimation_Ctrl * CAnimation_Ctrl::Create(const CAnimation_Ctrl & rhs)
{
	CAnimation_Ctrl *	pInstance = new CAnimation_Ctrl(rhs);

	if (FAILED(pInstance->Ready_Animation_Ctrl()))
	{
		MessageBox(0, L"CAnimation_Ctrl Clone Failed", nullptr, MB_OK);
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}
#ifndef _FINAL
void CAnimation_Ctrl::Debug_Render()
{
	CDebug::GetInstance()->Add_DebugText("CAnimation_Ctrl");
}
#endif
_ulong CAnimation_Ctrl::Free(void)
{
	Safe_Release(m_pAnimation_Ctrl);
	Safe_Release(m_pGraphic_Device);
	

	if (!m_bIsClone)
	{
		int iAni_Status_Size = (int)m_pCurAniInfo->size();

		for (size_t i = 0; i < iAni_Status_Size; ++i)
		{
			Safe_Delete_Array((*m_pCurAniInfo)[i].m_AniEvent);
		}
		Safe_Delete(m_pCurAniInfo);
	}


	return _ulong();
}
