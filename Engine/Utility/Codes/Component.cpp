#include "..\Headers\Component.h"
#include "Timer_Manager.h"
USING(Engine)

CComponent::CComponent(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
	, m_isClone(false)
	, m_pTimer(CTimer_Manager::GetInstance()->Get_MainGameTimer())
{
	m_pGraphicDev->AddRef();

#ifndef _FINAL
	Tag(L"CComponent", std::bind(&CComponent::Debug_Render, this));
#endif
}

CComponent::CComponent(const CComponent & rhs)
	: m_pGraphicDev(rhs.m_pGraphicDev)
	, m_isClone(true)
	, m_pTimer(CTimer_Manager::GetInstance()->Get_MainGameTimer())
{
	m_pGraphicDev->AddRef();
#ifndef _FINAL
	Tag(L"CComponent", std::bind(&CComponent::Debug_Render, this));
#endif
}

CComponent::~CComponent()
{
}
#ifndef _FINAL
void CComponent::Debug_Render()
{
	CDebug::GetInstance()->Add_DebugText("CComponent");
	CDebug::GetInstance()->Add_DebugText("Key :%d", GetKey());
	if (m_isClone)
		CDebug::GetInstance()->Add_DebugText("m_isClone : true");
	else
		CDebug::GetInstance()->Add_DebugText("m_isClone : false");

}
#endif
_ulong CComponent::Free(void)
{
	_ulong dwRefCnt = 0;

	Engine::Safe_Release(m_pGraphicDev);

	return dwRefCnt;	
}
