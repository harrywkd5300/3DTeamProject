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

class CUI : public Engine::CGameObject
{
public:
enum SHADERTYPE { TYPE_DEFAULT, TYPE_ALPHA, TYPE_ANIMATION, TYPE_GRADALPHA, TYPE_BORDER ,TYPE_COLOR,TYPE_DEPTH, TYPE_DEPTHANI};

enum RENDERSORT { RENDER_LOWEST,RENDER_ABOVE_LOWEST,RENDER_RELATIONLESS,RENDER_LOWERTHAN_TOP,RENDER_TOP,RENDER_PAUSE,RENDER_FADE,RENDER_LOADING};

protected:
	explicit CUI(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CUI() = default;
public:
	virtual HRESULT Ready_Component(void);
	virtual HRESULT Ready_GameObject(void) ;
	virtual _int Update_GameObject(const _float& fTimeDelta) ;
	virtual void Render_GameObject(void) ;
protected:
	//Engine::CTransform*				m_pTransformCom = nullptr;
	Engine::CRenderer*				m_pRendererCom = nullptr;
	Engine::CRect_Texture*			m_pBufferCom = nullptr;
#ifndef _FINAL
public:
	void Debug_Render();
#endif

protected:
	virtual _ulong Free(void);

};

