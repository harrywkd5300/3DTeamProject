#pragma once

#pragma once

#include "UI.h"

namespace Engine
{
	class CTexture;
}

class CUIObserver;
class CMyFont;

class CInventory : public CUI
{
private:
	explicit CInventory(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CInventory() = default;


public:
	HRESULT Ready_Observer(void);
	virtual HRESULT Ready_Component(void);
	virtual HRESULT Ready_GameObject(void);
	virtual _int Update_GameObject(const _float& fTimeDelta);
	virtual void Render_GameObject(void);

private:
	CUIObserver*				m_pObserver = nullptr;


public:
	static CInventory* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual _ulong Free(void) final;

};

