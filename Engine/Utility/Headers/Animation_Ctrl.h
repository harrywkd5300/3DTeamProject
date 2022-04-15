#pragma once

#include "Engine_Defines.h"
#include "Base.h"

BEGIN(Engine)

class CAnimation_Ctrl : public CBase
{
private:
	explicit CAnimation_Ctrl(LPDIRECT3DDEVICE9	pGraphicDev, LPD3DXANIMATIONCONTROLLER	pAnimation_Ctrl);
	explicit CAnimation_Ctrl(const CAnimation_Ctrl& rhs);
	virtual ~CAnimation_Ctrl() = default;
	
public:
	HRESULT Set_AnimationSet(const _uint& iAni_Index);
	_bool is_Finish_Animation(double fEndDelay); // 맵툴 전용
	int is_Finish_Animation();
public:
	HRESULT Ready_Animation_Ctrl(const _tchar * pFileFullPath = nullptr);
	void Play_Animation(const _float& fTimeDelta);
	void SetUp_Animation();

	FORCEINLINE D3DXTRACK_DESC* Get_TrackInfo() { return &m_Track_Desc; }
	FORCEINLINE _double Get_Period() { return m_Period; }
	FORCEINLINE _double* Get_Pointer_Period() { return &m_Period; }
	FORCEINLINE void Set_Speed (float fSpeed) { (*m_pCurAniInfo)[m_iOldAniSetIdx].fAni_Speed = fSpeed; } // 클라이언트에서 절대 사용 금지
	FORCEINLINE float Get_Speed() const { return m_fAniSpeed; }
	const ANI_STATUS& Get_CurAnimationInfo() const { return (*m_pCurAniInfo)[m_iOldAniSetIdx]; }
	
	FORCEINLINE const float Get_AniLerp() const { return m_fAniLerp; }
	FORCEINLINE const _uint Get_MaxAniSet() const { return m_iMaxNumAniSet; }
	FORCEINLINE const _uint Get_CurAniSet() const { return m_iOldAniSetIdx; }
	
private:
	LPDIRECT3DDEVICE9			m_pGraphic_Device = nullptr;
	LPD3DXANIMATIONCONTROLLER	m_pAnimation_Ctrl = nullptr;
	D3DXTRACK_DESC				m_Track_Desc; // 현재 트랙 정보

	bool						m_bIsClone = false;

	_uint						m_iCurrentTrack = 0;
	_uint						m_iNewTrack = 1;
	_float						m_fTimeAcc = 0.f;

	_uint						m_iMaxNumAniSet = 0; // 최대 애니메이션 인덱스
	_uint						m_iOldAniSetIdx = -1; // 현재 애니메이션 인덱스

	_double						m_Period = 0.0;
	_float						m_fAniSpeed = 1.f;

	_float						m_fAniLerp = 0.25f; // 애니메이션 간 보간 길이

	// 해당 메쉬 애니메이션 정보(이벤트, 끝나는 시기.. 등)
	vector<ANI_STATUS>*	m_pCurAniInfo = nullptr;	

private:
	HRESULT Load_AnimationData(const _tchar * pFileFullPath);
public:
	HRESULT Save_AnimationData(const _tchar * pFileFullPath);
	FORCEINLINE ANI_STATUS& Get_CurAnimationInfo_ForMapTool() { return (*m_pCurAniInfo)[m_iOldAniSetIdx]; }
	FORCEINLINE ANI_STATUS& Get_AnimationInfo_ForMapTool(int iIndex) { 
		if(iIndex<=(int)m_iMaxNumAniSet && iIndex>=0)
			return (*m_pCurAniInfo)[iIndex];
	}
public:
	static CAnimation_Ctrl* Create(LPDIRECT3DDEVICE9 pGraphicDev, LPD3DXANIMATIONCONTROLLER	pAnimation_Ctrl, const _tchar * pFileFullPath);
	static CAnimation_Ctrl* Create(const CAnimation_Ctrl& rhs);
#ifndef _FINAL
public:
	void Debug_Render();
#endif
	virtual _ulong Free(void);
};

END
