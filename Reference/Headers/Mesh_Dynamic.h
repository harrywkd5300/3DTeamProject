#pragma once

#include "Engine_Defines.h"
#include "Mesh.h"
#include "Animation_Ctrl.h"
BEGIN(Engine)

class CHierarchy_Loader;
//class CAnimation_Ctrl;
class DLL_EXPORT CMesh_Dynamic : public CMesh
{
private:
	explicit CMesh_Dynamic(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CMesh_Dynamic(const CMesh_Dynamic& rhs);
	virtual ~CMesh_Dynamic() = default;
public:
	const _vec3* Get_Min(const _uint& iMeshContainerIdx = 0);
	const _vec3* Get_Max(const _uint& iMeshContainerIdx = 0);
public:
	virtual void Update_Component(const _float& fTimeDelta);
	virtual void LastUpdate_Component(const _float& fTimeDelta);
public:
	_bool is_Finish_Animation(double fEndDelay); // 맵툴 전용
	int is_Finish_Animation();
	//_matrix* Find_FrameMatrixPointer(const char* pBoneName) {
	//	return &((D3DXFRAME_DERIVED*)D3DXFrameFind(m_pFrame, pBoneName))->matCombinedTransformationMatrix; }
	_matrix* Find_FrameMatrixPointer(const char* pBoneName) {
		return &((D3DXFRAME_DERIVED*)D3DXFrameFind(m_pCloneFrame, pBoneName))->matCombinedTransformationMatrix; }

public:
	virtual HRESULT Ready_Mesh(const _tchar* pFilePath, const _tchar* pFileName, _vec3 vecStartAngle);
	virtual void Render_Mesh(void);
	virtual void Render_Mesh(LPD3DXEFFECT pEffect, const char* pConstant_DiffuseTex_Name, const char* pConstant_NormalTex_Name, const char* pConstant_SpecularTex_Name);
	


public:
	HRESULT Set_AnimationSet(const _uint& iAni_Index); // 특정 동작을 셋팅하기위한 함수다(재생x)
	bool Play_Animation(const _float& fTimeDelta);
	FORCEINLINE _vec3 Get_MoveDelta() const { return m_vecDeltaPosition_Local; }
	FORCEINLINE _vec3 Get_AngleDelta() const { return m_vecDeltaAngle_Local; }
	FORCEINLINE _matrix Get_StartMatrix() const { return m_matStartMatrix; }
	FORCEINLINE const _uint Get_MaxAniSet() const { return m_pAnimation_Ctrl->Get_MaxAniSet(); }
	const ANI_STATUS& Get_CurAnimationInfo() const;

	FORCEINLINE CAnimation_Ctrl*	Get_AnimationCtrl_ForMapTool() { return m_pAnimation_Ctrl; }
private:
	CHierarchy_Loader*							m_pLoader = nullptr;
	D3DXFRAME_DERIVED*							m_pFrame = nullptr;
	D3DXFRAME_DERIVED*							m_pCloneFrame = nullptr;
	D3DXFRAME_DERIVED*							m_pRootMotionFrame = nullptr;

	_ulong										m_dwNumMeshContainer = 0;

	CAnimation_Ctrl*							m_pAnimation_Ctrl = nullptr;
	vector<D3DXMESHCONTAINER_DERIVED*>			m_vecMeshContainer;
	typedef vector<D3DXMESHCONTAINER_DERIVED*>	VECMESHCONTAINER;

	_vec3										m_vecStartAngle = _vec3(0.f,0.f,0.f);
	_matrix										m_matStartMatrix = *D3DXMatrixIdentity(&m_matStartMatrix);


	//_matrix		m_matStartAniLocal;
	bool		m_bInit = false;
	
	
	_matrix		m_matCurAniLocal = *D3DXMatrixIdentity(&m_matCurAniLocal);
	_matrix		m_matPreAniLocal = *D3DXMatrixIdentity(&m_matPreAniLocal);


	//_vec3		m_vecDeltaPosition_Local_Pre = _vec3(0.f,0.f,0.f);
	//_vec3		m_vecDeltaAngle_Local_Pre = _vec3(0.f, 0.f, 0.f);
	_vec3		m_vecDeltaPosition_Local = _vec3(0.f, 0.f, 0.f);
	_vec3		m_vecDeltaAngle_Local = _vec3(0.f, 0.f, 0.f);

	int			m_iCurLoopCount = 0;
	int			m_iPreLoopCount = 0;

	float		m_fInitDelay = 0.25f;

public:
	FORCEINLINE D3DXTRACK_DESC* Get_TrackInfo();
	FORCEINLINE _double Get_Period();
	FORCEINLINE _double* Get_Pointer_Period();
	void Set_Speed(float fSpeed);
	float Get_Speed();
	HRESULT Get_BoneList(vector<D3DXFRAME_DERIVED*>* pFrameList, D3DXFRAME_DERIVED * pFrame = nullptr);
	HRESULT Set_RootMotionFrame(const char* pBoneName)
	{
		if (pBoneName == nullptr)
			return E_FAIL;
		m_pRootMotionFrame = ((D3DXFRAME_DERIVED*)D3DXFrameFind(m_pFrame, pBoneName));

		return NOERROR;
	}
private:
	HRESULT Compute_NumMeshContainer(D3DXFRAME_DERIVED* pFrame);
	HRESULT Update_CombinedTransformaitionMatrix(D3DXFRAME_DERIVED* pFrame, const _matrix* pParentCombinedMatrix);
	//HRESULT Update_CombinedTransformaitionMatrix(D3DXFRAME_DERIVED* pFrame, const _matrix* pParentCombinedMatrix);
	

	HRESULT SetUp_BoneMatrixPointer(D3DXFRAME_DERIVED* pFrame);
public:
	virtual CComponent* Clone(void);
	static CMesh_Dynamic* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pFilePath, const _tchar* pFileName, _vec3 vecStartAngle = _vec3(270.f,180.f,0.f));
public:
#ifndef _FINAL
	void Debug_Render();
#endif
	virtual _ulong Free(void);	

};

END