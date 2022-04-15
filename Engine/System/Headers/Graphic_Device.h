#pragma once

#include "Engine_Defines.h"
#include "Base.h"
//#include "Text_3D.h"

BEGIN(Engine)

class DLL_EXPORT CGraphic_Device final : public CBase
{
	DECLARE_SINGLETON(CGraphic_Device)
public:
	enum WINMODE { MODE_FULL, MODE_WIN };
private:
	explicit CGraphic_Device();
	virtual ~CGraphic_Device()/* = default*/;
public:
	HRESULT		Ready_GraphicDev(LPDIRECT3DDEVICE9* ppGraphicDevPtr, HWND hWnd, WINMODE eMode, const _uint& iBackCX, const _uint& iBackCY);
	//C3D_Text*	Get_Font() { return m_pD3dFont; }
private:
	LPDIRECT3D9					m_pSDK = nullptr;
	LPDIRECT3DDEVICE9			m_pGraphicDev = nullptr;
	//C3D_Text*					m_pD3dFont = nullptr;
private:
	virtual _ulong Free(void) final;
};

END