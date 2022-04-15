#include "..\Headers\RectUI_Texture.h"

USING(Engine)

CRectUI_Texture::CRectUI_Texture(LPDIRECT3DDEVICE9 pGraphicDev)
	: CVIBuffer(pGraphicDev)
{
#ifndef _FINAL
	Tag(L"CRectUI_Texture", std::bind(&CRectUI_Texture::Debug_Render, this));
#endif

}
CRectUI_Texture::CRectUI_Texture(const CRectUI_Texture & rhs)
	: CVIBuffer(rhs)
{
#ifndef _FINAL
	Tag(L"CRectUI_Texture", std::bind(&CRectUI_Texture::Debug_Render, this));
#endif

}
CRectUI_Texture::~CRectUI_Texture()
{
}

HRESULT CRectUI_Texture::Ready_Buffer(void)
{
	m_iVertexSize = sizeof(VTXTEX_VIEWPORT);
	m_iVertexCnt = 4;
	m_dwVertexFVF = D3DFVF_XYZRHW | D3DFVF_TEX1;

	m_iIndexSize = sizeof(INDEX16);
	m_IndexFmt = D3DFMT_INDEX16;
	m_iTriCnt = 2;



	if (FAILED(CVIBuffer::Ready_Buffer()))
		return E_FAIL;

	if (nullptr == m_pVB)
		return E_FAIL;

	VTXTEX_VIEWPORT*			pVertex = nullptr;


	// Lock : 다른스레드가 접근할 수 없도록 막는다. (값을 갱신하는 중이니까)
	//      : 잠겨진 공간의 주소를 얻어온다.(값을 갱신하려고)
	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	/*pVertex[0].vPosition = _vec4(-0.5f, -0.5f, 0.0f, 1.f);
	pVertex[0].vTexUV = _vec2(0.0f, 0.f);

	pVertex[1].vPosition = _vec4(0.5f, -0.5f, 0.0f, 1.f);
	pVertex[1].vTexUV = _vec2(1.f, 0.f);

	pVertex[2].vPosition = _vec4(0.5f, 0.5f, 0.0f, 1.f);
	pVertex[2].vTexUV = _vec2(1.f, 1.f);

	pVertex[3].vPosition = _vec4(-0.5f, 0.5f, 0.0f, 1.f);
	pVertex[3].vTexUV = _vec2(0.f, 1.f);*/

	pVertex[0].vPosition = _vec4(-50.0f, -50.f, 0.0f, 1.f);
	pVertex[0].vTexUV = _vec2(0.0f, 0.f);

	pVertex[1].vPosition = _vec4(50.f, -50.f, 0.0f, 1.f);
	pVertex[1].vTexUV = _vec2(1.f, 0.f);

	pVertex[2].vPosition = _vec4(50.f, 50.f, 0.0f, 1.f);
	pVertex[2].vTexUV = _vec2(1.f, 1.f);

	pVertex[3].vPosition = _vec4(-50.f, 50.f, 0.0f, 1.f);
	pVertex[3].vTexUV = _vec2(0.f, 1.f);



	m_pVB->Unlock();

	INDEX16*			pIndex = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	pIndex[0]._0 = 0;
	pIndex[0]._1 = 1;
	pIndex[0]._2 = 2;

	pIndex[1]._0 = 0;
	pIndex[1]._1 = 2;
	pIndex[1]._2 = 3;

	m_pIB->Unlock();

	return NOERROR;
}

void CRectUI_Texture::Render_Buffer(void)
{
	CVIBuffer::Render_Buffer();
}

CComponent * CRectUI_Texture::Clone(void)
{
	return new CRectUI_Texture(*this);
}

CRectUI_Texture * CRectUI_Texture::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CRectUI_Texture *	pInstance = new CRectUI_Texture(pGraphicDev);

	if (FAILED(pInstance->Ready_Buffer()))
	{
		MessageBox(0, L"CRectUI_Texture Created Failed", nullptr, MB_OK);
		Engine::Safe_Release(pInstance);
	}

	return pInstance;
}
#ifndef _FINAL
void CRectUI_Texture::Debug_Render()
{
	CDebug::GetInstance()->Add_DebugText("CRectUI_Texture");

}
#endif
_ulong CRectUI_Texture::Free(void)
{
	return Engine::CVIBuffer::Free();	
}
