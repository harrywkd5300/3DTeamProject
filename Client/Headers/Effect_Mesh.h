#pragma once

#include "Defines.h"
#include "GameObject.h"

namespace Engine
{
	class CTransform;
	class CMesh_Static;
	class CRenderer;
	class CShader;
	class CTexture;
	class CCamera_Manager;
}

class CEffect_Mesh final : public Engine::CGameObject
{
private:
	explicit CEffect_Mesh(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CEffect_Mesh() = default;
public:
	virtual HRESULT		Ready_GameObject(void) final;
	virtual _int		Update_GameObject(const _float& fTimeDelta) final;
	virtual void		Render_GameObject(void) final;
private:
	//Engine::CTransform*				m_pTransformCom = nullptr;	
	Engine::CRenderer*				m_pRendererCom = nullptr;
	Engine::CShader*				m_pShaderCom = nullptr;
	Engine::CMesh_Static*			m_pMeshCom = nullptr;
	Engine::CTexture*				m_pTextureCom = nullptr;	

	Engine::CCamera_Manager*		m_pCam_Manager = nullptr;

private:
	_float		m_fUVSpeed = 0.f;
	_float		m_fUVScale = 0.f;
	_float		m_fLifeTime = 0.f;
	_float		m_fCurTime = 0.f;

private:
	HRESULT		Ready_Component(void);	
	HRESULT		SetUp_OnConstantTable(LPD3DXEFFECT pEffect);
public:
	// ���� �� �ʿ��� ���� - ScrollDir / SpriteSpeed / ���� �ؽ��� ����(��� �Ǵ� ������Ʈ �̸�) �� ����?
	static CEffect_Mesh* Create(LPDIRECT3DDEVICE9 pGraphicDev);
#ifndef _FINAL
public:
	void Debug_Render();
#endif
protected:
	virtual _ulong Free(void) final;

};

