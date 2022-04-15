#pragma once

#include "Defines.h"
#include "GameObject.h"

namespace Engine
{
	class CTransform;
	class CRect_Texture;
	class CRenderer;
	class CTexture;
	class CShader;
	class CCamera_Manager;
}

class CEffect_Distort final : public Engine::CGameObject
{
private:
	explicit CEffect_Distort(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CEffect_Distort() = default;
public:
	virtual HRESULT		Ready_GameObject(void) final;
	virtual _int		Update_GameObject(const _float& fTimeDelta) final;
	virtual void		Render_GameObject(void) final;
private:
	//Engine::CTransform*				m_pTransformCom = nullptr;	
	Engine::CRect_Texture*			m_pBufferCom = nullptr; 
	Engine::CRenderer*				m_pRendererCom = nullptr;
	Engine::CTexture*				m_pTextureCom = nullptr;	
	Engine::CShader*				m_pShaderCom = nullptr;

	Engine::CCamera_Manager*		m_pCam_Manager = nullptr;

private:
	_vec3		m_vPos = _vec3(0.f, 0.f, 0.f);

	_float		m_fTimeDelta = 0.f;

private:
	HRESULT		Ready_Component(void);	
	HRESULT		SetUp_OnConstantTable(LPD3DXEFFECT pEffect);
public:
	// 생성 시 필요한 변수 - FrameNum / SpriteSpeed / SliceCnt
	static CEffect_Distort* Create(LPDIRECT3DDEVICE9 pGraphicDev);
#ifndef _FINAL
public:
	void Debug_Render();
#endif
protected:
	virtual _ulong Free(void) final;

};

