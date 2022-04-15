#pragma once

#include "Defines.h"
#include "GameObject.h"

namespace Engine
{
	class CTransform;
	class CRect_Texture;
	class CRenderer;
	class CShader;
	class CTexture;
}

class DLL_EXPORT CEffect_Buffer final : public Engine::CGameObject
{
private:
	explicit CEffect_Buffer(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CEffect_Buffer() = default;
public:
	virtual HRESULT		Ready_GameObject(void) final;
	virtual _int		Update_GameObject(const _float& fTimeDelta) final;
	virtual void		Render_GameObject(void) final;
private:
	//Engine::CTransform*				m_pTransformCom = nullptr;	
	Engine::CRect_Texture*			m_pBufferCom = nullptr; 
	Engine::CRenderer*				m_pRendererCom = nullptr;
	Engine::CShader*				m_pShaderCom = nullptr;
	Engine::CTexture*				m_pTextureCom [5];	
private:
	_float		m_fUVSpeed = 0.f;
	_float		m_fTimeDelta = 0.f;
	_float		m_fUVScale [5];

	_uint		m_iTexCnt = 0;

	_vec3		m_vPos = _vec3(0.f, 0.f, 0.f);

private:
	HRESULT		Ready_Component(void);	
	HRESULT		SetUp_OnConstantTable(LPD3DXEFFECT pEffect);
public:
	// ���� �� �ʿ��� ���� - ScrollDir / SpriteSpeed / ���� �ؽ��� ����(��� �Ǵ� ������Ʈ �̸�) �� ����?
	static CEffect_Buffer* Create(LPDIRECT3DDEVICE9 pGraphicDev);
#ifndef _FINAL
public:
	void Debug_Render();
#endif

protected:
	virtual _ulong Free(void) final;

};

