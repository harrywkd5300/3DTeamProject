#include "..\Headers\Equipment.h"
#include "Component_Manager.h"
#include "Character.h"

USING(Engine)

CEquipment::CEquipment(LPDIRECT3DDEVICE9 pGraphicDev)
	:CGameObject(pGraphicDev),
	//m_pTransformCom(nullptr),
	m_pUser(nullptr),
	m_pBoneMatrix(nullptr)
	
{
#ifndef _FINAL
	Tag(L"CEquipment", std::bind(&CEquipment::Debug_Render, this));
#endif

}

_int CEquipment::Update_GameObject(const _float& fTimeDelta)
{
	if (m_bHide)
		return 0;
	
	// 장비 위치 갱신
	if (m_pBoneMatrix != nullptr
		&& m_pUser != nullptr)
	{
		CTransform* pTransform_User = m_pUser->Get_Transform();
		if (nullptr != pTransform_User)
			m_pTransformCom->Set_ParentMatrix(&(*m_pBoneMatrix * pTransform_User->Get_WorldMatrix()));
	}

	_int iReturn = CGameObject::Update_GameObject(fTimeDelta);


	return iReturn;
}

_int CEquipment::LastUpdate_GameObject(const _float & fTimeDelta)
{
	if (m_bHide)
		return 0;
	//// 장비 위치 갱신
	//if (m_pBoneMatrix != nullptr
	//	&& m_pUser != nullptr)
	//{
	//	CTransform* pTransform_User = m_pUser->Get_Transform();
	//	if (nullptr != pTransform_User)
	//		m_pTransformCom->Set_ParentMatrix(&(*m_pBoneMatrix * pTransform_User->Get_WorldMatrix()));
	//}

	_int iReturn = CGameObject::LastUpdate_GameObject(fTimeDelta);

	return iReturn;
}

#ifndef _FINAL
void CEquipment::Debug_Render(void)
{
	CDebug::GetInstance()->Add_DebugText("CEquipment");
	if (m_bHide)
		return;

}
#endif

void CEquipment::Set_User(CCharacter * pUser)
{
	if (pUser == nullptr)
		return; m_pUser = pUser; 

	//m_pUser->AddRef();
}

_ulong CEquipment::Free()
{
	//Safe_Release(m_pUser);
	//Safe_Release(m_pTransformCom);

	return  Engine::CGameObject::Free();
}
