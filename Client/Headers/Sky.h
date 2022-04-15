#pragma once

#include "Defines.h"
#include "GameObject.h"

namespace Engine
{
	class CCube_Texture;
	class CRenderer;
	class CTexture;
	class CTransform;
	class CMaterial;
	class CShader;
}

class CSky final : public Engine::CGameObject
{
private:
	explicit CSky(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CSky() = default;
public:
	virtual HRESULT Ready_GameObject(void) final;
	virtual _int Update_GameObject(const _float& fTimeDelta) final;
	virtual _int LastUpdate_GameObject(const _float& fTimeDelta) final;
	virtual void Render_GameObject(void) final;
private:
	//Engine::CTransform*				m_pTransformCom = nullptr;
	Engine::CCube_Texture*			m_pBufferCom = nullptr;
	Engine::CRenderer*				m_pRendererCom = nullptr;
	Engine::CTexture*				m_pTextureCom = nullptr;
	Engine::CMaterial*				m_pMaterialCom = nullptr;
	Engine::CShader*				m_pShaderCom = nullptr;
public:
	HRESULT Ready_Component(void);
	HRESULT SetUp_OnContantTable(LPD3DXEFFECT pEffect);
public:
	static CSky* Create(LPDIRECT3DDEVICE9 pGraphicDev);
#ifndef _FINAL
public:
	void Debug_Render();
#endif
protected:
	virtual _ulong Free(void) final;

};

