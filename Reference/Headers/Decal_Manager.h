#pragma once
#include "Engine_Defines.h"
#include "Base.h"
#include "SSDecal.h"

BEGIN( Engine )

class CShader;
class DLL_EXPORT CDecal_Manager final
	: public CBase
{
	DECLARE_SINGLETON(CDecal_Manager)
private:
	typedef struct tagSSDecalArrayData
	{
		CSSDecal**			ppSSDecal = nullptr;
		_uint				iSSDecalCount = 0;

		HRESULT Add_SSDecal( CSSDecal* _pSSDecal )
		{
			if ( nullptr == _pSSDecal )
				return E_FAIL;

			// 현재 담겨 있는 Decal 을 옮기는 작업..
			CSSDecal** ppTemp = new CSSDecal*[iSSDecalCount + 1];

			for ( _uint i = 0; i < iSSDecalCount; ++i )
			{
				ppTemp[i] = ppSSDecal[i];
			}

			// Add 할 Decal 을 추가..
			ppTemp[iSSDecalCount] = _pSSDecal;

			Engine::Safe_Delete_Array( ppSSDecal );

			ppSSDecal = ppTemp;

			++iSSDecalCount;

			return NOERROR;
		}

	} SSDECALARRAYDATA;
private:
	explicit CDecal_Manager();
	virtual ~CDecal_Manager() = default;
public:
	void Set_ShaderCom( Engine::CShader* _pShaderCom );
public:
	_uint Get_SSDecalCount( void );
	CSSDecal* Get_SSDecal( const _uint& _iIndex );
public:
	HRESULT Add_SSDecal( const SSDECALCREATEDATA& _SSDecalCreateData );
	void Render_SSDecal( void );
#ifdef SSD_DEBUG
	void Render_SSDecalDebug( const _uint& _iType );
#endif
public:
	HRESULT Ready_Decal_Manager( LPDIRECT3DDEVICE9 _pGraphicDev );
private:
	LPDIRECT3DDEVICE9	m_pGraphicDev = nullptr;
	SSDECALARRAYDATA	m_SSDecalInfo;
	Engine::CShader*	m_pShaderCom = nullptr;
private:
	virtual _ulong Free( void );
};

END