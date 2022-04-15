#pragma once

#include "Engine_Defines.h"
#include "Base.h"
#include "GameObject.h"
#include "Camera.h"

BEGIN(Engine)

class CCameraLayer;
//class CCamera;
class CComponent;
class CRenderer;

class DLL_EXPORT CCamera_Manager : public CBase
{
	DECLARE_SINGLETON(CCamera_Manager)
private:
	explicit CCamera_Manager();
	virtual ~CCamera_Manager();

public:
	_uint Get_NumCamera(const _uint& iSceneIdx, const _tchar* pLayerTag);
public:
	HRESULT Reserve_CameraContainer(const _uint&  iMaxSize);
	HRESULT Release_Camera(const _uint& iSceneIdx);

public:
	CCamera*		Get_FindCamera(const _uint & iArrayIdx, const _tchar * pLayerTag, const _uint & iLayerIdx);
	CCamera*		Get_CurrentCamera();

public:
	void Set_IsChangeCamera(_bool IsChangeCamera) { m_IsChangeCamera = IsChangeCamera; }
	HRESULT Set_CurrentCamera(CCamera* pCamera);
	HRESULT SetUp_Renderer(CRenderer* pRenderer);
public:
	HRESULT Add_Camera(const _uint & iArrayIdx, const _tchar* pLayerTag, CCamera * pCamera);

	HRESULT Reserve_NextCamera(CCamera * pCamera);

public:
	HRESULT Ready_Camera_Manager(void);
	_int Update_Camera_Manager(const _float& fTimeDelta);
	_int LastUpdate_Camera_Manager(const _float& fTimeDelta);

	_vec3 Get_WorldPosition();
	_matrix Get_WorldMatrix();
	_matrix Get_ProjMatrix();
	_matrix Get_ViewMatrix();
	_matrix Get_OldProjMatrix();
	_matrix Get_OldViewMatrix();
	_matrix Get_InverseProjMatrix();
	_matrix Get_InverseViewMatrix();
	_float  Get_Far();
	_vec3 Get_Direction();
	_vec3 Get_Direction_NotY();
	_bool Get_IsChangeCamera() { return m_IsChangeCamera; }

	_vec3 Get_At();
	_vec3 Get_Eye();


	
public:
	CCameraLayer* Find_Layer(const _uint& iArrayIdx, const _tchar* pLayerTag);

private:
	map<const _tchar*, CCameraLayer*>*			m_pMapLayer = nullptr;
	typedef map<const _tchar*, CCameraLayer*>	MAPLAYER;

private:
	list<CCamera*>								m_ReserveList;
	typedef	list<CCamera*>						RESERVELIST;

private:
	CCamera*								m_pCurrentCamera = nullptr;	
	CRenderer*								m_pRenderer = nullptr;
private:
	_uint									m_iContainerSize = 0;

private:
	_bool									m_IsChangeCamera = true;

#ifndef _FINAL
public:
	void Debug_Render();
#endif
private:
	virtual _ulong Free(void) final;

};

END