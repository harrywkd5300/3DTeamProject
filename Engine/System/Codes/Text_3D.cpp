#define STRICT
#include <stdio.h>
#include <tchar.h>
#include <stdarg.h>
#include <string.h>

#include <D3DX9.h>
#include "Text_3D.h"


#define MAX_NUM_VERTICES 50*6

struct FONT2DVERTEX { D3DXVECTOR4 p;   DWORD color;     FLOAT tu, tv; };
struct FONT3DVERTEX { D3DXVECTOR3 p;   D3DXVECTOR3 n;   FLOAT tu, tv; };

#define D3DFVF_FONT2DVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)
#define D3DFVF_FONT3DVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1)

inline FONT2DVERTEX InitFont2DVertex(const D3DXVECTOR4& p, D3DCOLOR color,FLOAT tu, FLOAT tv)
{
	FONT2DVERTEX v;   v.p = p;   v.color = color;   v.tu = tu;   v.tv = tv;
	return v;
}

inline FONT3DVERTEX InitFont3DVertex(const D3DXVECTOR3& p, const D3DXVECTOR3& n,FLOAT tu, FLOAT tv)
{
	FONT3DVERTEX v;   v.p = p;   v.n = n;   v.tu = tu;   v.tv = tv;
	return v;
}

C3D_Text::C3D_Text(LPDIRECT3DDEVICE9 pGraphicDev)
	:m_pGraphicDev(pGraphicDev)
	,m_dwTexWidth(0),m_dwTexHeight(0),m_fTextScale(0.f)
{
	m_pGraphicDev->AddRef();
}

HRESULT C3D_Text::RestoreDeviceObjects()
{
    HRESULT hr;

    // Create vertex buffer for the letters
    int vertexSize = max( sizeof(FONT2DVERTEX), sizeof(FONT3DVERTEX ) );
    if( FAILED( hr = m_pGraphicDev->CreateVertexBuffer( MAX_NUM_VERTICES * vertexSize, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 0, D3DPOOL_DEFAULT, &m_pVB, 0 ) ) )
        return hr;

    // Create the state blocks for rendering text
    for( UINT which=0; which<2; which++ )
    {
        m_pGraphicDev->BeginStateBlock();
        m_pGraphicDev->SetTexture( 0, m_pTexture );

        if ( D3DFONT_ZENABLE & m_dwFontFlags )
            m_pGraphicDev->SetRenderState( D3DRS_ZENABLE, TRUE );
        else
            m_pGraphicDev->SetRenderState( D3DRS_ZENABLE, FALSE );

        m_pGraphicDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
        m_pGraphicDev->SetRenderState( D3DRS_SRCBLEND,   D3DBLEND_SRCALPHA );
        m_pGraphicDev->SetRenderState( D3DRS_DESTBLEND,  D3DBLEND_INVSRCALPHA );
        m_pGraphicDev->SetRenderState( D3DRS_ALPHATESTENABLE,  TRUE );
        m_pGraphicDev->SetRenderState( D3DRS_ALPHAREF,         0x08 );
        m_pGraphicDev->SetRenderState( D3DRS_ALPHAFUNC,  D3DCMP_GREATEREQUAL );
        m_pGraphicDev->SetRenderState( D3DRS_FILLMODE,   D3DFILL_SOLID );
        m_pGraphicDev->SetRenderState( D3DRS_CULLMODE,   D3DCULL_CCW );
        m_pGraphicDev->SetRenderState( D3DRS_STENCILENABLE,    FALSE );
        m_pGraphicDev->SetRenderState( D3DRS_CLIPPING,         TRUE );
        m_pGraphicDev->SetRenderState( D3DRS_CLIPPLANEENABLE,  FALSE );
        m_pGraphicDev->SetRenderState( D3DRS_VERTEXBLEND,      D3DVBF_DISABLE );
        m_pGraphicDev->SetRenderState( D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE );
        m_pGraphicDev->SetRenderState( D3DRS_FOGENABLE,        FALSE );
        m_pGraphicDev->SetRenderState( D3DRS_COLORWRITEENABLE,
            D3DCOLORWRITEENABLE_RED  | D3DCOLORWRITEENABLE_GREEN |
            D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA );
        m_pGraphicDev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
        m_pGraphicDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
        m_pGraphicDev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
        m_pGraphicDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
        m_pGraphicDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
        m_pGraphicDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
        m_pGraphicDev->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
        m_pGraphicDev->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
        m_pGraphicDev->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
        m_pGraphicDev->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
        m_pGraphicDev->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
        m_pGraphicDev->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
        m_pGraphicDev->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );

        if( which==0 )
            m_pGraphicDev->EndStateBlock( &m_pStateBlockSaved );
        else
            m_pGraphicDev->EndStateBlock( &m_pStateBlockDrawText );
    }

    return S_OK;
}


void C3D_Text::Set_FontName(const TCHAR * strFontName)
{
	_tcsncpy_s(m_strFontName, strFontName, sizeof(m_strFontName) / sizeof(TCHAR));
	m_strFontName[sizeof(m_strFontName) / sizeof(TCHAR) - 1] = _T('\0');
}

HRESULT C3D_Text::Ready_D3dFont(const TCHAR* strFontName, DWORD dwHeight, DWORD dwFlags)
{
	_tcsncpy_s(m_strFontName, strFontName, sizeof(m_strFontName) / sizeof(TCHAR));
	m_strFontName[sizeof(m_strFontName) / sizeof(TCHAR) - 1] = _T('\0');
	m_dwFontHeight = dwHeight;
	m_dwFontFlags = dwFlags;

	if (FAILED(InitDeviceObjects()))
		goto exception;
	
	if (FAILED(RestoreDeviceObjects()))
		goto exception;

	return NOERROR;

exception:
	MSG_BOX("Ready_D3dfont failed");
	return E_FAIL;
}

HRESULT C3D_Text::DebugText(FLOAT sx, FLOAT sy, DWORD dwColor, const char * strTextFormat, ...)
{
	if (m_pGraphicDev == NULL)
		return E_FAIL;

	DWORD dwFlags = 0L;

	va_list ap;
	va_start(ap, strTextFormat);
	vsprintf_s(m_buf, 512, strTextFormat, ap);
	va_end(ap);
	puts(m_buf);

	const char* strText = m_buf;

	// Setup renderstate
	m_pStateBlockSaved->Capture();
	m_pStateBlockDrawText->Apply();
	m_pGraphicDev->SetFVF(D3DFVF_FONT2DVERTEX);
	m_pGraphicDev->SetPixelShader(NULL);
	m_pGraphicDev->SetStreamSource(0, m_pVB, 0, sizeof(FONT2DVERTEX));

	// Set filter states
	if (dwFlags & D3DFONT_FILTERED)
	{
		m_pGraphicDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		m_pGraphicDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	}

	FLOAT fStartX = sx;

	// Fill vertex buffer
	FONT2DVERTEX* pVertices = NULL;
	DWORD         dwNumTriangles = 0;
	m_pVB->Lock(0, 0, (void**)&pVertices, D3DLOCK_DISCARD);

	while (*strText)
	{
		char c = *strText++;

		if (c == _T('\n'))
		{
			sx = fStartX;
			sy += (m_fTexCoords[0][3] - m_fTexCoords[0][1])*m_dwTexHeight;
		}

		if ((c - 32) < 0 || (c - 32) >= 128 - 32)
			continue;

		FLOAT tx1 = m_fTexCoords[c - 32][0];
		FLOAT ty1 = m_fTexCoords[c - 32][1];
		FLOAT tx2 = m_fTexCoords[c - 32][2];
		FLOAT ty2 = m_fTexCoords[c - 32][3];

		FLOAT w = (tx2 - tx1) *  m_dwTexWidth / m_fTextScale;
		FLOAT h = (ty2 - ty1) * m_dwTexHeight / m_fTextScale;

		if (c != _T(' '))
		{
			*pVertices++ = InitFont2DVertex(D3DXVECTOR4(sx + 0 - 0.5f, sy + h - 0.5f, 0.9f, 1.0f), dwColor, tx1, ty2);
			*pVertices++ = InitFont2DVertex(D3DXVECTOR4(sx + 0 - 0.5f, sy + 0 - 0.5f, 0.9f, 1.0f), dwColor, tx1, ty1);
			*pVertices++ = InitFont2DVertex(D3DXVECTOR4(sx + w - 0.5f, sy + h - 0.5f, 0.9f, 1.0f), dwColor, tx2, ty2);
			*pVertices++ = InitFont2DVertex(D3DXVECTOR4(sx + w - 0.5f, sy + 0 - 0.5f, 0.9f, 1.0f), dwColor, tx2, ty1);
			*pVertices++ = InitFont2DVertex(D3DXVECTOR4(sx + w - 0.5f, sy + h - 0.5f, 0.9f, 1.0f), dwColor, tx2, ty2);
			*pVertices++ = InitFont2DVertex(D3DXVECTOR4(sx + 0 - 0.5f, sy + 0 - 0.5f, 0.9f, 1.0f), dwColor, tx1, ty1);
			dwNumTriangles += 2;

			if (dwNumTriangles * 3 > (MAX_NUM_VERTICES - 6))
			{
				// Unlock, render, and relock the vertex buffer
				m_pVB->Unlock();
				m_pGraphicDev->DrawPrimitive(D3DPT_TRIANGLELIST, 0, dwNumTriangles);
				pVertices = NULL;
				m_pVB->Lock(0, 0, (void**)&pVertices, D3DLOCK_DISCARD);
				dwNumTriangles = 0L;
			}
		}

		sx += w;
	}

	// Unlock and render the vertex buffer
	m_pVB->Unlock();
	if (dwNumTriangles > 0)
		m_pGraphicDev->DrawPrimitive(D3DPT_TRIANGLELIST, 0, dwNumTriangles);

	// Restore the modified renderstates
	m_pStateBlockSaved->Apply();

	return S_OK;
}

HRESULT C3D_Text::DrawText( FLOAT sx, FLOAT sy, DWORD dwColor, const char* strText, DWORD dwFlags )
{
    if( m_pGraphicDev == NULL )
        return E_FAIL;

    // Setup renderstate
    m_pStateBlockSaved->Capture();
    m_pStateBlockDrawText->Apply();
    m_pGraphicDev->SetFVF( D3DFVF_FONT2DVERTEX );
    m_pGraphicDev->SetPixelShader( NULL );
    m_pGraphicDev->SetStreamSource( 0, m_pVB, 0, sizeof(FONT2DVERTEX) );

    // Set filter states
    if( dwFlags & D3DFONT_FILTERED )
    {
        m_pGraphicDev->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
        m_pGraphicDev->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
    }

    FLOAT fStartX = sx;

    // Fill vertex buffer
    FONT2DVERTEX* pVertices = NULL;
    DWORD         dwNumTriangles = 0;
    m_pVB->Lock( 0, 0, (void**)&pVertices, D3DLOCK_DISCARD );

    while( *strText )
    {
        char c = *strText++;

        if( c == _T('\n') )
        {
            sx = fStartX;
            sy += (m_fTexCoords[0][3]-m_fTexCoords[0][1])*m_dwTexHeight;
        }

        if( (c-32) < 0 || (c-32) >= 128-32 )
            continue;

        FLOAT tx1 = m_fTexCoords[c-32][0];
        FLOAT ty1 = m_fTexCoords[c-32][1];
        FLOAT tx2 = m_fTexCoords[c-32][2];
        FLOAT ty2 = m_fTexCoords[c-32][3];

        FLOAT w = (tx2-tx1) *  m_dwTexWidth / m_fTextScale;
        FLOAT h = (ty2-ty1) * m_dwTexHeight / m_fTextScale;

        if( c != _T(' ') )
        {
            *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+0-0.5f,sy+h-0.5f,0.9f,1.0f), dwColor, tx1, ty2 );
            *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+0-0.5f,sy+0-0.5f,0.9f,1.0f), dwColor, tx1, ty1 );
            *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+w-0.5f,sy+h-0.5f,0.9f,1.0f), dwColor, tx2, ty2 );
            *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+w-0.5f,sy+0-0.5f,0.9f,1.0f), dwColor, tx2, ty1 );
            *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+w-0.5f,sy+h-0.5f,0.9f,1.0f), dwColor, tx2, ty2 );
            *pVertices++ = InitFont2DVertex( D3DXVECTOR4(sx+0-0.5f,sy+0-0.5f,0.9f,1.0f), dwColor, tx1, ty1 );
            dwNumTriangles += 2;

            if( dwNumTriangles*3 > (MAX_NUM_VERTICES-6) )
            {
                // Unlock, render, and relock the vertex buffer
                m_pVB->Unlock();
                m_pGraphicDev->DrawPrimitive( D3DPT_TRIANGLELIST, 0, dwNumTriangles );
                pVertices = NULL;
                m_pVB->Lock( 0, 0, (void**)&pVertices, D3DLOCK_DISCARD );
                dwNumTriangles = 0L;
            }
        }

        sx += w;
    }

    // Unlock and render the vertex buffer
    m_pVB->Unlock();
    if( dwNumTriangles > 0 )
        m_pGraphicDev->DrawPrimitive( D3DPT_TRIANGLELIST, 0, dwNumTriangles );

    // Restore the modified renderstates
    m_pStateBlockSaved->Apply();

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: DrawTextScaled()
// Desc: Draws scaled 2D text.  Note that x and y are in viewport coordinates
//       (ranging from -1 to +1).  fXScale and fYScale are the size fraction 
//       relative to the entire viewport.  For example, a fXScale of 0.25 is
//       1/8th of the screen width.  This allows you to output text at a fixed
//       fraction of the viewport, even if the screen or window size changes.
//-----------------------------------------------------------------------------
HRESULT C3D_Text::DrawTextScaled(FLOAT x, FLOAT y, FLOAT z,FLOAT fXScale, FLOAT fYScale, DWORD dwColor,const TCHAR* strText, DWORD dwFlags)
{
	if (m_pGraphicDev == NULL)
		return E_FAIL;

	// Set up renderstate
	m_pStateBlockSaved->Capture();
	m_pStateBlockDrawText->Apply();
	m_pGraphicDev->SetFVF(D3DFVF_FONT2DVERTEX);
	m_pGraphicDev->SetPixelShader(NULL);
	m_pGraphicDev->SetStreamSource(0, m_pVB, 0, sizeof(FONT2DVERTEX));

	// Set filter states
	if (dwFlags & D3DFONT_FILTERED)
	{
		m_pGraphicDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		m_pGraphicDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	}

	D3DVIEWPORT9 vp;
	m_pGraphicDev->GetViewport(&vp);
	FLOAT sx = (x + 1.0f)*vp.Width / 2;
	FLOAT sy = (y - 1.0f)*vp.Height / 2;
	FLOAT sz = z;
	FLOAT rhw = 1.0f;
	FLOAT fStartX = sx;

	FLOAT fLineHeight = (m_fTexCoords[0][3] - m_fTexCoords[0][1]) * m_dwTexHeight;

	// Fill vertex buffer
	FONT2DVERTEX* pVertices;
	DWORD         dwNumTriangles = 0L;
	m_pVB->Lock(0, 0, (void**)&pVertices, D3DLOCK_DISCARD);

	while (*strText)
	{
		TCHAR c = *strText++;

		if (c == _T('\n'))
		{
			sx = fStartX;
			sy += fYScale*vp.Height;
		}

		if ((c - 32) < 0 || (c - 32) >= 128 - 32)
			continue;

		FLOAT tx1 = m_fTexCoords[c - 32][0];
		FLOAT ty1 = m_fTexCoords[c - 32][1];
		FLOAT tx2 = m_fTexCoords[c - 32][2];
		FLOAT ty2 = m_fTexCoords[c - 32][3];

		FLOAT w = (tx2 - tx1)*m_dwTexWidth;
		FLOAT h = (ty2 - ty1)*m_dwTexHeight;

		w *= (fXScale*vp.Height) / fLineHeight;
		h *= (fYScale*vp.Height) / fLineHeight;

		if (c != _T(' '))
		{
			*pVertices++ = InitFont2DVertex(D3DXVECTOR4(sx + 0 - 0.5f, sy + h - 0.5f, sz, rhw), dwColor, tx1, ty2);
			*pVertices++ = InitFont2DVertex(D3DXVECTOR4(sx + 0 - 0.5f, sy + 0 - 0.5f, sz, rhw), dwColor, tx1, ty1);
			*pVertices++ = InitFont2DVertex(D3DXVECTOR4(sx + w - 0.5f, sy + h - 0.5f, sz, rhw), dwColor, tx2, ty2);
			*pVertices++ = InitFont2DVertex(D3DXVECTOR4(sx + w - 0.5f, sy + 0 - 0.5f, sz, rhw), dwColor, tx2, ty1);
			*pVertices++ = InitFont2DVertex(D3DXVECTOR4(sx + w - 0.5f, sy + h - 0.5f, sz, rhw), dwColor, tx2, ty2);
			*pVertices++ = InitFont2DVertex(D3DXVECTOR4(sx + 0 - 0.5f, sy + 0 - 0.5f, sz, rhw), dwColor, tx1, ty1);
			dwNumTriangles += 2;

			if (dwNumTriangles * 3 > (MAX_NUM_VERTICES - 6))
			{
				// Unlock, render, and relock the vertex buffer
				m_pVB->Unlock();
				m_pGraphicDev->DrawPrimitive(D3DPT_TRIANGLELIST, 0, dwNumTriangles);
				m_pVB->Lock(0, 0, (void**)&pVertices, D3DLOCK_DISCARD);
				dwNumTriangles = 0L;
			}
		}

		sx += w;
	}

	// Unlock and render the vertex buffer
	m_pVB->Unlock();
	if (dwNumTriangles > 0)
		m_pGraphicDev->DrawPrimitive(D3DPT_TRIANGLELIST, 0, dwNumTriangles);

	// Restore the modified renderstates
	m_pStateBlockSaved->Apply();

	return S_OK;
}

HRESULT C3D_Text::Draw_Text_3D( const TCHAR* strText, DWORD dwFlags )
{
    if( m_pGraphicDev == NULL )
        return E_FAIL;

    // Setup renderstate
    m_pStateBlockSaved->Capture();
    m_pStateBlockDrawText->Apply();
    m_pGraphicDev->SetFVF( D3DFVF_FONT3DVERTEX );
    m_pGraphicDev->SetPixelShader( NULL );
    m_pGraphicDev->SetStreamSource( 0, m_pVB, 0, sizeof(FONT3DVERTEX) );

    // Set filter states
    if( dwFlags & D3DFONT_FILTERED )
    {
        m_pGraphicDev->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
        m_pGraphicDev->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
    }

    // Position for each text element
    FLOAT x = 0.0f;
    FLOAT y = 0.0f;

    // Center the text block at the origin
    if( dwFlags & D3DFONT_CENTERED )
    {
        SIZE sz;
        Get_TextExtent( strText, &sz );
        x = -(((FLOAT)sz.cx)/10.0f)/2.0f;
        y = -(((FLOAT)sz.cy)/10.0f)/2.0f;
    }

    // Turn off culling for two-sided text
    if( dwFlags & D3DFONT_TWOSIDED )
        m_pGraphicDev->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

    FLOAT fStartX = x;
    TCHAR c;

    // Fill vertex buffer
    FONT3DVERTEX* pVertices;
    DWORD         dwNumTriangles = 0L;
    m_pVB->Lock( 0, 0, (void**)&pVertices, D3DLOCK_DISCARD );

    while( (c = *strText++) != 0 )
    {
        if( c == '\n' )
        {
            x = fStartX;
            y -= (m_fTexCoords[0][3]-m_fTexCoords[0][1])*m_dwTexHeight/10.0f;
        }

        if( (c-32) < 0 || (c-32) >= 128-32 )
            continue;

        FLOAT tx1 = m_fTexCoords[c-32][0];
        FLOAT ty1 = m_fTexCoords[c-32][1];
        FLOAT tx2 = m_fTexCoords[c-32][2];
        FLOAT ty2 = m_fTexCoords[c-32][3];

        FLOAT w = (tx2-tx1) * m_dwTexWidth  / ( 10.0f * m_fTextScale );
        FLOAT h = (ty2-ty1) * m_dwTexHeight / ( 10.0f * m_fTextScale );

        if( c != _T(' ') )
        {
            *pVertices++ = InitFont3DVertex( D3DXVECTOR3(x+0,y+0,0), D3DXVECTOR3(0,0,-1), tx1, ty2 );
            *pVertices++ = InitFont3DVertex( D3DXVECTOR3(x+0,y+h,0), D3DXVECTOR3(0,0,-1), tx1, ty1 );
            *pVertices++ = InitFont3DVertex( D3DXVECTOR3(x+w,y+0,0), D3DXVECTOR3(0,0,-1), tx2, ty2 );
            *pVertices++ = InitFont3DVertex( D3DXVECTOR3(x+w,y+h,0), D3DXVECTOR3(0,0,-1), tx2, ty1 );
            *pVertices++ = InitFont3DVertex( D3DXVECTOR3(x+w,y+0,0), D3DXVECTOR3(0,0,-1), tx2, ty2 );
            *pVertices++ = InitFont3DVertex( D3DXVECTOR3(x+0,y+h,0), D3DXVECTOR3(0,0,-1), tx1, ty1 );
            dwNumTriangles += 2;

            if( dwNumTriangles*3 > (MAX_NUM_VERTICES-6) )
            {
                // Unlock, render, and relock the vertex buffer
                m_pVB->Unlock();
                m_pGraphicDev->DrawPrimitive( D3DPT_TRIANGLELIST, 0, dwNumTriangles );
                m_pVB->Lock( 0, 0, (void**)&pVertices, D3DLOCK_DISCARD );
                dwNumTriangles = 0L;
            }
        }

        x += w;
    }

    // Unlock and render the vertex buffer
    m_pVB->Unlock();
    if( dwNumTriangles > 0 )
        m_pGraphicDev->DrawPrimitive( D3DPT_TRIANGLELIST, 0, dwNumTriangles );

    // Restore the modified renderstates
    m_pStateBlockSaved->Apply();

    return S_OK;
}

HRESULT C3D_Text::Draw_Text_3D_BillBoard(const TCHAR * strText, const _matrix & matWorld_Camera, const _vec3 & vScale, const _vec3 & vPosition, DWORD dwFlags)
{
	if (m_pGraphicDev == NULL)
		return E_FAIL;

	_matrix matWorld;
	D3DXMatrixIdentity(&matWorld);
	
	(_vec3&)matWorld.m[0] = (_vec3&)matWorld_Camera.m[0] * vScale.x;
	(_vec3&)matWorld.m[1] *= vScale.y;
	(_vec3&)matWorld.m[2] = (_vec3&)matWorld_Camera.m[0] * vScale.z;
	(_vec3&)matWorld.m[3] = vPosition;
	m_pGraphicDev->SetTransform(D3DTS_WORLD,&matWorld);

	// Setup renderstate
	m_pStateBlockSaved->Capture();
	m_pStateBlockDrawText->Apply();
	m_pGraphicDev->SetFVF(D3DFVF_FONT3DVERTEX);
	m_pGraphicDev->SetPixelShader(NULL);
	m_pGraphicDev->SetStreamSource(0, m_pVB, 0, sizeof(FONT3DVERTEX));

	// Set filter states
	if (dwFlags & D3DFONT_FILTERED)
	{
		m_pGraphicDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		m_pGraphicDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	}

	// Position for each text element
	FLOAT x = 0.0f;
	FLOAT y = 0.0f;

	// Center the text block at the origin
	if (dwFlags & D3DFONT_CENTERED)
	{
		SIZE sz;
		Get_TextExtent(strText, &sz);
		x = -(((FLOAT)sz.cx) / 10.0f) / 2.0f;
		y = -(((FLOAT)sz.cy) / 10.0f) / 2.0f;
	}

	// Turn off culling for two-sided text
	if (dwFlags & D3DFONT_TWOSIDED)
		m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	FLOAT fStartX = x;
	TCHAR c;

	// Fill vertex buffer
	FONT3DVERTEX* pVertices;
	DWORD         dwNumTriangles = 0L;
	m_pVB->Lock(0, 0, (void**)&pVertices, D3DLOCK_DISCARD);

	while ((c = *strText++) != 0)
	{
		if (c == '\n')
		{
			x = fStartX;
			y -= (m_fTexCoords[0][3] - m_fTexCoords[0][1])*m_dwTexHeight / 10.0f;
		}

		if ((c - 32) < 0 || (c - 32) >= 128 - 32)
			continue;

		FLOAT tx1 = m_fTexCoords[c - 32][0];
		FLOAT ty1 = m_fTexCoords[c - 32][1];
		FLOAT tx2 = m_fTexCoords[c - 32][2];
		FLOAT ty2 = m_fTexCoords[c - 32][3];

		FLOAT w = (tx2 - tx1) * m_dwTexWidth / (10.0f * m_fTextScale);
		FLOAT h = (ty2 - ty1) * m_dwTexHeight / (10.0f * m_fTextScale);

		if (c != _T(' '))
		{
			*pVertices++ = InitFont3DVertex(D3DXVECTOR3(x + 0, y + 0, 0), D3DXVECTOR3(0, 0, -1), tx1, ty2);
			*pVertices++ = InitFont3DVertex(D3DXVECTOR3(x + 0, y + h, 0), D3DXVECTOR3(0, 0, -1), tx1, ty1);
			*pVertices++ = InitFont3DVertex(D3DXVECTOR3(x + w, y + 0, 0), D3DXVECTOR3(0, 0, -1), tx2, ty2);
			*pVertices++ = InitFont3DVertex(D3DXVECTOR3(x + w, y + h, 0), D3DXVECTOR3(0, 0, -1), tx2, ty1);
			*pVertices++ = InitFont3DVertex(D3DXVECTOR3(x + w, y + 0, 0), D3DXVECTOR3(0, 0, -1), tx2, ty2);
			*pVertices++ = InitFont3DVertex(D3DXVECTOR3(x + 0, y + h, 0), D3DXVECTOR3(0, 0, -1), tx1, ty1);
			dwNumTriangles += 2;

			if (dwNumTriangles * 3 > (MAX_NUM_VERTICES - 6))
			{
				// Unlock, render, and relock the vertex buffer
				m_pVB->Unlock();
				m_pGraphicDev->DrawPrimitive(D3DPT_TRIANGLELIST, 0, dwNumTriangles);
				m_pVB->Lock(0, 0, (void**)&pVertices, D3DLOCK_DISCARD);
				dwNumTriangles = 0L;
			}
		}

		x += w;
	}

	// Unlock and render the vertex buffer
	m_pVB->Unlock();
	if (dwNumTriangles > 0)
		m_pGraphicDev->DrawPrimitive(D3DPT_TRIANGLELIST, 0, dwNumTriangles);

	// Restore the modified renderstates
	m_pStateBlockSaved->Apply();

	return S_OK;
}


HRESULT C3D_Text::Get_TextExtent(const TCHAR* strText, SIZE* pSize)
{
	if (NULL == strText || NULL == pSize)
		return E_FAIL;

	FLOAT fRowWidth = 0.0f;
	FLOAT fRowHeight = (m_fTexCoords[0][3] - m_fTexCoords[0][1])*m_dwTexHeight;
	FLOAT fWidth = 0.0f;
	FLOAT fHeight = fRowHeight;

	while (*strText)
	{
		TCHAR c = *strText++;

		if (c == _T('\n'))
		{
			fRowWidth = 0.0f;
			fHeight += fRowHeight;
		}

		if ((c - 32) < 0 || (c - 32) >= 128 - 32)
			continue;

		FLOAT tx1 = m_fTexCoords[c - 32][0];
		FLOAT tx2 = m_fTexCoords[c - 32][2];

		fRowWidth += (tx2 - tx1)*m_dwTexWidth;

		if (fRowWidth > fWidth)
			fWidth = fRowWidth;
	}

	pSize->cx = (int)fWidth;
	pSize->cy = (int)fHeight;

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: InitDeviceObjects()
// Desc: Initializes device-dependent objects, including the vertex buffer used
//       for rendering text and the texture map which stores the font image.
//-----------------------------------------------------------------------------
HRESULT C3D_Text::InitDeviceObjects()
{
	HRESULT hr;

	// Keep a local copy of the device
	// m_pGraphicDev = pd3dDevice;

	// Establish the font and texture size
	m_fTextScale = 1.0f; // Draw fonts into texture without scaling

						 // Large fonts need larger textures
	if (m_dwFontHeight > 40)
		m_dwTexWidth = m_dwTexHeight = 1024;
	else if (m_dwFontHeight > 20)
		m_dwTexWidth = m_dwTexHeight = 512;
	else
		m_dwTexWidth = m_dwTexHeight = 256;

	// If requested texture is too big, use a smaller texture and smaller font,
	// and scale up when rendering.
	D3DCAPS9 d3dCaps;
	m_pGraphicDev->GetDeviceCaps(&d3dCaps);

	if (m_dwTexWidth > d3dCaps.MaxTextureWidth)
	{
		m_fTextScale = (FLOAT)d3dCaps.MaxTextureWidth / (FLOAT)m_dwTexWidth;
		m_dwTexWidth = m_dwTexHeight = d3dCaps.MaxTextureWidth;
	}

	// Create a new texture for the font
	hr = m_pGraphicDev->CreateTexture(m_dwTexWidth, m_dwTexHeight, 1,
		0, D3DFMT_A4R4G4B4,
		D3DPOOL_MANAGED, &m_pTexture, 0);
	if (FAILED(hr))
		return hr;

	// Prepare to create a bitmap
	DWORD*      pBitmapBits;
	BITMAPINFO bmi;
	ZeroMemory(&bmi.bmiHeader, sizeof(BITMAPINFOHEADER));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = (int)m_dwTexWidth;
	bmi.bmiHeader.biHeight = -(int)m_dwTexHeight;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biBitCount = 32;

	// Create a DC and a bitmap for the font
	HDC     hDC = CreateCompatibleDC(NULL);
	HBITMAP hbmBitmap = CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS,
		(void**)&pBitmapBits, NULL, 0);
	SetMapMode(hDC, MM_TEXT);

	// Create a font.  By specifying ANTIALIASED_QUALITY, we might get an
	// antialiased font, but this is not guaranteed.
	INT nHeight = -MulDiv(m_dwFontHeight,
		(INT)(GetDeviceCaps(hDC, LOGPIXELSY) * m_fTextScale), 72);
	DWORD dwBold = (m_dwFontFlags&D3DFONT_BOLD) ? FW_BOLD : FW_NORMAL;
	DWORD dwItalic = (m_dwFontFlags&D3DFONT_ITALIC) ? TRUE : FALSE;
	HFONT hFont = CreateFont(nHeight, 0, 0, 0, dwBold, dwItalic,
		FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
		VARIABLE_PITCH, m_strFontName);
	if (NULL == hFont)
		return E_FAIL;

	SelectObject(hDC, hbmBitmap);
	SelectObject(hDC, hFont);

	// Set text properties
	SetTextColor(hDC, RGB(255, 255, 255));
	SetBkColor(hDC, 0x00000000);
	SetTextAlign(hDC, TA_TOP);

	// Loop through all printable character and output them to the bitmap..
	// Meanwhile, keep track of the corresponding tex coords for each character.
	DWORD x = 0;
	DWORD y = 0;
	TCHAR str[2] = _T("x");
	SIZE size;

	for (TCHAR c = 32; c < 127; c++)
	{
		str[0] = c;
		GetTextExtentPoint32(hDC, str, 1, &size);

		if ((DWORD)(x + size.cx + 1) > m_dwTexWidth)
		{
			x = 0;
			y += size.cy + 1;
		}

		ExtTextOut(hDC, x + 0, y + 0, ETO_OPAQUE, NULL, str, 1, NULL);

		m_fTexCoords[c - 32][0] = ((FLOAT)(x + 0)) / m_dwTexWidth;
		m_fTexCoords[c - 32][1] = ((FLOAT)(y + 0)) / m_dwTexHeight;
		m_fTexCoords[c - 32][2] = ((FLOAT)(x + 0 + size.cx)) / m_dwTexWidth;
		m_fTexCoords[c - 32][3] = ((FLOAT)(y + 0 + size.cy)) / m_dwTexHeight;

		x += size.cx + 1;
	}

	// Lock the surface and write the alpha values for the set pixels
	D3DLOCKED_RECT d3dlr;
	m_pTexture->LockRect(0, &d3dlr, 0, 0);
	BYTE* pDstRow = (BYTE*)d3dlr.pBits;
	WORD* pDst16;
	BYTE bAlpha; // 4-bit measure of pixel intensity

	for (y = 0; y < m_dwTexHeight; y++)
	{
		pDst16 = (WORD*)pDstRow;
		for (x = 0; x < m_dwTexWidth; x++)
		{
			bAlpha = (BYTE)((pBitmapBits[m_dwTexWidth*y + x] & 0xff) >> 4);
			if (bAlpha > 0)
			{
				*pDst16++ = (bAlpha << 12) | 0x0fff;
			}
			else
			{
				*pDst16++ = 0x0000;
			}
		}
		pDstRow += d3dlr.Pitch;
	}

	// Done updating texture, so clean up used objects
	m_pTexture->UnlockRect(0);
	DeleteObject(hbmBitmap);
	DeleteDC(hDC);
	DeleteObject(hFont);

	return S_OK;
}

C3D_Text * C3D_Text::Create(LPDIRECT3DDEVICE9 pGraphicDev, const TCHAR * strFontName, DWORD dwHeight, DWORD dwFlags)
{
	C3D_Text* pInstance = new C3D_Text(pGraphicDev);

	if (FAILED(pInstance->Ready_D3dFont(strFontName, dwHeight, dwFlags)))
	{
		MSG_BOX("d3dfont create failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

_ulong C3D_Text::Free()
{
	_ulong dwRefCount = 0;

	dwRefCount = Safe_Release(m_pVB);
	dwRefCount = Safe_Release(m_pStateBlockSaved);
	dwRefCount = Safe_Release(m_pStateBlockDrawText);
	dwRefCount = Safe_Release(m_pTexture);

	Safe_Release(m_pGraphicDev);

	return 0;
}


