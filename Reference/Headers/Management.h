#pragma once

#include "Engine_Defines.h"

#include "Base.h"

// ���� �ΰ��ӿ� ��������� ���� �ּҸ� ������ �ְ�, ����� �������� ����ϳ�.

BEGIN(Engine)

class CScene;
class CRenderer;
class DLL_EXPORT CManagement : public CBase
{
	DECLARE_SINGLETON(CManagement)
private:
	explicit CManagement();
	virtual ~CManagement();
public:
	HRESULT SetUp_CurrentScene(CScene* pNewScene);
	HRESULT SetUp_Renderer(CRenderer* pRenderer);
public:
	HRESULT Ready_Management(const _uint& iMaxSize, LPDIRECT3DDEVICE9 pGraphicDev);
	_int Update_Management(const _float& fTimeDelta);
	void Render_Management(void);
	FORCEINLINE  CScene* Get_Current_Scene() const {
		return m_pCurrent_Scene;
	}
private:
	Engine::CScene*				m_pCurrent_Scene = nullptr;
	Engine::CScene*				m_pNew_Scene = nullptr;
	Engine::CRenderer*			m_pRenderer = nullptr;
public:
#ifndef _FINAL
	void Debug_Render();
#endif
private:
	virtual _ulong Free(void) final;
};

END