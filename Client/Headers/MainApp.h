#pragma once

#include "Defines.h"
#include "Base.h"
#include "Graphic_Device.h"

namespace Engine
{
	class CComponent_Manager;
	class CManagement;
}

class CMainApp final : public CBase
{
private:
	explicit CMainApp();	
	virtual ~CMainApp();	
public:
	HRESULT Ready_MainApp(void); // CMainApp 객체를 초기화한다.
	int Update_MainApp(const float& fTimeDelta); // CMainApp 객체를 업데이트한다.
	void Render_MainApp(void);
private:
	Engine::CGraphic_Device*		m_pGraphic_Device = nullptr;
	Engine::CComponent_Manager*		m_pComponent_Manager = nullptr;
	Engine::CManagement*			m_pManagement = nullptr;
private:
	LPDIRECT3DDEVICE9				m_pGraphicDev = nullptr;
private:
	char							m_szFPS[32] = "";
	char							m_DeviceRef[16] = "";

	_int							m_iFps = 0;
	_int							m_iRenderFps = 0;
	_float							m_fTimer = 0.f;
private:
	HRESULT Ready_DefaultSetting(HWND hWnd, Engine::CGraphic_Device::WINMODE eMode, const _uint& iBackCX, const _uint& iBackCY);
	HRESULT Ready_StaticComponent(void);
	HRESULT Ready_StartScene(void);
	void	Render_FPS();
	void Render_DeviceRef();
public:
	static CMainApp* Create(void);
public:
	// override 부모클래스에 있는 함수를 오버라이딩한 함수다.
	virtual unsigned long Free(void) /*override*/final;
};


