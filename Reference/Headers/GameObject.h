#pragma once

#include "Engine_Defines.h"
#include "Base.h"
#include "Component.h"
#include "Timer.h"

BEGIN( Engine )
//class CTimer;
//class CComponent;
class CCollider;
class CTransform;
class DLL_EXPORT CGameObject : public CBase
{
protected:
	explicit CGameObject(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CGameObject() = default;
public:
	Engine::CComponent*		Get_Component(const _tchar* pComTag);
	FORCEINLINE void		Set_IsDead(_bool bIsDead = true) { 
#ifndef _FINAL
		//this->Set_LeafNode();
		//CDebug::GetInstance()->Add_ConsoleDebugText(Get_LeafTagName());
#endif
		m_bIsDead = bIsDead; 
	}
	FORCEINLINE CTransform*	Get_Transform() { return m_pTransformCom; }
public:
	virtual HRESULT Ready_GameObject(void);
	virtual _int Update_GameObject(const _float& fTimeDelta);
	virtual _int LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void Render_GameObject(void);
	virtual void Render_Shadow(LPD3DXEFFECT pEffect, const char* pConstantWorldMatrixName) {}
	//////////////////////////////////////////////////////////////////////////////////////////////
	// OnCollision : 충돌 시 호출 ( Collision 구조체 안에 충돌에 관련된 데이터가 있음 )..
	// OnCollisionExit : 충돌이 끝났을 때 호출함????(아직 안함)..
	// _pCollider : 나의 콜리더..
	// _pOtherCollider : 상대의 콜리더..
	//////////////////////////////////////////////////////////////////////////////////////////////
	virtual void OnCollision( const Engine::Collision& _Collision ) {}
	virtual void OnCollisionExit( void ) {}
protected:
	LPDIRECT3DDEVICE9	m_pGraphicDev = nullptr;
	CTransform*			m_pTransformCom = nullptr;

protected:
	map<const _tchar*, CComponent*>			m_mapComponent[Engine::CComponent::TYPE_END];
	typedef map<const _tchar*, CComponent*>	MAPCOMPONENT;
protected:
	_bool				m_bIsDead = false;
	const CTimer*		m_pTimer = nullptr;
	_bool				m_bIsRender = true;
	//_float				m_fGameTimeDelta = 0.f;	// 현재 프레임 타임 델타
public:
	FORCEINLINE _bool				Get_IsDead() { return m_bIsDead; }
	FORCEINLINE _bool*				Get_IsRender_Pointer() { return &m_bIsRender; }
	FORCEINLINE _bool				Get_IsRender() { return m_bIsRender; }
	//FORCEINLINE	void Set_TimeDelta(_float fTimeDelta) { m_fGameTimeDelta = fTimeDelta; }

private:
	CComponent* Find_Component(const _tchar* pComTag);
#ifndef _FINAL
public:
	void Debug_Render();
#endif
protected:
	virtual _ulong Free(void) override;
};

END