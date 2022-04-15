#pragma once

#include "Defines.h"
#include "GameObject.h"

namespace Engine
{
	class CRenderer;
	class CTransform;		
	class CMesh_Static;
	class CCollider;
	class CVIBuffer;
	class CShader;
}

class CTree : public CGameObject
{
private:
	explicit CTree(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CTree() = default;
public:
	virtual HRESULT Ready_GameObject(void) final;
	virtual _int Update_GameObject(const _float& fTimeDelta) final;
	virtual void Render_GameObject(void) final;
private:
	//Engine::CTransform*				m_pTransformCom = nullptr;	
	Engine::CMesh_Static*			m_pMeshCom = nullptr;
	Engine::CRenderer*				m_pRendererCom = nullptr;
	Engine::CCollider*				m_pColliderCom = nullptr;
	Engine::CShader*				m_pShaderCom = nullptr;
private:
	Engine::CVIBuffer*				m_pTargetBufferCom = nullptr;
private:
	HRESULT Ready_Component(void);
	void Move_OnTerrain(void);
	HRESULT SetUp_ConstantTable(LPD3DXEFFECT pEffect);

public:
	static CTree* Create(LPDIRECT3DDEVICE9 pGraphicDev);
#ifndef _FINAL
public:
	void Debug_Render();
#endif
	virtual _ulong Free(void);
};

