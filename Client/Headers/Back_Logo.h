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

class CMyFont;

class CBack_Logo final : public Engine::CGameObject
{
private:
	explicit CBack_Logo(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CBack_Logo() = default;

public:
	void Set_EnterKey(_bool bEnterKey) { m_bRenderEnterKey = bEnterKey; }

public:
	virtual HRESULT Ready_GameObject(void) final;
	virtual _int Update_GameObject(const _float& fTimeDelta) final;
	virtual void Render_GameObject(void) final;
		
private:
	void Set_AlphaMotion(const _float& fTimeDelta);
	void Render_Font();


public:
	HRESULT Ready_Component(void);
	HRESULT SetUp_OnContantTable(LPD3DXEFFECT pEffect);
	HRESULT SetUp_OnContantTable_TextureKey(LPD3DXEFFECT pEffect);
	HRESULT SetUp_OnContantTable_BackScreen(LPD3DXEFFECT pEffect);
	HRESULT SetUp_OnContantTable_ShuriKen(LPD3DXEFFECT pEffect);
	HRESULT SetUp_OnContantTable_BackShuriKen(LPD3DXEFFECT pEffect);

private:
	//Engine::CTransform*				m_pTransformCom = nullptr;
	Engine::CRect_Texture*			m_pBufferCom = nullptr;

	Engine::CRenderer*				m_pRendererCom = nullptr;
	Engine::CTexture*				m_pTextureCom = nullptr;
	Engine::CTexture*				m_pKeyTextureCom = nullptr;
	Engine::CTexture*				m_pBackTextureCom = nullptr;
	Engine::CTexture*				m_pShuriKenTextureCom = nullptr;
	Engine::CShader*				m_pShaderCom = nullptr;

	CMyFont*						m_pFont = nullptr;
	
private:
	_float			m_fX, m_fY;
	_float			m_fSizeX, m_fSizeY;

	_float			m_fKeyX, m_fKeyY;
	_float			m_fKeySizeX, m_fKeySizeY;

	_float			m_fBackX, m_fBackY;
	_float			m_fBackSizeX, m_fBackSizeY;

	_float			m_fShuriKenX, m_fShuriKenY;
	_float			m_fShuriKenSizeX, m_fShuriKenSizeY;
	_float			m_fShuriKenAlpha = 1.f;

	_float			m_fShuriKenAngle = 0.f;

	_bool			m_IsChangeAlpha = false;
	_float			m_fAlpha = 1.f;

	_bool			m_IsChangeFontAlpha = false;
	_float			m_fFontAlpha = 1.f;

	_bool			m_bRenderEnterKey = false;


public:
	static CBack_Logo* Create(LPDIRECT3DDEVICE9 pGraphicDev);
#ifndef _FINAL
public:
	void Debug_Render();
#endif
protected:
	virtual _ulong Free(void) final;

};

