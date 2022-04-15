#pragma once
#include "Tool_Defines.h"
#include "Engine_Defines.h"

// CShaderToolMainView 뷰입니다.
namespace Engine
{
	class CManagement;
	class CComponent_Manager;
	class CObject_Manager;
	class CNavigationMesh;
	class CGraphic_Device;
}

class CShaderToolMainView : public CView
{
	DECLARE_DYNCREATE(CShaderToolMainView)

protected:
	CShaderToolMainView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CShaderToolMainView();
public:
	LPDIRECT3DDEVICE9	Get_GraphicDev( void ) const { return m_pGraphicDev; }
public:
	CPoint		m_ptMouse;
	_bool		m_bMouseInClient = true;
	POINT		m_ptMove;
	POINT		m_ptPrevMouse;
public:
	void Update_MainView( const _float& _fTimeDelta );
private:
	Engine::CGraphic_Device*	m_pGraphic_Device = nullptr;
	Engine::CManagement*		m_pManagement = nullptr;
	Engine::CComponent_Manager*	m_pComponent_Manager = nullptr;
	Engine::CObject_Manager*	m_pObject_Manager = nullptr;
	Engine::CNavigationMesh*	m_pNaviMeshCom = nullptr;
	LPDIRECT3DDEVICE9			m_pGraphicDev = nullptr;
private:
	HRESULT Ready_DefaultSetting( void );
	HRESULT Ready_Management( void );
	HRESULT	Ready_StaticComponent( void );
	HRESULT Ready_Scene( void );
public:
	virtual void OnDraw(CDC* pDC);      // 이 뷰를 그리기 위해 재정의되었습니다.
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnMouseMove( UINT nFlags, CPoint point );
};


