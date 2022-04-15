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
	//트리거 속성
	enum Type {
		충돌후벗어나면파괴, 
		충돌후벗어나면작동, 
		충돌시작동후파괴, 
		충돌시작동,
		속성없음};

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
	CTrigger::Type	m_eType = CTrigger::Type::속성없음;
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