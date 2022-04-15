#pragma once

#include "Defines.h"
#include "GameObject.h"

namespace Engine
{
	class CRect_Texture;
	class CRenderer;
	class CTexture;
	class CTransform;
	class CShader;
}

class CEffect_Sprite final : public Engine::CGameObject
{
private:
	explicit CEffect_Sprite(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CEffect_Sprite() = default;
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
private:
	_float		m_fCurFrame = 0.f;
	_float		m_fFrameNum = 0.f;
	_float		m_fSpriteSpeed = 0.f;

	_vec2		m_vSliceCnt = _vec2(0.f, 0.f);
	_vec2		m_vUVOffset = _vec2(0.f, 0.f);

private:
	HRESULT		Ready_Component(void);	
	HRESULT		SetUp_OnConstantTable(LPD3DXEFFECT pEffect);
public:
	// 생성 시 필요한 변수 - FrameNum / SpriteSpeed / SliceCnt
	static CEffect_Sprite* Create(LPDIRECT3DDEVICE9 pGraphicDev);
#ifndef _FINAL
public:
	void Debug_Render();
#endif
protected:
	virtual _ulong Free(void) final;

};

