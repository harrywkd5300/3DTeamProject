#pragma once

#include <tchar.h>
//#include <D3D9.h>
#include "engine_defines.h"
#include "base.h"

// Font creation flags
#define D3DFONT_BOLD        0x0001
#define D3DFONT_ITALIC      0x0002
#define D3DFONT_ZENABLE     0x0004

// Font rendering flags
#define D3DFONT_CENTERED    0x0001
#define D3DFONT_TWOSIDED    0x0002
#define D3DFONT_FILTERED    0x0004

BEGIN(Engine)

class DLL_EXPORT CD3DFont
	:public CBase
{
/*ctor dtor*/
private:
	explicit CD3DFont(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CD3DFont() = default;


/*FUNCTION*/
public:
	void	Set_FontName(const TCHAR* strFontName);
public:
	HRESULT Ready_D3dFont(const TCHAR* strFontName, DWORD dwHeight, DWORD dwFlags = 0L);

    // 2D and 3D text drawing functions
    HRESULT DrawText( FLOAT x, FLOAT y, DWORD dwColor, const char* strText, DWORD dwFlags=0L );
    HRESULT DrawTextScaled( FLOAT x, FLOAT y, FLOAT z, FLOAT fXScale, FLOAT fYScale, DWORD dwColor,  const TCHAR* strText, DWORD dwFlags=0L );
    HRESULT Render3DText( const TCHAR* strText, DWORD dwFlags=0L );
    

private:
	HRESULT Get_TextExtent(const TCHAR* strText, SIZE* pSize);
	HRESULT InitDeviceObjects();
	HRESULT RestoreDeviceObjects();


/*MEMBER*/
private:
	LPDIRECT3DDEVICE9       m_pGraphicDev = nullptr; // A D3DDevice used for rendering
	LPDIRECT3DTEXTURE9      m_pTexture = nullptr;   // The d3d texture for this font
	LPDIRECT3DVERTEXBUFFER9 m_pVB = nullptr;        // VertexBuffer for rendering text
	
	TCHAR   m_strFontName[80] = {};            // Font properties
	DWORD   m_dwFontHeight = 0;
	DWORD   m_dwFontFlags = 0;

	DWORD   m_dwTexWidth = 0;                 // Texture dimensions
	DWORD   m_dwTexHeight = 0;
	FLOAT   m_fTextScale = 0.f;
	FLOAT   m_fTexCoords[128 - 32][4] = { 0 };

	// Stateblocks for setting and restoring render states
	LPDIRECT3DSTATEBLOCK9 m_pStateBlockSaved;
	LPDIRECT3DSTATEBLOCK9 m_pStateBlockDrawText;

/*related ctor dtor*/
public:
	static CD3DFont*	Create(LPDIRECT3DDEVICE9 pGraphicDev, const TCHAR* strFontName, DWORD dwHeight, DWORD dwFlags = 0L);

private:
	virtual _ulong Free() final;
};

END
