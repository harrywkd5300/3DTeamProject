#pragma once
#include "GameObject.h"

BEGIN(Engine)

typedef struct tagSSDecalCreateData
{
	_vec3 vPosition = _vec3( 0.f, 0.f, 0.f );	// 위치 정보..
	_vec3 vRotationDegree = _vec3( 0.f, 0.f, 0.f );	// 회전 정보( Degree )..
	_vec3 vScale = _vec3( 0.f, 0.f, 0.f );		// 스케일 정보..
	_tchar szImageName[128] = L"";				// 텍스쳐 이름..
	_uint  iImageSceneIndex = 0;				// 이미지 씬 인덱스..

} SSDECALCREATEDATA;

class CTexture;
class CRenderer;
class CCube_Texture;
class CShader;
class CCamera_Manager;
class DLL_EXPORT CSSDecal final :
	public Engine::CGameObject
{
public:
	explicit CSSDecal( LPDIRECT3DDEVICE9 _pGraphicDev );
	virtual ~CSSDecal() = default;
public:
	HRESULT Ready_SSDecal( const SSDECALCREATEDATA& _SSDecalCreateData, Engine::CShader* _pShaderCom );
	_int Update_GameObject( const _float& _fTimeDelta ) override;
	_int LastUpdate_GameObject( const _float& _fTimeDelta ) override;
	void Render_GameObject( void ) override;
#ifdef SSD_DEBUG
	void Render_DebugDecal( const _uint& _iType );
#endif
private:
	Engine::CCamera_Manager*	m_pCam_Manager = nullptr;
private:
	Engine::CTexture*		m_pTextureCom = nullptr;
	Engine::CCube_Texture*	m_pBufferCom = nullptr;
	Engine::CRenderer*		m_pRendererCom = nullptr;
	Engine::CShader*		m_pShaderCom = nullptr;		// 쉐이더는 CSSDecal_Manager 매니저에서 받아올 예정..
private:
	LPD3DXEFFECT			m_pEffect = nullptr;
private:
	_matrix					m_matWorld;
	_matrix					m_matInvWorld;
private:
	HRESULT Ready_Component( const SSDECALCREATEDATA& _SSDecalCreateData, Engine::CShader* _pShaderCom );
	HRESULT Ready_DefaultSetting( const SSDECALCREATEDATA& _SSDecalCreateData );
private:
	HRESULT SetUp_OnConstantTable( LPD3DXEFFECT _pEffect );
public:
	static CSSDecal* Create( LPDIRECT3DDEVICE9 _pGraphicDev, const SSDECALCREATEDATA& _SSDecalCreateData, Engine::CShader* _pShaderCom );
private:
	virtual _ulong Free( void ) override;

};

END