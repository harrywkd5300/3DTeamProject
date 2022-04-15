#pragma once


#include "GameObject.h"

// Client상에 필요한 뷰스페이스 변환 and 투영 변환에대한 관리를 수행한다.

BEGIN(Engine)

class CInput_Device;
class DLL_EXPORT CCamera abstract : public CGameObject
{
protected:
	explicit CCamera(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCamera(void) = default;

	
public:
	_vec3 Get_WorldPosition() { return *((_vec3*)m_matView_Inverse.m[3]); }
	_matrix Get_WorldMatrix() { return m_matWorld; }
	_matrix Get_ProjMatrix() { return m_matProj; }
	_matrix Get_ViewMatrix() { return m_matView; }
	_matrix Get_OldProjMatrix() { return m_matProj_Old; }
	_matrix Get_OldViewMatrix() { return m_matView_Old; }
	_matrix Get_InverseProjMatrix() { return m_matProj_Inverse; }
	_matrix Get_InverseViewMatrix() { return m_matView_Inverse; }
	_float	Get_Far() { return m_fFar; }

	_vec3 Get_Direction() { return m_vDir; }
	_vec3 Get_Direction_NotY() { return m_vDir_NotY; }
	_vec3 Get_At() { return m_vAt; }
	_vec3 Get_Eye() { return m_vEye; }

public:
	void Set_Operation(_bool bOperate) { m_bOperate = bOperate; }

public:
	virtual HRESULT Ready_GameObject(const _vec3* pEye, const _vec3* pAt, const _vec3* pUp
		, const _float& fFovY, const _float& fAspect, const _float& fNear, const _float& fFar);
	virtual _int Update_GameObject(const _float& fTimeDelta);
	virtual _int LastUpdate_GameObject(const _float& fTimeDelta);


private:
	void SetUp_ViewMatrix(_matrix* pMatrix);
	void SetUp_ProjMatrix(_matrix* pMatrix);

protected:
	CInput_Device*		m_pInput_Device = nullptr;
	_bool				m_bOperate = false;
protected:
	_matrix				m_matWorld;
protected:
	_matrix				m_matView;
	_matrix				m_matView_Old;
	_matrix				m_matView_Inverse;
	_vec3				m_vEye;
	_vec3				m_vAt;
	_vec3				m_vFixedUp;
protected:
	_matrix				m_matProj;
	_matrix				m_matProj_Old;
	_matrix				m_matProj_Inverse;
	_float				m_fFovY;
	_float				m_fAspect;
	_float				m_fNear;
	_float				m_fFar;

	bool				m_bActive = false; // 플레이어카메라 항상 false?
										   // 씬카메라 동작중일때 ture
										   
protected:
	_vec3				m_vDir;
	_vec3				m_vDir_NotY;
#ifndef _FINAL
public:
	void Debug_Render();
#endif
protected:
	virtual _ulong Free(void);
};

END