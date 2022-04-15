// Jangwon
#pragma once

#include "Engine_Defines.h"
#include "gameObject.h"


BEGIN(Engine)
class CCharacter;
class CTransform;
class DLL_EXPORT CEquipment abstract : public CGameObject
{

protected:
	explicit CEquipment(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CEquipment() = default;

public:
	virtual HRESULT Ready_GameObject() PURE;
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void	Render_GameObject() PURE;

/*MEMBER*/
protected:
	//CTransform*		m_pTransformCom = nullptr;
	CCharacter*		m_pUser = nullptr;
	_matrix*		m_pBoneMatrix = nullptr;
	bool			m_bHide = false;

public:
	FORCEINLINE void Set_Bone(_matrix* pBoneMatrix) { if (pBoneMatrix == nullptr)return; m_pBoneMatrix = pBoneMatrix; }
	FORCEINLINE void Set_User(CCharacter* pUser);
	FORCEINLINE void Hide_Item(bool bHide) { m_bHide = bHide; }
#ifndef _FINAL
public:
	void Debug_Render();
#endif
protected:
	virtual _ulong	Free() override;
};

END