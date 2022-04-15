// Jangwon
#pragma once

#include "Engine_Defines.h"
#include "gameObject.h"


BEGIN(Engine)
class CTransform;
class CRenderer;
class DLL_EXPORT CTrigger : public CGameObject
{
	typedef std::function<void()> Func;
public:
	//Ʈ���� �Ӽ�
	enum Type {
		�浹�Ĺ�����ı�, 
		�浹�Ĺ�����۵�, 
		�浹���۵����ı�, 
		�浹���۵�,
		�Ӽ�����};

protected:
	explicit CTrigger(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CTrigger() = default;

public:
	virtual HRESULT Ready_GameObject(Func pFunc, CTrigger::Type eType);
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void	Render_GameObject();

/*MEMBER*/
protected:
	//CTransform*		m_pTransformCom = nullptr;
	CRenderer*		m_pRendererCom = nullptr;

	Func			m_pFunc = nullptr;
	CTrigger::Type	m_eType = CTrigger::Type::�Ӽ�����;
	bool			m_bCollOut = false;
	bool			m_bColl = false;
	bool			m_bHide = false;
	
public:
	FORCEINLINE	CTransform* Get_Transform() { return m_pTransformCom; }
	FORCEINLINE void Set_Hide(bool bHide) { m_bHide = bHide; }
public:
	static CTrigger* Create(LPDIRECT3DDEVICE9 pGraphicDev, Func pFunc, CTrigger::Type eType);
#ifndef _FINAL
public:
	void Debug_Render();
#endif
protected:
	virtual _ulong	Free() override;
};

END