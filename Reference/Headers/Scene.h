#pragma once

#include "Engine_Defines.h"
#include "Base.h"

#include "Object_Manager.h"
#include "Camera_Manager.h" //By.PSH
#include "Effect_Manager.h" //By.KJW

BEGIN( Engine )

class CCollider_Manager; // By.KMH
class DLL_EXPORT CScene abstract : public CBase
{	
protected:
	explicit CScene(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CScene() = default;
public:
	virtual HRESULT Ready_Scene(void);
	virtual _int Update_Scene(const _float& fTimeDelta);
	virtual _int LastUpdate_Scene(const _float& fTimeDelta);
	virtual void Render_Scene(void);
public:
	HRESULT Release_Object(const _uint& iSceneIdx);
	const _uint	Get_SceneIndex() const{ return m_iSceneIdx; }
protected:
	LPDIRECT3DDEVICE9				m_pGraphicDev = nullptr;
	_uint							m_iSceneIdx = 0;
protected:
	Engine::CCollider_Manager*		m_pCollider_Manager = nullptr; // By.KMH
	Engine::CObject_Manager*		m_pObject_Manager = nullptr;
	Engine::CCamera_Manager*		m_pCamera_Manager = nullptr;
	Engine::CEffect_Manager*		m_pEffect_Manager = nullptr;
public:
	void Debug_Render();

protected:
	virtual _ulong Free(void) override;
};

END