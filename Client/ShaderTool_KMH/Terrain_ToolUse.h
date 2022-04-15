#pragma once
#include "Tool_Defines.h"
#include "GameObject.h"

namespace Engine
{
	class CTerrain_Buffer;
	class CRenderer;
	class CTexture;
	class CTransform;
	class CShader;
	class CFrustum;
}

class CTerrain_ToolUse final : public Engine::CGameObject
{
private:
	explicit CTerrain_ToolUse(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CTerrain_ToolUse() = default;
public:
	Engine::CTerrain_Buffer* Get_TerrainBuffer( void ) { return m_pBufferCom; }
public:
	virtual HRESULT Ready_GameObject(void) final;
	virtual _int Update_GameObject(const _float& fTimeDelta) final;
	virtual void Render_GameObject( void ) final;
private:
	Engine::CTransform*				m_pTransformCom = nullptr;
	Engine::CTerrain_Buffer*		m_pBufferCom = nullptr; 
	Engine::CRenderer*				m_pRendererCom = nullptr;
	Engine::CTexture*				m_pTextureCom = nullptr;
	Engine::CTexture*				m_pFilterCom = nullptr;
	Engine::CTexture*				m_pBrushCom = nullptr;
	Engine::CShader*				m_pShaderCom = nullptr;
	Engine::CFrustum*				m_pFrustumCom = nullptr;
private:
	LPDIRECT3DTEXTURE9				m_pFilterTexture = nullptr;
public:
	HRESULT Ready_Component(void);
	HRESULT SetUp_OnConstantTable(LPD3DXEFFECT pEffect);

public:
	static CTerrain_ToolUse* Create(LPDIRECT3DDEVICE9 pGraphicDev);
protected:
	virtual _ulong Free(void) final;

};

