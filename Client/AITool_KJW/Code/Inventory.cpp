#include "stdafx.h"
#include "..\Headers\Inventory.h"

#include "Component_Manager.h"
#include "Camera_Manager.h"

#include "Subject_Manager.h"
#include "UIObserver.h"
#include "Object_Manager.h"
#include "Player.h"

#include "Input_Device.h" // »èÁ¦
#include "MyFont.h"


CInventory::CInventory(LPDIRECT3DDEVICE9 pGraphicDev)
	:CUI(pGraphicDev)
{
}

HRESULT CInventory::Ready_Observer(void)
{
	m_pObserver = CUIObserver::Create();
	CSubject_Manager::GetInstance()->AddObserver(m_pObserver, CSubject_Manager::TYPE_STATIC);

	return NOERROR;
}

HRESULT CInventory::Ready_Component(void)
{

	return NOERROR;
}

HRESULT CInventory::Ready_GameObject(void)
{
	if (FAILED(CUI::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	if (FAILED(Ready_Observer()))
		return E_FAIL;
	
	return NOERROR;
}

_int CInventory::Update_GameObject(const _float & fTimeDelta)
{
	CUI::Update_GameObject(fTimeDelta);

	
	return 0;
}

void CInventory::Render_GameObject(void)
{
	
}

CInventory * CInventory::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CInventory *	pInstance = new CInventory(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MSG_BOX("CInventory Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}

_ulong CInventory::Free(void)
{	
	Safe_Release(m_pObserver);	
	return CUI::Free();
}
