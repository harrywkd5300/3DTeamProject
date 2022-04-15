#include "..\Headers\Renderer.h"
#include "GameObject.h"
#include "Target_Manager.h"
#include "Light_Manager.h"
#include "Shader.h"
#include "Camera_Manager.h"
#include "Debug_Manager.h"
#include "Component_Manager.h"
#include "GlareDefD3D.h"
#include "Timer_Manager.h"
#include "Timer.h"
#include "Decal_Manager.h"


USING(Engine)

HRESULT GetSampleOffsets_DownScale4x4( DWORD dwWidth, DWORD dwHeight, D3DXVECTOR4 avSampleOffsets[] )
{
	if( NULL == avSampleOffsets )
		return E_INVALIDARG;

	float tU = 1.0f / dwWidth;
	float tV = 1.0f / dwHeight;

	// Sample from the 16 surrounding points. Since the center point will be in
	// the exact center of 16 texels, a 0.5f offset is needed to specify a texel
	// center.
	int index = 0;
	for( int y = 0; y < 4; y++ )
	{
		for( int x = 0; x < 4; x++ )
		{
			avSampleOffsets[ index ].x = ( x - 1.5f ) * tU;
			avSampleOffsets[ index ].y = ( y - 1.5f ) * tV;

			index++;
		}
	}

	return S_OK;
}

CRenderer::CRenderer( LPDIRECT3DDEVICE9 pGraphicDev )
	: CComponent( pGraphicDev )
	, m_pTarget_Manager( CTarget_Manager::GetInstance() )
#ifndef KMH_MODIFY // By.KMH
	, m_pShader_Light( nullptr )
	, m_pShader_Blend( nullptr )
#endif
	, m_pShader_Distort( nullptr ) // By.KJY
	, m_pDecal_Manager( CDecal_Manager::GetInstance() )
#ifdef KMH_MODIFY // By.KMH
	, m_pCam_Manager( CCamera_Manager::GetInstance() )
	, m_pLight_Manager( CLight_Manager::GetInstance() )
	, m_pInput_Device( CInput_Device::GetInstance() )
#endif
{
	m_pTarget_Manager->AddRef();
#ifndef _FINAL
	Tag(L"CRenderer", std::bind(&CRenderer::Debug_Render, this));
#endif

}

HRESULT CRenderer::Add_RenderGroup(RENDER eRenderGroup, CGameObject * pGameObject, const _float& fSort)
{
	if (eRenderGroup >= RENDER_END
		|| nullptr == pGameObject)
		return E_FAIL;

	REDNEROBJECT tObject;

	tObject.pGameObject = pGameObject;
	tObject.fSort = -fSort;

	m_RenderGroup[eRenderGroup].push_back(tObject);
	pGameObject->AddRef();

	return NOERROR;
}

HRESULT CRenderer::Add_StaticRenderObject( const STATICRENDER & eStaticRenderGroup, CGameObject * pGameObject, const _float & fSort )
{
	if ( eStaticRenderGroup >= STATICRENDER_END
		 || nullptr == pGameObject )
		return E_FAIL;

	REDNEROBJECT tObject;

	tObject.pGameObject = pGameObject;
	tObject.fSort = -fSort;

	STATICRENDERGROUP& CurGroup = m_StaticRenderGroup[eStaticRenderGroup];

	STATICRENDEROBJECT* pObject = new STATICRENDEROBJECT[CurGroup.iGroupCount + 1];
	for ( _uint i = 0; i < CurGroup.iGroupCount; --i )
	{
		pObject[i] = CurGroup.pGroup[i];
	}

	CurGroup.pGroup = pObject;
	++CurGroup.iGroupCount;

	pGameObject->AddRef();

	return NOERROR;
}

HRESULT CRenderer::Add_StaticRenderGroup( const STATICRENDER& eStaticRenderGroup, STATICRENDERGROUP& StaticGroupData )
{
	if ( eStaticRenderGroup >= STATICRENDER_END )
		return E_FAIL;

	STATICRENDERGROUP& CurGroup = m_StaticRenderGroup[eStaticRenderGroup];
	
	CurGroup = StaticGroupData;

	for ( _uint i = 0; i < CurGroup.iGroupCount; ++i )
	{
		if ( nullptr == CurGroup.pGroup[i].pGameObject )
			return E_FAIL;

		CurGroup.pGroup[i].pGameObject->AddRef();
		CurGroup.pGroup[i].pTransformCom = dynamic_cast<CTransform*>(CurGroup.pGroup[i].pGameObject->Get_Component( L"Com_Transform" ));

		if ( nullptr == CurGroup.pGroup[i].pTransformCom )
			return E_FAIL;

		CurGroup.pGroup[i].vPos = *CurGroup.pGroup[i].pTransformCom->Get_Infomation( Engine::CTransform::INFO_POSITION );
	}

	return NOERROR;
}

HRESULT CRenderer::Add_ShadowGroup( CGameObject * pGameObject, _vec3 _vPos, const _float& _fRadius )
{
	_uint iIndex = 0;

	for ( _uint i = 0; i < Cascade_End; ++i )
	{
		if ( -1 == Compute_CascadeIndex( _vPos, _fRadius ) )
			continue;
		if ( iIndex >= Cascade_End )
			continue;

		m_ShadowRenderGroup[iIndex].push_back( pGameObject );
		pGameObject->AddRef();
	}

	return NOERROR;
}

void CRenderer::Set_RenderBackBufferKind( const RENDERBACKBUFFERKIND & _eRenderTargetBackBuffer, const _tchar * _pRenderTargetTag )
{
	m_iPassNum = _eRenderTargetBackBuffer;
	m_pDebugRenderTargetTag = _pRenderTargetTag;
}

void CRenderer::Set_ShaderOptionOnOff( const SHADEROPTION & _eShaderOption, const _bool & _bIsOnShaderOption )
{
	if ( _eShaderOption <= ShaderOption_None || _eShaderOption >= ShaderOption_End )
		return;

	if ( true == _bIsOnShaderOption )
		m_iShaderOption |= (1 << _eShaderOption);
	else
	{
		m_iShaderOption &= ~(1 << _eShaderOption);

		switch ( _eShaderOption )
		{
			case Engine::CRenderer::ShaderOption_HDR:
				//m_pTarget_Manager->Clear_Target( L"RenderTarget_HDRFilter" );
				break;
			case Engine::CRenderer::ShaderOption_EdgeDetecting:
				m_pTarget_Manager->Clear_Target( L"RenderTarget_EdgeDetecting" );
				break;
			case Engine::CRenderer::ShaderOption_Shadow:
				m_pTarget_Manager->Clear_MRT( L"MRT_Shadow" );
				break;
			case Engine::CRenderer::ShaderOption_MotionBlur:
				
				break;
			case Engine::CRenderer::ShaderOption_SSAO:
				m_pTarget_Manager->Clear_Target( L"RenderTarget_SSAO" );
				break;
			case Engine::CRenderer::ShaderOption_DOF:
				
				break;
			case Engine::CRenderer::ShaderOption_FXAA:
				
				break;
			case Engine::CRenderer::ShaderOption_LightShaft:
				
				break;
		}
	}
}

_bool CRenderer::Get_ShaderOptionOnOff( const SHADEROPTION & _eShaderOption )
{
	if ( _eShaderOption <= ShaderOption_None || _eShaderOption >= ShaderOption_End )
		return false;

	return (m_iShaderOption & (1 << _eShaderOption)) != 0;
}

void CRenderer::SetUp_ShadowDataOnShader( LPD3DXEFFECT _pEffect, const char * _pLightVPName, const char * _pLightVPBName, const char* _pCascadeRangeName )
{
	_pEffect->SetMatrixArray( _pLightVPName, m_matLightVP, Cascade_End );
	_pEffect->SetMatrixArray( _pLightVPBName, m_matLightVPB, Cascade_End );
	_pEffect->SetFloatArray( _pCascadeRangeName, m_fCasCadeRange, Cascade_End );
}

HRESULT CRenderer::Ready_LoadShaderVarData( const _tchar * _pFilePath )
{
	HANDLE hFile = CreateFile( _pFilePath, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

	if ( hFile == INVALID_HANDLE_VALUE )
	{
		MSG_BOX( "Load Failed" );
		return E_FAIL;
	}

	_uint iCount = 0;
	_ulong dwByte = 0;

	ReadFile( hFile, &iCount, sizeof( _uint ), &dwByte, nullptr );

	m_ShaderVarData.iEffectVarDataCount = iCount;
	m_ShaderVarData.pEffectVarData = new EFFECTVARDATA[Shader_End];

	for ( _uint i = 0; i < iCount; ++i )
	{
		CRenderer::EFFECTVARDATA	EffectVarData;

		_uint jLength = 0;
		ReadFile( hFile, &jLength, sizeof( _uint ), &dwByte, nullptr );

		EffectVarData.iEffectVarCount = jLength;
		EffectVarData.pEffectVar = new Engine::CRenderer::EFFECTVARTYPE[jLength];
		for ( _uint j = 0; j < jLength; ++j )
		{
			ReadFile( hFile, &EffectVarData.pEffectVar[j], sizeof( Engine::CRenderer::EFFECTVARTYPE ), &dwByte, nullptr );
		}

		m_ShaderVarData.pEffectVarData[i] = EffectVarData;
	}

	CloseHandle( hFile );

	return NOERROR;
}

HRESULT CRenderer::Ready_Renderer(void)
{
	if(nullptr == m_pTarget_Manager)
		return E_FAIL;

	D3DVIEWPORT9		ViewPort;
	ZeroMemory(&ViewPort, sizeof(D3DVIEWPORT9));

	m_pGraphicDev->GetViewport(&ViewPort);

	if ( FAILED( Ready_RenderTarget( ViewPort ) ) )
		return E_FAIL;
	if ( FAILED( Ready_ShaderCom() ) )
		return E_FAIL;
	if ( FAILED( Ready_Buffer( ViewPort ) ) )
		return E_FAIL;
	if ( FAILED( Ready_DefaultSetting( ViewPort ) ) )
		return E_FAIL;

#ifdef ADD_GETTER_TARGETANDSHADER
	m_pAllEffectArr = new LPD3DXEFFECT[Shader_End];
	m_iEffectCount = Shader_End;

	for ( size_t i = 0; i < Shader_End; ++i )
	{
		m_pAllEffectArr[i] = m_pEffectArr[i];
	}

	m_iTargetCount = m_pTarget_Manager->Get_TargetCount();
	m_pAllTagetNameArr = new _tchar*[m_iTargetCount];
	for ( _uint i = 0; i < m_iTargetCount; ++i )
	{
		m_pAllTagetNameArr[i] = new _tchar[128];
		const _tchar* pTargetName = m_pTarget_Manager->Get_TargetName( i );
		if ( nullptr == pTargetName )
			return E_FAIL;
		lstrcpy( m_pAllTagetNameArr[i], pTargetName );
	}
#endif

	m_SortRenderObject = [&](const REDNEROBJECT& Dst, const REDNEROBJECT& Src) {return Dst.fSort > Src.fSort; };

	if ( FAILED( Ready_LoadShaderVarData( L"../Bin/Data/ShaderOptionData/EffectVarData_Stage1.dat" ) ) )
		return E_FAIL;

	return NOERROR;
}

void CRenderer::Render_RenderGroup(void)
{
#ifdef KEYBOARDCHECK
	KeyCheck_DebugRenderTarget();
#endif

#ifdef KMH_MODIFY // By.KMH -> 재사용하는 렌더 타겟들 한번 초기화하기..
	//m_pTarget_Manager->Clear_MRT( L"MRT_Filter" );
	//m_pTarget_Manager->Clear_MRT( L"MRT_FrameBuffer" );

	// --------------- 사용하는 카메라 데이터를 미리 받아놓는다.. --------------- //
	ModifyCameraData();
	// --------------- 공통으로 사용하는 쉐이더 데이터 미리 세팅.. --------------- //
	ModifyShaderData();
#endif

	// --------------- Render Priority Object ( Skybox 등등 ).. --------------- //
	BEGINRT( L"RenderTarget_Priority" ); // By.KMH

	Render_Priority();

	ENDRT( L"RenderTarget_Priority" ); // By.KMH


	// --------------- Non Alpha Object Deferred Rendering.. --------------- //
	Render_Deferred();


	// --------------- FrameBuffer 가 그려지기 전 수행해야 할 것들.. --------------- //
	if ( true == Get_ShaderOptionOnOff( ShaderOption_EdgeDetecting ) )
		Render_EdgeDection();


	// --------------- Render FrameBuffer.. --------------- //
	BEGINRT( L"RenderTarget_FrameBuffer" );
	//BEGINRT( L"RenderTarget_ResultBuffer" );
	m_pLastRTName = L"RenderTarget_FrameBuffer";

	Render_FrameBuffer();

	//ENDRT( L"RenderTarget_ResultBuffer" );
	ENDRT( L"RenderTarget_FrameBuffer" );

	const _tchar* pTemp = nullptr;

#ifdef RENDEREREFFECTHDR
	// --------------- 모든 렌더된 오브젝트를 하나의 렌더 타겟에 담아버리기.. --------------- //
	m_pTempRTName = L"RenderTarget_ResultBuffer";
	//m_pTempRTName = L"RenderTarget_FrameBuffer";
	BEGINRT( m_pTempRTName );

	if ( true == Get_ShaderOptionOnOff( ShaderOption_LightShaft ) )
		Render_LightOcclusion();

	Render_SumBuffer();	

	// --------------- Alpha Object Rendering.. --------------- //
	Render_Alpha();

	Render_Effect(); // By.KJY - 일반 이펙트

	ENDRT( m_pTempRTName );

	// --------------- Calc HDR.. --------------- //
	_bool bHDROn = true == Get_ShaderOptionOnOff( ShaderOption_HDR );
	if ( true == bHDROn )
	{
		pTemp = m_pTempRTName;
		m_pTempRTName = m_pLastRTName;
		m_pLastRTName = pTemp;

		Calc_HDRProgress();
	}
	else
	{
		m_pLastRTName = L"RenderTarget_FrameBuffer";
		m_pTempRTName = L"RenderTarget_ResultBuffer";
	}
#else
	// --------------- Calc HDR.. --------------- //
	_bool bHDROn = true == Get_ShaderOptionOnOff( ShaderOption_HDR );
	if ( true == bHDROn )
	{
		m_pTempRTName = L"RenderTarget_ResultBuffer";

		Calc_HDRProgress();

		pTemp = m_pTempRTName;
		m_pTempRTName = m_pLastRTName;
		m_pLastRTName = pTemp;
	}
	else
	{
		m_pTempRTName = L"RenderTarget_ResultBuffer";
	}

	if ( true == Get_ShaderOptionOnOff( ShaderOption_LightShaft ) )
		Render_LightOcclusion();

	// --------------- 모든 렌더된 오브젝트를 하나의 렌더 타겟에 담아버리기.. --------------- //
	BEGINRT( m_pTempRTName );
	
	Render_SumBuffer();	
	
	// --------------- Alpha Object Rendering.. --------------- //
	Render_Alpha();

	// --------------- Render Decal.. --------------- //
	Render_Decal();
	
	ENDRT( m_pTempRTName );
#endif
	
	pTemp = m_pTempRTName;
	m_pTempRTName = m_pLastRTName;
	m_pLastRTName = pTemp;
	
	
	// --------------- 왜곡 이펙트 적용.. --------------- //	
	BEGINRT( m_pTempRTName );
	
	Render_Distortion(); // By.KJY - 왜곡 이펙트(노말텍스쳐)
	
	ENDRT( m_pTempRTName );
	
	pTemp = m_pTempRTName;
	m_pTempRTName = m_pLastRTName;
	m_pLastRTName = pTemp;
	
	
	// --------------- Calc PostProcessing.. --------------- //
	Calc_PostProcessing();
	
	
	// --------------- Render Final RenderTarget( BackBuffer ).. --------------- //
	Render_Final();

	pTemp = m_pTempRTName;
	m_pTempRTName = m_pLastRTName;
	m_pLastRTName = pTemp;

	if ( true == Get_ShaderOptionOnOff( ShaderOption_FXAA ) )
	//if( true )
	{
		Render_FXAA();

		pTemp = m_pTempRTName;
		m_pTempRTName = m_pLastRTName;
		m_pLastRTName = pTemp;
	}

	Render_BackBuffer();

	// --------------- Render UI.. --------------- //
	Render_UI();



	if ( m_pInput_Device->Get_DownKeyState( DIK_F2 ) )
		m_bIsShowAllTarget = !m_bIsShowAllTarget;
	if(true == m_bIsShowAllTarget )
		m_pTarget_Manager->Render_DebugAllTarget();

	Clear_RenderGroup();
}

void CRenderer::Release_StaticRenderGroup( void )
{
	for ( _uint i = 0; i < STATICRENDER_END; ++i )
	{
		for ( _uint j = 0; j < m_StaticRenderGroup[i].iGroupCount; ++j )
			Engine::Safe_Release( m_StaticRenderGroup[i].pGroup[j].pGameObject );

		m_StaticRenderGroup[i].iGroupCount = 0;
		Engine::Safe_Delete_Array( m_StaticRenderGroup[i].pGroup );
	}
}

_bool CRenderer::Set_ShaderVarData( const _uint & _iShaderIndex, Engine::CRenderer::EFFECTVARDATA* _pEffectVarData )
{
	if ( _iShaderIndex >= m_ShaderVarData.iEffectVarDataCount )
		return false;

	m_ShaderVarData.pEffectVarData[_iShaderIndex] = *_pEffectVarData;
	return true;
}

Engine::CRenderer::EFFECTVARDATA* CRenderer::Get_ShaderVar( const _uint & _iIndex )
{
	if ( _iIndex >= m_ShaderVarData.iEffectVarDataCount )
		return nullptr;

	return &m_ShaderVarData.pEffectVarData[_iIndex];
}

#ifdef RENDERERDEBUGING
void CRenderer::SelectMoveKeyCheck( _uint & _iSelect )
{
	static _uint iFrameUp = 20;
	static _uint iFrameDown = 20;

	if ( m_iSelect > 0 )
	{
		if ( m_pInput_Device->Get_DownKeyState( CRenderer::UpKey ) )
			--m_iSelect;
		if ( m_pInput_Device->Get_DIKeyState( CRenderer::UpKey ) )
		{
			if ( iFrameUp > 0 )
				--iFrameUp;
			if ( iFrameUp <= 0 )
			{
				iFrameUp = 3;
				--m_iSelect;
			}
		}
		else
			iFrameUp = 20;
	}
	else
		iFrameUp = 20;

	if ( m_iSelect <= m_iLimit )
	{
		if ( m_pInput_Device->Get_DownKeyState( CRenderer::DownKey ) )
			++m_iSelect;
		if ( m_pInput_Device->Get_DIKeyState( CRenderer::DownKey ) )
		{
			if ( iFrameDown > 0 )
				--iFrameDown;
			if ( iFrameDown <= 0 )
			{
				iFrameDown = 3;
				++m_iSelect;
			}
		}
		else
			iFrameDown = 20;
	}
	else
		iFrameDown = 20;
}

void CRenderer::PrevKeyCheck( void )
{
	if ( m_pInput_Device->Get_DownKeyState( CRenderer::BackKey ) )
	{
		if ( nullptr == m_ActNodeList.back() )
			return;

		m_iSelect = 0;
		m_ActFunction = m_ActNodeList.back();
		m_ActNodeList.pop_back();
	}
}

void CRenderer::Debug_ShowSelectOption( void )
{
	m_iLimit = 2;
	SelectMoveKeyCheck( m_iSelect );

	_float fStartX = 1500.f;
	_float fStartY = 200.f;
	_float fHeight = 20.f;

	CDebug::GetInstance()->Add_DebugText( fStartX, fStartY + fHeight * 0.f, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), "0. Show RenderTarget" );
	CDebug::GetInstance()->Add_DebugText( fStartX, fStartY + fHeight * 1.f, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), "1. Set Shader Option" );
	CDebug::GetInstance()->Add_DebugText( fStartX, fStartY + fHeight * 2.f, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), "2. Set Render Channel" );
	CDebug::GetInstance()->Add_DebugText( fStartX - 30.f, fStartY + fHeight * m_iSelect, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), "=>" );

	CDebug::GetInstance()->Add_DebugText( fStartX, fStartY - 140.f, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), "UpKey is DIK_I" );
	CDebug::GetInstance()->Add_DebugText( fStartX, fStartY - 110.f, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), "DownKey is DIK_K" );
	CDebug::GetInstance()->Add_DebugText( fStartX, fStartY - 80.f, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), "SelectButton is DIK_U" );
	CDebug::GetInstance()->Add_DebugText( fStartX, fStartY - 50.f, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), "BackButton is DIK_P" );

	if ( m_pInput_Device->Get_DownKeyState( CRenderer::SelectKey ) )
	{
		m_ActNodeList.push_back( m_ActFunction );

		switch ( m_iSelect )
		{
			case 0:
				m_ActFunction = [&]( void ) { Debug_ShowSelectRT(); };
				break;
			case 1:
				m_ActFunction = [&]( void ) { Debug_ShowSelectShaderOption(); };
				break;
			case 2:
				m_ActFunction = [&]( void ) { Debug_ShowTargetOption(); };
				break;
			default:
				return;
		}

		m_iSelect = 0;
	}
}

void CRenderer::Debug_ShowSelectRT( void )
{
	m_iLimit = m_iTargetCount - 1;
	SelectMoveKeyCheck( m_iSelect );

	_float fStartX = 1500.f;
	_float fStartY = 200.f;
	_float fHeight = 20.f;

	CDebug::GetInstance()->Add_DebugText( fStartX, fStartY - 140.f, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), "UpKey is DIK_I" );
	CDebug::GetInstance()->Add_DebugText( fStartX, fStartY - 110.f, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), "DownKey is DIK_K" );
	CDebug::GetInstance()->Add_DebugText( fStartX, fStartY - 80.f, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), "SelectButton is DIK_U" );
	CDebug::GetInstance()->Add_DebugText( fStartX, fStartY - 50.f, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), "BackButton is DIK_P" );

	if ( 0 == m_iSelect )
		m_iSelect = 1;
	
	CDebug::GetInstance()->Add_DebugText( fStartX, fStartY - 20.f, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), "BackBuffer Show => DIK_L" );
	CDebug::GetInstance()->Add_DebugText( fStartX, fStartY, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), "Select Target Save => DIK_HOME" );



	char szName[256] = "";
	int len = WideCharToMultiByte( CP_ACP, 0, m_pAllTagetNameArr[m_iSelect - 1], -1, NULL, 0, NULL, NULL );
	WideCharToMultiByte( CP_ACP, 0, m_pAllTagetNameArr[m_iSelect - 1], -1, szName, len, NULL, NULL );

	CDebug::GetInstance()->Add_DebugText( fStartX, fStartY + fHeight, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), szName );

	CDebug::GetInstance()->Add_DebugText( fStartX - 30.f, fStartY + fHeight * 1, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), "=>" );

	_bool bNext = false;
	if ( m_pInput_Device->Get_DownKeyState( DIK_L ) )
	{
		Set_RenderBackBufferKind( RenderBackBuffer_Default, nullptr );
	}
	if ( m_pInput_Device->Get_DownKeyState( DIK_HOME ) )
	{
		if ( nullptr != m_pDebugRenderTargetTag )
			m_pTarget_Manager->Save_Target( m_pDebugRenderTargetTag );
	}
	if ( m_pInput_Device->Get_DownKeyState( CRenderer::SelectKey ) )
		bNext = true;

	if ( true == bNext )
	{
		if ( true == bNext )
		{
			if ( m_iSelect == 0 )
				Set_RenderBackBufferKind( RenderBackBuffer_Default, nullptr );
			else
				Set_RenderBackBufferKind( RenderBackBuffer_DebugRenderTarget, m_pAllTagetNameArr[m_iSelect - 1] );
		}
	}
	else
		PrevKeyCheck();
}

void CRenderer::Debug_ShowTargetOption( void )
{
	m_iLimit = 5 - 1;
	SelectMoveKeyCheck( m_iSelect );

	_float fStartX = 1500.f;
	_float fStartY = 200.f;
	_float fHeight = 20.f;

	CDebug::GetInstance()->Add_DebugText( fStartX, fStartY - 140.f, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), "UpKey is DIK_I" );
	CDebug::GetInstance()->Add_DebugText( fStartX, fStartY - 110.f, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), "DownKey is DIK_K" );
	CDebug::GetInstance()->Add_DebugText( fStartX, fStartY - 80.f, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), "SelectButton is DIK_U" );
	CDebug::GetInstance()->Add_DebugText( fStartX, fStartY - 50.f, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), "BackButton is DIK_P" );

	CDebug::GetInstance()->Add_DebugText( fStartX, fStartY, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), "RenderTarget Render Mode" );

	CDebug::GetInstance()->Add_DebugText( fStartX, fStartY + fHeight * 1.f, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), "0. Default" );
	CDebug::GetInstance()->Add_DebugText( fStartX, fStartY + fHeight * 2.f, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), "1. Alpha" );
	CDebug::GetInstance()->Add_DebugText( fStartX, fStartY + fHeight * 3.f, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), "2. Red" );
	CDebug::GetInstance()->Add_DebugText( fStartX, fStartY + fHeight * 4.f, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), "3. Greed" );
	CDebug::GetInstance()->Add_DebugText( fStartX, fStartY + fHeight * 5.f, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), "4. Blue" );
	CDebug::GetInstance()->Add_DebugText( fStartX, fStartY + fHeight * 6.f, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), "5. TexCoord" );

	CDebug::GetInstance()->Add_DebugText( fStartX - 30.f, fStartY + fHeight * (m_iSelect + 1), D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), "=>" );

	if ( m_pInput_Device->Get_DownKeyState( CRenderer::SelectKey ) )
		m_iDebugTargetOption = m_iSelect;
	else
		PrevKeyCheck();
}

void CRenderer::Debug_ShowSelectShaderOption( void )
{
	m_iLimit = ShaderOption_End - 1;
	SelectMoveKeyCheck( m_iSelect );

	_float fStartX = 1500.f;
	_float fStartY = 200.f;
	_float fHeight = 20.f;

	CDebug::GetInstance()->Add_DebugText( fStartX, fStartY - 140.f, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), "UpKey is DIK_I" );
	CDebug::GetInstance()->Add_DebugText( fStartX, fStartY - 110.f, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), "DownKey is DIK_K" );
	CDebug::GetInstance()->Add_DebugText( fStartX, fStartY - 80.f, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), "SelectButton is DIK_U" );
	CDebug::GetInstance()->Add_DebugText( fStartX, fStartY - 50.f, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), "BackButton is DIK_P" );

	for ( size_t i = 0; i < ShaderOption_End; ++i )
	{
		switch ( i )
		{
			case Engine::CRenderer::ShaderOption_Light:
				CDebug::GetInstance()->Add_DebugText( fStartX, fStartY + i * fHeight, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), "Light Option" );
				break;
			case Engine::CRenderer::ShaderOption_HDR:
				CDebug::GetInstance()->Add_DebugText( fStartX, fStartY + i * fHeight, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), "HDR Option" );
				break;
			case Engine::CRenderer::ShaderOption_EdgeDetecting:
				CDebug::GetInstance()->Add_DebugText( fStartX, fStartY + i * fHeight, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), "EdgeDection Option" );
				break;
			case Engine::CRenderer::ShaderOption_Shadow:
				CDebug::GetInstance()->Add_DebugText( fStartX, fStartY + i * fHeight, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), "Shadow Option" );
				break;
			case Engine::CRenderer::ShaderOption_MotionBlur:
				CDebug::GetInstance()->Add_DebugText( fStartX, fStartY + i * fHeight, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), "MotionBlur Option" );
				break;
			case Engine::CRenderer::ShaderOption_SSAO:
				CDebug::GetInstance()->Add_DebugText( fStartX, fStartY + i * fHeight, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), "SSAO Option" );
				break;
			case Engine::CRenderer::ShaderOption_DOF:
				CDebug::GetInstance()->Add_DebugText( fStartX, fStartY + i * fHeight, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), "DOF Option" );
				break;
			case Engine::CRenderer::ShaderOption_FXAA:
				CDebug::GetInstance()->Add_DebugText( fStartX, fStartY + i * fHeight, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), "FXAA Option" );
				break;
			case Engine::CRenderer::ShaderOption_LightShaft:
				CDebug::GetInstance()->Add_DebugText( fStartX, fStartY + i * fHeight, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), "LightShaft Option" );
				break;
			case Engine::CRenderer::ShaderOption_Fog:
				CDebug::GetInstance()->Add_DebugText( fStartX, fStartY + i * fHeight, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), "Fog Option" );
				break;
			case Engine::CRenderer::ShaderOption_RainAmbience:
				CDebug::GetInstance()->Add_DebugText( fStartX, fStartY + i * fHeight, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), "RainAmbience Option" );
				break;
		}
	}

	CDebug::GetInstance()->Add_DebugText( fStartX - 30.f, fStartY + fHeight * m_iSelect, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), "=>" );

	if ( m_pInput_Device->Get_DownKeyState( CRenderer::SelectKey ) )
	{
		m_ActNodeList.push_back( m_ActFunction );

		m_iWaitFrame = 20;
		m_ActFunction = [&]( void ) { Debug_ShowShaderOptionYesOrNo(); };
	}
	else
		PrevKeyCheck();
}

void CRenderer::Debug_ShowShaderOptionYesOrNo( void )
{
	m_iLimit = 2;
	SelectMoveKeyCheck( m_iSelect );

	_float fStartX = 1500.f;
	_float fStartY = 200.f;

	CDebug::GetInstance()->Add_DebugText( fStartX, fStartY, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), "Yes is DIK_U" );
	CDebug::GetInstance()->Add_DebugText( fStartX, fStartY + 20.f, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), "No is DIK_P" );

	if ( m_pInput_Device->Get_DownKeyState( CRenderer::SelectKey ) )
	{
		Set_ShaderOptionOnOff( SHADEROPTION( m_iSelect ), true );

		m_iWaitFrame = 20;
		m_ActFunction = [&]( void ) { Debug_ShowWaitFrame(); };
	}
	else if ( m_pInput_Device->Get_DownKeyState( CRenderer::BackKey ) )
	{
		Set_ShaderOptionOnOff( SHADEROPTION( m_iSelect ), false );

		m_iWaitFrame = 20;
		m_ActFunction = [&]( void ) { Debug_ShowWaitFrame(); };
	}
}

void CRenderer::Debug_ShowWaitFrame( void )
{
	m_iWaitFrame -= 3;

	_float fStartX = 1500.f;
	_float fStartY = 200.f;

	CDebug::GetInstance()->Add_DebugText( fStartX, fStartY, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), "Success!!!! Wait : %d Frame", m_iWaitFrame );

	if ( m_iWaitFrame <= 0 )
	{
		if ( nullptr == m_ActNodeList.back() )
		{
			MSG_BOX( "nullptr Prev Act Function" );
			return;
		}

		m_ActFunction = m_ActNodeList.back();
		m_ActNodeList.pop_back();
	}
}
#endif

void CRenderer::Render_Priority(void)
{
	//m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);
	//m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	for (auto& RenderObject : m_RenderGroup[RENDER_PRIORITY])
	{
		if(nullptr != RenderObject.pGameObject)
			RenderObject.pGameObject->Render_GameObject();
	}

	//m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, TRUE);
	//m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
}

void CRenderer::Render_NonAlpha( void )
{
	for ( auto& RenderObject : m_RenderGroup[RENDER_NONALPHA] )
	{
		if ( nullptr != RenderObject.pGameObject )
			RenderObject.pGameObject->Render_GameObject();
	}
	
	STATICRENDERGROUP& StaticRenderGroup = m_StaticRenderGroup[STATICRENDER_NONALPHA];

	m_pEffectArr[Shader_Mesh]->Begin(nullptr, 0);
	for ( _uint i = 0; i < StaticRenderGroup.iGroupCount; ++i )
	{
		if ( nullptr != StaticRenderGroup.pGroup[i].pGameObject )
		{
			if ( true == m_pFrustum->isIn_Frustum( &StaticRenderGroup.pGroup[i].vPos, StaticRenderGroup.pGroup[i].fCullSize, StaticRenderGroup.pGroup[i].pGameObject->Get_IsRender_Pointer()) )
			{
				StaticRenderGroup.pGroup[i].pGameObject->Render_GameObject();
			}
		}
	}
	m_pEffectArr[Shader_Mesh]->End();
}

void CRenderer::Render_Alpha(void)
{
	if (nullptr == m_pGraphicDev)
		return;

	//m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	SettingAlphaBlendRenderObjectSortValue( m_RenderGroup[RENDER_ALPHA] );
	m_RenderGroup[RENDER_ALPHA].sort(m_SortRenderObject);

	m_pEffectArr[Shader_Effect]->Begin(nullptr, 0);
	for (auto& RenderObject : m_RenderGroup[RENDER_ALPHA])
	{
		if (nullptr != RenderObject.pGameObject)
			RenderObject.pGameObject->Render_GameObject();
	}
	m_pEffectArr[Shader_Effect]->End();

	//m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

void CRenderer::Render_Shadow( void )
{
	if ( nullptr == m_pEffectArr[Shader_Shadow] )
		return;

	Engine::CCamera* pCamera = m_pCam_Manager->Get_CurrentCamera();
	if ( pCamera == nullptr )
		return;


	// 그림자에서 사용할 라이트영역으로 이동할 행렬과 쉐도우맵 텍스쳐영역으로 이동할 행렬을 세팅..
	_vec3 vLightAt, vLightPos, vLightDir;
	vLightDir = _vec3( 1.f, -1.f, 1.f );
	vLightAt = pCamera->Get_At();

	vLightPos = vLightAt + -vLightDir * 200.f;
	D3DXMatrixLookAtLH( &m_matLightView[Cascade_Level1], &vLightPos, &vLightAt, &_vec3( 0.f, 1.f, 0.f ) );

	vLightPos = vLightAt + -vLightDir * 500.f;
	D3DXMatrixLookAtLH( &m_matLightView[Cascade_Level2], &vLightPos, &vLightAt, &_vec3( 0.f, 1.f, 0.f ) );
	
	for ( _uint i = 0; i < Cascade_End; ++i )
	{
		m_matLightVP[i] = m_matLightView[i] * m_matLightProj[i];
		m_matLightVPB[i] = m_matLightVP[i] * m_matScaleBias[i];
	}

	const _tchar* pRTName[Cascade_End] = { L"RenderTarget_ShadowMap_Near", L"RenderTarget_ShadowMap_Mid" };

	m_pEffectArr[Shader_Shadow]->Begin( nullptr, 0 );

	for ( _uint i = 0; i < Cascade_End; ++i ) // CasCade 용 행렬을 세팅하고 루프..
	{
		m_pEffectArr[Shader_Shadow]->BeginPass( 0 );

		// Depth Stencil 버퍼 받아놓기..
		LPDIRECT3DSURFACE9 pOldZBuffer = nullptr;
		m_pGraphicDev->GetDepthStencilSurface( &pOldZBuffer );

		// 쉐도우 맵 만들기..
		BEGINRT( pRTName[i] );

		m_pEffectArr[Shader_Shadow]->SetMatrix( "g_matLightVP", &m_matLightVP[i] );

		m_pEffectArr[Shader_Shadow]->CommitChanges();

		for ( auto& RenderObject : m_ShadowRenderGroup[i] )
		{
			if ( nullptr != RenderObject )
				RenderObject->Render_Shadow( m_pEffectArr[Shader_Shadow], "g_matWorld" );
		}

		ENDRT( pRTName[i] );
	
		// Depth Stencil 버퍼 되돌리기(????)..
		m_pGraphicDev->SetDepthStencilSurface( pOldZBuffer );
		Engine::Safe_Release( pOldZBuffer );
	
		m_pGraphicDev->Clear( 0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DXCOLOR( 1.f, 1.f, 1.f, 1.f ), 1.f, 0 );
	
		m_pEffectArr[Shader_Shadow]->EndPass();
	}
	
	m_pEffectArr[Shader_Shadow]->End();
	
	//// 그림자를 흐리게끔 한다..
	//Shader_Begin( m_pEffectArr[Shader_Filter], "DownFilter" );
	//
	//const _tchar* pRTDownFilterName[Cascade_End] = { L"RenderTarget_ShadowMap_Near_X4", L"RenderTarget_ShadowMap_Mid_X4" };
	//
	//for ( _uint i = 0; i < 1; ++i )
	//{
	//	// 다운 샘플링을 한 것에 담은 후 다시 업 샘플림을 진행한다..
	//	BEGINRT( pRTDownFilterName[i] );
	//
	//	SETRT( m_pEffectArr[Shader_Filter], pRTName[i], "g_Src0Texture" );
	//
	//	Shader_Draw( m_pEffectArr[Shader_Filter], 0 );
	//
	//	ENDRT( pRTDownFilterName[i] );
	//
	//
	//	// 흐리게 한 뒤 그림자 맵에 다시 담는다..
	//	BEGINRT( pRTName[i] );
	//
	//	SETRT( m_pEffectArr[Shader_Filter], pRTDownFilterName[i], "g_Src0Texture" );
	//
	//	Shader_Draw( m_pEffectArr[Shader_Filter], 3 );
	//
	//	ENDRT( pRTName[i] );
	//}
	//
	//m_pEffectArr[Shader_Filter]->End();
}

void CRenderer::Render_Distortion(void)
{
	if (nullptr == m_pGraphicDev
		|| nullptr == m_pShader_Distort)
		return;

	m_pTarget_Manager->Begin_Target(L"Target_Distort", 0); // By.KJY

	for (auto& RenderObject : m_RenderGroup[RENDER_DISOTRT])
	{
		if (nullptr != RenderObject.pGameObject)
			RenderObject.pGameObject->Render_GameObject();
	}

	m_pTarget_Manager->End_Target(L"Target_Distort", 0); // By.KJY

	// By.KJY - 최종 씬과 왜곡 블렌딩
	Render_Blend_Distortion();
}

void CRenderer::Render_Decal( void )
{
	if ( nullptr == m_pEffectArr[Shader_SSDecal] ||
		 nullptr == m_pDecal_Manager )
		return;

#ifndef VIEWPROJMATRIXSETTING_ONCE
	SettingViewProjMatrix( m_pEffectArr[Shader_SSDecal] );
#endif

	m_pEffectArr[Shader_SSDecal]->SetMatrix( "g_matView", &m_CamData.matView );
	m_pEffectArr[Shader_SSDecal]->SetMatrix( "g_matProj", &m_CamData.matProj );
	m_pEffectArr[Shader_SSDecal]->SetMatrix( "g_matInvProj", &m_CamData.matInvProj );
	SETRT( m_pEffectArr[Shader_SSDecal], L"RenderTarget_Depth", "g_DepthTexture" );
	m_pEffectArr[Shader_SSDecal]->SetFloat( "g_fCamFar", m_CamData.fCamFar );

	m_pEffectArr[Shader_SSDecal]->Begin( nullptr, 0 );

	m_pDecal_Manager->Render_SSDecal();

	m_pEffectArr[Shader_SSDecal]->End();

#ifdef SSD_DEBUG
	static _ubyte iDebugSSDecalType = 0;

	switch ( iDebugSSDecalType )
	{
		case 0:
			CDebug::GetInstance()->Add_DebugText( 1500.f, 10.f, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), "NUMPAD7 : Show SSDecal WireFrame" );
			break;
		case 1:
			CDebug::GetInstance()->Add_DebugText( 1500.f, 10.f, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), "NUMPAD7 : Show SSDecal Cube" );
			break;
		case 2:
			CDebug::GetInstance()->Add_DebugText( 1500.f, 10.f, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), "NUMPAD7 : Hide SSDecal" );
			break;
	}

	if ( CInput_Device::GetInstance()->Get_DownKeyState( DIK_NUMPAD7 ) )
	{
		iDebugSSDecalType = (iDebugSSDecalType + 1) % 3;
	}

	if ( iDebugSSDecalType )
	{
		m_pDecal_Manager->Render_SSDecalDebug( iDebugSSDecalType - 1 );
	}
#endif
}

void CRenderer::Render_UI(void)
{
	m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	m_RenderGroup[RENDER_UI].sort(m_SortRenderObject);

	for (auto& RenderObject : m_RenderGroup[RENDER_UI])
	{
		if (nullptr != RenderObject.pGameObject)
			RenderObject.pGameObject->Render_GameObject();
	}
	m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

}

#ifdef KMH_MODIFY
HRESULT CRenderer::Ready_RenderTarget( const D3DVIEWPORT9 & ViewPort )
{
#ifndef KMH_MODIFY // By.KMH
	// 렌더타겟을 생성한다.ㅑ(Target_Color)
	if(FAILED(m_pTarget_Manager->Add_Target(m_pGraphicDev, L"Target_Color", ViewPort.Width, ViewPort.Height
											 , D3DFMT_A8R8G8B8, D3DXCOLOR(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if(FAILED(m_pTarget_Manager->Ready_DebugBuffer(L"Target_Color", 0.f, 0.f, 200.f, 200.f)))
		return E_FAIL;

	// 렌더타겟을 생성한다.ㅑ(Target_Normal)
	if(FAILED(m_pTarget_Manager->Add_Target(m_pGraphicDev, L"Target_Normal", ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	if(FAILED(m_pTarget_Manager->Ready_DebugBuffer(L"Target_Normal", 0.f, 200.0f, 200.f, 200.f)))
		return E_FAIL;

	// 렌더타겟을 생성한다.ㅑ(Target_Depth)
	if(FAILED(m_pTarget_Manager->Add_Target(m_pGraphicDev, L"Target_Depth", ViewPort.Width, ViewPort.Height, D3DFMT_A32B32G32R32F, D3DXCOLOR(1.f, 1.f, 1.f, 0.f))))
		return E_FAIL;

	if(FAILED(m_pTarget_Manager->Ready_DebugBuffer(L"Target_Depth", 0.0f, 400.f, 200.f, 200.f)))
		return E_FAIL;

	// 렌더타겟을 생성한다.ㅑ(Target_Shade)
	if(FAILED(m_pTarget_Manager->Add_Target(m_pGraphicDev, L"Target_Shade", ViewPort.Width, ViewPort.Height, D3DFMT_A8R8G8B8, D3DXCOLOR(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	if(FAILED(m_pTarget_Manager->Ready_DebugBuffer(L"Target_Shade", 200.0f, 0.f, 200.f, 200.f)))
		return E_FAIL;

	// 렌더타겟을 생성한다.ㅑ(Target_Specular)
	if(FAILED(m_pTarget_Manager->Add_Target(m_pGraphicDev, L"Target_Specular", ViewPort.Width, ViewPort.Height, D3DFMT_A8R8G8B8, D3DXCOLOR(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if(FAILED(m_pTarget_Manager->Ready_DebugBuffer(L"Target_Specular", 200.0f, 200.f, 200.f, 200.f)))
		return E_FAIL;

	//----------------------------------------------------------------------------------------------------------------------------------------------------------------------//

	// 멀티렌더타겟을 구성한다.
	if(FAILED(m_pTarget_Manager->Add_MRT(L"MRT_Deferred", L"Target_Color")))
		return E_FAIL;

	if(FAILED(m_pTarget_Manager->Add_MRT(L"MRT_Deferred", L"Target_Normal")))
		return E_FAIL;

	if(FAILED(m_pTarget_Manager->Add_MRT(L"MRT_Deferred", L"Target_Depth")))
		return E_FAIL;

	if(FAILED(m_pTarget_Manager->Add_MRT(L"MRT_LightAcc", L"Target_Shade")))
		return E_FAIL;

	if(FAILED(m_pTarget_Manager->Add_MRT(L"MRT_LightAcc", L"Target_Specular")))
		return E_FAIL;
#else
	if ( nullptr == m_pTarget_Manager )
		return E_FAIL;

	// ---- For. Deferred RenderTarget Group..
	if ( FAILED( m_pTarget_Manager->Add_Target( m_pGraphicDev, L"RenderTarget_Color", ViewPort.Width, ViewPort.Height, D3DFMT_A8R8G8B8, D3DXCOLOR( 0.f, 0.f, 0.f, 0.f ) ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_Target( m_pGraphicDev, L"RenderTarget_Normal", ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR( 1.f, 1.f, 1.f, 0.f ) ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_Target( m_pGraphicDev, L"RenderTarget_Depth", ViewPort.Width, ViewPort.Height, D3DFMT_A32B32G32R32F, D3DXCOLOR( 1.f, 0.f, 0.f, 0.f ) ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_Target( m_pGraphicDev, L"RenderTarget_SpecularIntensity", ViewPort.Width, ViewPort.Height, D3DFMT_A8R8G8B8, D3DXCOLOR( 0.f, 0.f, 0.f, 0.f ) ) ) )
		return E_FAIL;

	if ( FAILED( m_pTarget_Manager->Ready_DebugBuffer( L"RenderTarget_Color", 0.f, 0.f, 200.f, 200.f ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Ready_DebugBuffer( L"RenderTarget_Normal", 0.f, 200.f, 200.f, 200.f ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Ready_DebugBuffer( L"RenderTarget_Depth", 0.f, 400.f, 200.f, 200.f ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Ready_DebugBuffer( L"RenderTarget_SpecularIntensity", 0.f, 600.f, 200.f, 200.f ) ) )
		return E_FAIL;

	if ( FAILED( m_pTarget_Manager->Add_MRT( L"MRT_Deferred", L"RenderTarget_Color" ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_MRT( L"MRT_Deferred", L"RenderTarget_Normal" ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_MRT( L"MRT_Deferred", L"RenderTarget_Depth" ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_MRT( L"MRT_Deferred", L"RenderTarget_SpecularIntensity" ) ) )
		return E_FAIL;



	// ---- For. LightAcc RenderTarget Group..
	if ( FAILED( m_pTarget_Manager->Add_Target( m_pGraphicDev, L"RenderTarget_Shade", ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR( 0.f, 0.f, 0.f, 0.f ) ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_Target( m_pGraphicDev, L"RenderTarget_Specular", ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR( 0.f, 0.f, 0.f, 0.f ) ) ) )
		return E_FAIL;

	if ( FAILED( m_pTarget_Manager->Ready_DebugBuffer( L"RenderTarget_Shade", 200.f, 0.f, 200.f, 200.f ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Ready_DebugBuffer( L"RenderTarget_Specular", 200.f, 200.f, 200.f, 200.f ) ) )
		return E_FAIL;

	if ( FAILED( m_pTarget_Manager->Add_MRT( L"MRT_LightAcc", L"RenderTarget_Shade" ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_MRT( L"MRT_LightAcc", L"RenderTarget_Specular" ) ) )
		return E_FAIL;



	// ---- For. FrameBuffer RenderTarget Group..
	//if ( FAILED( m_pTarget_Manager->Add_Target( m_pGraphicDev, L"RenderTarget_FrameBuffer", ViewPort.Width, ViewPort.Height, D3DFMT_A8R8G8B8, D3DXCOLOR( 0.f, 0.f, 0.f, 0.f ) ) ) )
	if ( FAILED( m_pTarget_Manager->Add_Target( m_pGraphicDev, L"RenderTarget_FrameBuffer", ViewPort.Width, ViewPort.Height, D3DFMT_A8R8G8B8, D3DXCOLOR( 0.f, 0.f, 0.f, 0.f ) ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_Target( m_pGraphicDev, L"RenderTarget_ResultBuffer", ViewPort.Width, ViewPort.Height, D3DFMT_A8R8G8B8, D3DXCOLOR( 0.f, 0.f, 0.f, 0.f ) ) ) )
		return E_FAIL;

	if ( FAILED( m_pTarget_Manager->Ready_DebugBuffer( L"RenderTarget_FrameBuffer", 400.f, 0.f, 200.f, 200.f ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Ready_DebugBuffer( L"RenderTarget_ResultBuffer", 400.f, 200.f, 200.f, 200.f ) ) )
		return E_FAIL;
	
	if ( FAILED( m_pTarget_Manager->Add_MRT( L"MRT_FrameBuffer", L"RenderTarget_FrameBuffer" ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_MRT( L"MRT_FrameBuffer", L"RenderTarget_ResultBuffer" ) ) )
		return E_FAIL;



	// ---- For. EdgeDetecting Group..
	if ( FAILED( m_pTarget_Manager->Add_Target( m_pGraphicDev, L"RenderTarget_EdgeDetecting", ViewPort.Width, ViewPort.Height, D3DFMT_A8R8G8B8, D3DXCOLOR( 0.f, 0.f, 0.f, 0.f ) ) ) )
		return E_FAIL;

	if ( FAILED( m_pTarget_Manager->Ready_DebugBuffer( L"RenderTarget_EdgeDetecting", 400.f, 400.f, 200.f, 200.f ) ) )
		return E_FAIL;

	if ( FAILED( m_pTarget_Manager->Add_MRT( L"MRT_EdgeDetecting", L"RenderTarget_EdgeDetecting" ) ) )
		return E_FAIL;



	// ---- For. Priority RenderTarget Group..
	if ( FAILED( m_pTarget_Manager->Add_Target( m_pGraphicDev, L"RenderTarget_Priority", ViewPort.Width, ViewPort.Height, D3DFMT_A8R8G8B8, D3DXCOLOR( 0.f, 0.f, 1.f, 1.f ) ) ) )
		return E_FAIL;

	if ( FAILED( m_pTarget_Manager->Ready_DebugBuffer( L"RenderTarget_Priority", 400.f, 600.f, 200.f, 200.f ) ) )
		return E_FAIL;

	if ( FAILED( m_pTarget_Manager->Add_MRT( L"MRT_Priority", L"RenderTarget_Priority" ) ) )
		return E_FAIL;



	// ---- For. Filter RenderTarget Group..
	if ( FAILED( m_pTarget_Manager->Add_Target( m_pGraphicDev, L"RenderTarget_Filter1_X1", ViewPort.Width, ViewPort.Height, D3DFMT_A8R8G8B8, D3DXCOLOR( 0.f, 0.f, 0.f, 0.f ) ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_Target( m_pGraphicDev, L"RenderTarget_Filter2_X1", ViewPort.Width, ViewPort.Height, D3DFMT_A8R8G8B8, D3DXCOLOR( 0.f, 0.f, 0.f, 0.f ) ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_Target( m_pGraphicDev, L"RenderTarget_Filter1_X4", ViewPort.Width / 4, ViewPort.Height / 4, D3DFMT_A8R8G8B8, D3DXCOLOR( 0.f, 0.f, 0.f, 0.f ) ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_Target( m_pGraphicDev, L"RenderTarget_Filter2_X4", ViewPort.Width / 4, ViewPort.Height / 4, D3DFMT_A8R8G8B8, D3DXCOLOR( 0.f, 0.f, 0.f, 0.f ) ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_Target( m_pGraphicDev, L"RenderTarget_Filter1_X16", ViewPort.Width / 16, ViewPort.Height / 16, D3DFMT_A8R8G8B8, D3DXCOLOR( 0.f, 0.f, 0.f, 0.f ) ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_Target( m_pGraphicDev, L"RenderTarget_Filter2_X16", ViewPort.Width / 16, ViewPort.Height / 16, D3DFMT_A8R8G8B8, D3DXCOLOR( 0.f, 0.f, 0.f, 0.f ) ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_Target( m_pGraphicDev, L"RenderTarget_HDRFilter1_X4", ViewPort.Width / 4, ViewPort.Height / 4, D3DFMT_A16B16G16R16F, D3DXCOLOR( 0.f, 0.f, 0.f, 0.f ) ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_Target( m_pGraphicDev, L"RenderTarget_HDRFilter2_X4", ViewPort.Width / 4, ViewPort.Height / 4, D3DFMT_A16B16G16R16F, D3DXCOLOR( 0.f, 0.f, 0.f, 0.f ) ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_Target( m_pGraphicDev, L"RenderTarget_BrightPass", ViewPort.Width / 4 + 2, ViewPort.Height / 4 + 2, D3DFMT_A8R8G8B8, D3DXCOLOR( 0.f, 0.f, 0.f, 0.f ) ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_Target( m_pGraphicDev, L"RenderTarget_BrightPass2", ViewPort.Width / 4 + 2, ViewPort.Height / 4 + 2, D3DFMT_A8R8G8B8, D3DXCOLOR( 0.f, 0.f, 0.f, 0.f ) ) ) )
		return E_FAIL;
	//if ( FAILED( m_pTarget_Manager->Add_Target( m_pGraphicDev, L"RenderTarget_LumFilter64x64", 64, 64, D3DFMT_R16F, D3DXCOLOR( 0.f, 0.f, 0.f, 0.f ) ) ) )
	//	return E_FAIL;
	//if ( FAILED( m_pTarget_Manager->Add_Target( m_pGraphicDev, L"RenderTarget_LumFilter16x16", 16, 16, D3DFMT_R16F, D3DXCOLOR( 0.f, 0.f, 0.f, 0.f ) ) ) )
	//	return E_FAIL;
	//if ( FAILED( m_pTarget_Manager->Add_Target( m_pGraphicDev, L"RenderTarget_LumFilter4x4", 4, 4, D3DFMT_R16F, D3DXCOLOR( 0.f, 0.f, 0.f, 0.f ) ) ) )
	//	return E_FAIL;
	//if ( FAILED( m_pTarget_Manager->Add_Target( m_pGraphicDev, L"RenderTarget_LumFilter1x1", 1, 1, D3DFMT_R16F, D3DXCOLOR( 0.f, 0.f, 0.f, 0.f ) ) ) )
	//	return E_FAIL;
	//if ( FAILED( m_pTarget_Manager->Add_Target( m_pGraphicDev, L"RenderTarget_LastAdpaterLumFilter1x1", 1, 1, D3DFMT_R16F, D3DXCOLOR( 0.f, 0.f, 0.f, 0.f ) ) ) )
	//	return E_FAIL;
	//if ( FAILED( m_pTarget_Manager->Add_Target( m_pGraphicDev, L"RenderTarget_AdpaterLumFilter1x1", 1, 1, D3DFMT_R16F, D3DXCOLOR( 0.f, 0.f, 0.f, 0.f ) ) ) )
	//	return E_FAIL;

	if ( FAILED( m_pTarget_Manager->Ready_DebugBuffer( L"RenderTarget_Filter1_X1", 600.f, 0.f, 100.f, 100.f ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Ready_DebugBuffer( L"RenderTarget_Filter2_X1", 700.f, 0.f, 100.f, 100.f ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Ready_DebugBuffer( L"RenderTarget_Filter1_X4", 600.f, 100.f, 100.f, 100.f ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Ready_DebugBuffer( L"RenderTarget_Filter2_X4", 700.f,  100.f, 100.f, 100.f ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Ready_DebugBuffer( L"RenderTarget_Filter1_X16", 600.f, 200.f, 100.f, 100.f ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Ready_DebugBuffer( L"RenderTarget_Filter2_X16", 700.f,  200.f, 100.f, 100.f ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Ready_DebugBuffer( L"RenderTarget_HDRFilter1_X4", 600.f, 300.f, 100.f, 100.f ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Ready_DebugBuffer( L"RenderTarget_HDRFilter2_X4", 700.f, 300.f, 100.f, 100.f ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Ready_DebugBuffer( L"RenderTarget_BrightPass", 600.f, 400.f, 200.f, 200.f ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Ready_DebugBuffer( L"RenderTarget_BrightPass2", 600.f, 600.f, 200.f, 200.f ) ) )
		return E_FAIL;
	//if ( FAILED( m_pTarget_Manager->Ready_DebugBuffer( L"RenderTarget_LumFilter64x64", 800.f, 0.f, 100.f, 100.f ) ) )
	//	return E_FAIL;
	//if ( FAILED( m_pTarget_Manager->Ready_DebugBuffer( L"RenderTarget_LumFilter16x16", 900.f, 0.f, 100.f, 100.f ) ) )
	//	return E_FAIL;
	//if ( FAILED( m_pTarget_Manager->Ready_DebugBuffer( L"RenderTarget_LumFilter4x4", 800.f, 100.f, 100.f, 100.f ) ) )
	//	return E_FAIL;
	//if ( FAILED( m_pTarget_Manager->Ready_DebugBuffer( L"RenderTarget_LumFilter1x1", 900.f, 100.f, 100.f, 100.f ) ) )
	//	return E_FAIL;
	//if ( FAILED( m_pTarget_Manager->Ready_DebugBuffer( L"RenderTarget_LastAdpaterLumFilter1x1", 800.f, 200.f, 100.f, 100.f ) ) )
	//	return E_FAIL;
	//if ( FAILED( m_pTarget_Manager->Ready_DebugBuffer( L"RenderTarget_AdpaterLumFilter1x1", 900.f, 200.f, 100.f, 100.f ) ) )
	//	return E_FAIL;

	if ( FAILED( m_pTarget_Manager->Add_MRT( L"MRT_Filter", L"RenderTarget_Filter1_X1" ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_MRT( L"MRT_Filter", L"RenderTarget_Filter2_X1" ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_MRT( L"MRT_Filter", L"RenderTarget_Filter1_X4" ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_MRT( L"MRT_Filter", L"RenderTarget_Filter2_X4" ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_MRT( L"MRT_Filter", L"RenderTarget_Filter1_X16" ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_MRT( L"MRT_Filter", L"RenderTarget_Filter2_X16" ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_MRT( L"MRT_Filter", L"RenderTarget_HDRFilter1_X4" ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_MRT( L"MRT_Filter", L"RenderTarget_HDRFilter2_X4" ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_MRT( L"MRT_Filter", L"RenderTarget_BrightPass" ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_MRT( L"MRT_Filter", L"RenderTarget_BrightPass2" ) ) )
		return E_FAIL;
	//if ( FAILED( m_pTarget_Manager->Add_MRT( L"MRT_Filter", L"RenderTarget_LumFilter64x64" ) ) )
	//	return E_FAIL;
	//if ( FAILED( m_pTarget_Manager->Add_MRT( L"MRT_Filter", L"RenderTarget_LumFilter16x16" ) ) )
	//	return E_FAIL;
	//if ( FAILED( m_pTarget_Manager->Add_MRT( L"MRT_Filter", L"RenderTarget_LumFilter4x4" ) ) )
	//	return E_FAIL;
	//if ( FAILED( m_pTarget_Manager->Add_MRT( L"MRT_Filter", L"RenderTarget_LumFilter1x1" ) ) )
	//	return E_FAIL;
	//if ( FAILED( m_pTarget_Manager->Add_MRT( L"MRT_Filter", L"RenderTarget_LastAdpaterLumFilter1x1" ) ) )
	//	return E_FAIL;
	//if ( FAILED( m_pTarget_Manager->Add_MRT( L"MRT_Filter", L"RenderTarget_AdpaterLumFilter1x1" ) ) )
	//	return E_FAIL;



	// ---- For. Shadow Group..
	if ( FAILED( m_pTarget_Manager->Add_Target( m_pGraphicDev, L"RenderTarget_ShadowMap_Near", (_uint)SHADOWMAP_WIDTH[0], (_uint)SHADOWMAP_HEIGHT[0], D3DFMT_A16B16G16R16F, D3DXCOLOR( 1.f, 1.f, 1.f, 1.f ) ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_Target( m_pGraphicDev, L"RenderTarget_ShadowMap_Mid", (_uint)SHADOWMAP_WIDTH[1], (_uint)SHADOWMAP_HEIGHT[1], D3DFMT_A16B16G16R16F, D3DXCOLOR( 1.f, 1.f, 1.f, 1.f ) ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_Target( m_pGraphicDev, L"RenderTarget_ShadowMap_Near_X4", (_uint)(SHADOWMAP_WIDTH[0]), (_uint)(SHADOWMAP_HEIGHT[0]), D3DFMT_A16B16G16R16F, D3DXCOLOR( 1.f, 1.f, 1.f, 1.f ) ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_Target( m_pGraphicDev, L"RenderTarget_ShadowMap_Mid_X4", (_uint)(SHADOWMAP_WIDTH[1] / 4), (_uint)(SHADOWMAP_HEIGHT[1] / 4), D3DFMT_A16B16G16R16F, D3DXCOLOR( 1.f, 1.f, 1.f, 1.f ) ) ) )
		return E_FAIL;

	if ( FAILED( m_pTarget_Manager->Ready_DebugBuffer( L"RenderTarget_ShadowMap_Near", 800.f, 0.f, 200.f, 200.f ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Ready_DebugBuffer( L"RenderTarget_ShadowMap_Mid", 800.f, 200.f, 200.f, 200.f ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Ready_DebugBuffer( L"RenderTarget_ShadowMap_Near_X4", 800.f, 400.f, 200.f, 200.f ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Ready_DebugBuffer( L"RenderTarget_ShadowMap_Mid_X4", 800.f, 600.f, 200.f, 200.f ) ) )
		return E_FAIL;

	if ( FAILED( m_pTarget_Manager->Add_MRT( L"MRT_Shadow", L"RenderTarget_ShadowMap_Near" ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_MRT( L"MRT_Shadow", L"RenderTarget_ShadowMap_Mid" ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_MRT( L"MRT_Shadow", L"RenderTarget_ShadowMap_Near_X4" ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_MRT( L"MRT_Shadow", L"RenderTarget_ShadowMap_Mid_X4" ) ) )
		return E_FAIL;



	// ---- For. SSAO..
	if ( FAILED( m_pTarget_Manager->Add_Target( m_pGraphicDev, L"RenderTarget_SSAO", ViewPort.Width, ViewPort.Height, D3DFMT_R16F, D3DXCOLOR( 1.f, 1.f, 1.f, 1.f ) ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_Target( m_pGraphicDev, L"RenderTarget_Velocity", ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR( 0.f, 0.f, 0.f, 0.f ) ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_Target( m_pGraphicDev, L"RenderTarget_LightOcclusion", ViewPort.Width, ViewPort.Height, D3DFMT_A8R8G8B8, D3DXCOLOR( 0.f, 0.f, 0.f, 0.f ) ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_Target( m_pGraphicDev, L"RenderTarget_LightShaft", ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR( 0.f, 0.f, 0.f, 0.f ) ) ) )
		return E_FAIL;

	if ( FAILED( m_pTarget_Manager->Ready_DebugBuffer( L"RenderTarget_SSAO", 1000.f, 0.f, 200.f, 200.f ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Ready_DebugBuffer( L"RenderTarget_Velocity", 1000.f, 200.f, 200.f, 200.f ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Ready_DebugBuffer( L"RenderTarget_LightOcclusion", 1000.f, 400.f, 200.f, 200.f ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Ready_DebugBuffer( L"RenderTarget_LightShaft", 1000.f, 600.f, 200.f, 200.f ) ) )
		return E_FAIL;

	if ( FAILED( m_pTarget_Manager->Add_MRT( L"MRT_PostProcessing", L"RenderTarget_SSAO" ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_MRT( L"MRT_PostProcessing", L"RenderTarget_Velocity" ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_MRT( L"MRT_PostProcessing", L"RenderTarget_LightOcclusion" ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_MRT( L"MRT_PostProcessing", L"RenderTarget_LightShaft" ) ) )
		return E_FAIL;


	// ---- For. Closs Filter..
	if ( FAILED( m_pTarget_Manager->Add_Target( m_pGraphicDev, L"RenderTarget_Star0", ViewPort.Width / 4, ViewPort.Height / 4, D3DFMT_A16B16G16R16F, D3DXCOLOR( 0.f, 0.f, 0.f, 0.f ) ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_Target( m_pGraphicDev, L"RenderTarget_Star1", ViewPort.Width / 4, ViewPort.Height / 4, D3DFMT_A16B16G16R16F, D3DXCOLOR( 0.f, 0.f, 0.f, 0.f ) ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_Target( m_pGraphicDev, L"RenderTarget_Star2", ViewPort.Width / 4, ViewPort.Height / 4, D3DFMT_A16B16G16R16F, D3DXCOLOR( 0.f, 0.f, 0.f, 0.f ) ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_Target( m_pGraphicDev, L"RenderTarget_Star3", ViewPort.Width / 4, ViewPort.Height / 4, D3DFMT_A16B16G16R16F, D3DXCOLOR( 0.f, 0.f, 0.f, 0.f ) ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_Target( m_pGraphicDev, L"RenderTarget_Star4", ViewPort.Width / 4, ViewPort.Height / 4, D3DFMT_A16B16G16R16F, D3DXCOLOR( 0.f, 0.f, 0.f, 0.f ) ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_Target( m_pGraphicDev, L"RenderTarget_Star5", ViewPort.Width / 4, ViewPort.Height / 4, D3DFMT_A16B16G16R16F, D3DXCOLOR( 0.f, 0.f, 0.f, 0.f ) ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_Target( m_pGraphicDev, L"RenderTarget_Star6", ViewPort.Width / 4, ViewPort.Height / 4, D3DFMT_A16B16G16R16F, D3DXCOLOR( 0.f, 0.f, 0.f, 0.f ) ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_Target( m_pGraphicDev, L"RenderTarget_Star7", ViewPort.Width / 4, ViewPort.Height / 4, D3DFMT_A16B16G16R16F, D3DXCOLOR( 0.f, 0.f, 0.f, 0.f ) ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_Target( m_pGraphicDev, L"RenderTarget_Star8", ViewPort.Width / 4, ViewPort.Height / 4, D3DFMT_A16B16G16R16F, D3DXCOLOR( 0.f, 0.f, 0.f, 0.f ) ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_Target( m_pGraphicDev, L"RenderTarget_Star9", ViewPort.Width / 4, ViewPort.Height / 4, D3DFMT_A16B16G16R16F, D3DXCOLOR( 0.f, 0.f, 0.f, 0.f ) ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_Target( m_pGraphicDev, L"RenderTarget_Star10", ViewPort.Width / 4, ViewPort.Height / 4, D3DFMT_A16B16G16R16F, D3DXCOLOR( 0.f, 0.f, 0.f, 0.f ) ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_Target( m_pGraphicDev, L"RenderTarget_Star11", ViewPort.Width / 4, ViewPort.Height / 4, D3DFMT_A16B16G16R16F, D3DXCOLOR( 0.f, 0.f, 0.f, 0.f ) ) ) )
		return E_FAIL;

	if ( FAILED( m_pTarget_Manager->Ready_DebugBuffer( L"RenderTarget_Star0", 1700.f, 0.f, 50.f, 50.f ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Ready_DebugBuffer( L"RenderTarget_Star1", 1750.f, 0.f, 50.f, 50.f ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Ready_DebugBuffer( L"RenderTarget_Star2", 1700.f, 50.f, 50.f, 50.f ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Ready_DebugBuffer( L"RenderTarget_Star3", 1750.f, 50.f, 50.f, 50.f ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Ready_DebugBuffer( L"RenderTarget_Star4", 1700.f, 100.f, 50.f, 50.f ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Ready_DebugBuffer( L"RenderTarget_Star5", 1750.f, 100.f, 50.f, 50.f ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Ready_DebugBuffer( L"RenderTarget_Star6", 1700.f, 150.f, 50.f, 50.f ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Ready_DebugBuffer( L"RenderTarget_Star7", 1750.f, 150.f, 50.f, 50.f ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Ready_DebugBuffer( L"RenderTarget_Star8", 1700.f, 200.f, 50.f, 50.f ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Ready_DebugBuffer( L"RenderTarget_Star9", 1750.f, 200.f, 50.f, 50.f ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Ready_DebugBuffer( L"RenderTarget_Star10", 1700.f, 250.f, 50.f, 50.f ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Ready_DebugBuffer( L"RenderTarget_Star11", 1750.f, 250.f, 50.f, 50.f ) ) )
		return E_FAIL;

	if ( FAILED( m_pTarget_Manager->Add_MRT( L"MRT_STAR", L"RenderTarget_Star0" ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_MRT( L"MRT_STAR", L"RenderTarget_Star1" ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_MRT( L"MRT_STAR", L"RenderTarget_Star2" ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_MRT( L"MRT_STAR", L"RenderTarget_Star3" ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_MRT( L"MRT_STAR", L"RenderTarget_Star4" ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_MRT( L"MRT_STAR", L"RenderTarget_Star5" ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_MRT( L"MRT_STAR", L"RenderTarget_Star6" ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_MRT( L"MRT_STAR", L"RenderTarget_Star7" ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_MRT( L"MRT_STAR", L"RenderTarget_Star8" ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_MRT( L"MRT_STAR", L"RenderTarget_Star9" ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_MRT( L"MRT_STAR", L"RenderTarget_Star10" ) ) )
		return E_FAIL;
	if ( FAILED( m_pTarget_Manager->Add_MRT( L"MRT_STAR", L"RenderTarget_Star11" ) ) )
		return E_FAIL;

#endif

	//----------------------------------------------------------------------------------------------------------------------------------------------------------------------//
	// By.KJY
#ifndef KMH_MODIFY // By.KMH
	if (FAILED(m_pTarget_Manager->Add_Target(m_pGraphicDev, L"RenderTarget_FrameBuffer", ViewPort.Width, ViewPort.Height, D3DFMT_A8R8G8B8, D3DXCOLOR(0.f, 0.f, 1.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Ready_DebugBuffer(L"RenderTarget_FrameBuffer", 400.0f, 0.f, 200.f, 200.f)))
		return E_FAIL;
#endif

	if (FAILED(m_pTarget_Manager->Add_Target(m_pGraphicDev, L"Target_Distort", ViewPort.Width, ViewPort.Height, D3DFMT_A8R8G8B8, D3DXCOLOR(0.5f, 0.5f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Ready_DebugBuffer(L"Target_Distort", 1720.f, 880.f, 200.f, 200.f)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CRenderer::Ready_ShaderCom( void )
{
	// 필수셰이더를 컴파일하자.
#ifndef KMH_MODIFY // By.KMH
	m_pShader_Light = CShader::Create(m_pGraphicDev, L"../../Reference/Headers/Shader_Light.fx");
	if(nullptr == m_pShader_Light)
		return E_FAIL;

	m_pShader_Blend = CShader::Create(m_pGraphicDev, L"../../Reference/Headers/Shader_Blend.fx");
	if(nullptr == m_pShader_Blend)
		return E_FAIL;
#else
	m_pShaderArr[Shader_Deferred] = CShader::Create( m_pGraphicDev, L"../../Reference/Headers/Shader_Deferred.fx" );
	m_pShaderArr[Shader_Filter] = CShader::Create( m_pGraphicDev, L"../../Reference/Headers/Shader_Filter.fx" );
	m_pShaderArr[Shader_FXAA] = CShader::Create( m_pGraphicDev, L"../../Reference/Headers/Shader_FXAA.fx" );
	m_pShaderArr[Shader_HDR] = CShader::Create( m_pGraphicDev, L"../../Reference/Headers/Shader_HDR.fx" );
	m_pShaderArr[Shader_Light] = CShader::Create( m_pGraphicDev, L"../../Reference/Headers/Shader_Light.fx" );
	m_pShaderArr[Shader_PostProcess] = CShader::Create( m_pGraphicDev, L"../../Reference/Headers/Shader_PostProcess.fx" );
	m_pShaderArr[Shader_Shadow] = CShader::Create( m_pGraphicDev, L"../../Reference/Headers/Shader_Shadow.fx" );
	m_pShaderArr[Shader_SSDecal] = CShader::Create( m_pGraphicDev, L"../../Reference/Headers/Shader_SSDecal.fx" );
	m_pShaderArr[Shader_Effect] = (CShader*)CComponent_Manager::GetInstance()->Clone_Component(0, L"CShader_Effect");
	m_pShaderArr[Shader_Mesh] = (CShader*)CComponent_Manager::GetInstance()->Clone_Component(0, L"CShader_Mesh");
	//m_pShaderArr[Shader_Effect] = CShader::Create(m_pGraphicDev, L"../Bin/ShaderFiles/Shader_Effect.fx");
	//m_pShaderArr[Shader_Mesh] = CShader::Create(m_pGraphicDev, L"../Bin/ShaderFiles/Shader_Mesh.fx");

	for ( size_t i = 0; i < Shader_End; ++i )
	{
		if ( nullptr == m_pShaderArr[i] )
			return E_FAIL;

		m_pEffectArr[i] = m_pShaderArr[i]->Get_EffectHandle();

		if ( nullptr == m_pEffectArr[i] )
			return E_FAIL;

		m_pEffectArr[i]->AddRef();
	}
#endif

	// By.KJY
	m_pShader_Distort = CShader::Create(m_pGraphicDev, L"../../Reference/Headers/Shader_Distort.fx");
	if (nullptr == m_pShader_Distort)
		return E_FAIL;

	return NOERROR;
}

HRESULT CRenderer::Ready_Buffer( const D3DVIEWPORT9 & ViewPort )
{
#ifdef KMH_MODIFY // By.KMH - 뷰포트 버퍼를 일반 버퍼로 ( View 행렬과 직교 투영 행렬을 사용하여 화면 사이즈로 변환 )..
	if(FAILED(m_pGraphicDev->CreateVertexBuffer(sizeof(VTXTEX) * 4, 0, VTXTEX::FVF, D3DPOOL_MANAGED, &m_pVB, nullptr)))
		return E_FAIL;

	VTXTEX* pVertex = nullptr;

	m_pVB->Lock( 0, 0, (void**)&pVertex, 0 );

	pVertex[0].vPosition = _vec3( -0.5f, 0.5f, 0.f );
	pVertex[0].vTexUV = _vec2( 0.f, 0.f );

	pVertex[1].vPosition = _vec3( 0.5f, 0.5f, 0.f );
	pVertex[1].vTexUV = _vec2( 1.f, 0.f );

	pVertex[2].vPosition = _vec3( 0.5f, -0.5f, 0.f );
	pVertex[2].vTexUV = _vec2( 1.f, 1.f );

	pVertex[3].vPosition = _vec3( -0.5f, -0.5f, 0.f );
	pVertex[3].vTexUV = _vec2( 0.f, 1.f );

	m_pVB->Unlock();
#else
	if(FAILED(m_pGraphicDev->CreateVertexBuffer(sizeof(VTXTEX_VIEWPORT) * 4, 0, D3DFVF_XYZRHW | D3DFVF_TEX1, D3DPOOL_MANAGED, &m_pVB, nullptr)))
		return E_FAIL;

	VTXTEX_VIEWPORT*	pVertex = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	pVertex[0].vPosition = _vec4(0.0f - 0.5f, 0.0f - 0.5f, 0.f, 1.f);
	pVertex[0].vTexUV = _vec2(0.f, 0.f);

	pVertex[1].vPosition = _vec4(0.0f + ViewPort.Width - 0.5f, 0.0f - 0.5f, 0.f, 1.f);
	pVertex[1].vTexUV = _vec2(1.f, 0.f);

	pVertex[2].vPosition = _vec4(0.0f + ViewPort.Width - 0.5f, 0.0f + ViewPort.Height - 0.5f, 0.f, 1.f);
	pVertex[2].vTexUV = _vec2(1.f, 1.f);

	pVertex[3].vPosition = _vec4(0.0f - 0.5f, 0.0f + ViewPort.Height - 0.5f, 0.f, 1.f);
	pVertex[3].vTexUV = _vec2(0.f, 1.f);

	m_pVB->Unlock();
#endif

	if(FAILED(m_pGraphicDev->CreateIndexBuffer(sizeof(INDEX16) * 2, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, nullptr)))
		return E_FAIL;

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

HRESULT CRenderer::Ready_DefaultSetting( const D3DVIEWPORT9 & ViewPort )
{
	D3DXMatrixIdentity( &m_matOrthoView );

	m_matOrthoView._11 = (_float)ViewPort.Width;
	m_matOrthoView._22 = (_float)ViewPort.Height;
	m_matOrthoView._33 = 1.f;
	m_matOrthoView._41 = -0.5f - ViewPort.Width * 0.5f + m_matOrthoView._11 * 0.5f;
	m_matOrthoView._42 = -(-0.5f) + ViewPort.Height * 0.5f - m_matOrthoView._22 * 0.5f;

	D3DXMatrixOrthoLH( &m_matOrthoProj, (_float)ViewPort.Width, (_float)ViewPort.Height, 0.f, 1.f );

	for ( _uint i = 0; i < Cascade_End; ++i )
	{
		_float fOffsetX = 0.5f + ( 0.5f / (_float)SHADOWMAP_WIDTH[i] );
		_float fOffsetY = 0.5f + ( 0.5f / (_float)SHADOWMAP_HEIGHT[i] );
		_matrix mScaleBias(	0.5f,     0.0f,     0.0f,   0.0f,
							0.0f,    -0.5f,     0.0f,   0.0f,
							0.0f,     0.0f,     0.0f,	0.0f,
							fOffsetX, fOffsetY, 0.0f,   1.0f );

		m_matScaleBias[i] = mScaleBias;
	}

#ifdef SHADER_FULL_OPTION
	// 이거 주석 풀면 쉐이더 풀 옵션으로 시작함....
	for ( size_t i = 0; i < Shader_End; ++i )
		Set_ShaderOptionOnOff( Engine::CRenderer::SHADEROPTION( i ), true );
	Set_ShaderOptionOnOff( Engine::CRenderer::ShaderOption_DOF, false );
	Set_ShaderOptionOnOff( Engine::CRenderer::ShaderOption_FXAA, false );
	Set_ShaderOptionOnOff( Engine::CRenderer::ShaderOption_RainAmbience, false );
	Set_ShaderOptionOnOff( Engine::CRenderer::ShaderOption_LightShaft, false );
	//Set_ShaderOptionOnOff( Engine::CRenderer::ShaderOption_EdgeDetecting, false );
#endif

	if ( FAILED( D3DXCreateTextureFromFile( m_pGraphicDev, L"../Bin/Resources/Textures/Noise/SSAONoise.tga", &m_pRandomTexture ) ) )
	//if ( FAILED( D3DXCreateTextureFromFile( m_pGraphicDev, L"../Bin/Resources/Textures/Noise/RandomNormal2.tga", &m_pRandomTexture ) ) )
		return E_FAIL;
	if ( nullptr == m_pRandomTexture )
		return E_FAIL;

	if ( FAILED( D3DXCreateTextureFromFile( m_pGraphicDev, L"../Bin/Resources/Textures/LUT/LUT_Night.tga", &m_pLUTTexture ) ) )
		return E_FAIL;
	if ( nullptr == m_pLUTTexture )
		return E_FAIL;


	// 주변 16텍셀을 샘플링점으로 해서
	// 0.5는 중심에 맞추귀위한 보정
	int index=0;
	for( int y=0; y < 4; y++ ) {
		for( int x=0; x < 4; x++ ) {
			m_vSampleOffsetsArr[ index ].x = (x - 1.5f) / ViewPort.Width;
			m_vSampleOffsetsArr[ index ].y = (y - 1.5f) / ViewPort.Height;
			index++;
		}
	}


	// 필요한 컴포넌트를 받아온다..
	m_pFrustum = dynamic_cast<CFrustum*>(CComponent_Manager::GetInstance()->Clone_Component( 0, L"CFrustum" ));
	if ( nullptr == m_pFrustum )
		return E_FAIL;
	Engine::Safe_Release( m_pFrustum );

	// 그림자를 그릴 영역별 Projection 행렬을 만드는 작업..
	m_fCasCadeRange[Cascade_Level1] = 10.f;
	m_fCasCadeRange[Cascade_Level2] = 50.f;

	for ( _uint i = 0; i < Cascade_End; ++i )
	{
		D3DXMatrixOrthoLH( &m_matLightProj[i], m_fCasCadeRange[i], m_fCasCadeRange[i], 0.03f, 10000.f );
	}

	m_pGlareDef = new CGlareDef;
	m_pGlareDef->Initialize( GLT_FILTER_CROSSSCREEN );

	if ( nullptr == m_pDecal_Manager )
		return E_FAIL;

	m_pDecal_Manager->Set_ShaderCom( m_pShaderArr[Shader_SSDecal] );

	return NOERROR;
}

void CRenderer::Render_EdgeDection( void )
{
	BEGINRT( L"RenderTarget_EdgeDetecting" );

	//static _float g_fRimWidth = 0.2f;
	//static char szName[256] = "";
	//if ( GetAsyncKeyState( 'Z' ) )
	//	g_fRimWidth -= 0.1f / 30.f;
	//if ( GetAsyncKeyState( 'X' ) )
	//	g_fRimWidth += 0.1f / 30.f;
	//
	//sprintf_s( szName, "g_fRimWidth : %f", g_fRimWidth );
	//CDebug::GetInstance()->Add_DebugText( 1000.f, 800.f, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), szName );
	//m_pEffectArr[Shader_Deferred]->SetVector( "g_vRimLightData", &_vec4( 255.f / 255.f, 213.f / 255.f, 242.f / 255.f, g_fRimWidth ) );

	m_pEffectArr[Shader_Deferred]->SetFloat( "g_fCamFar", m_CamData.fCamFar );
	m_pEffectArr[Shader_Deferred]->SetVector( "g_vCamPos", &m_CamData.vPosition );
	m_pEffectArr[Shader_Deferred]->SetMatrix( "g_matInvView", &m_CamData.matInvView );
	m_pEffectArr[Shader_Deferred]->SetMatrix( "g_matInvProj", &m_CamData.matInvProj );

#ifndef VIEWPROJMATRIXSETTING_ONCE
	SettingViewProjMatrix( m_pEffectArr[Shader_Deferred] );
#endif

	SETRT( m_pEffectArr[Shader_Deferred], L"RenderTarget_Normal", "g_NormalTexture" );
	SETRT( m_pEffectArr[Shader_Deferred], L"RenderTarget_Depth", "g_DepthTexture" );

	Shader_BeginDrawEnd( m_pEffectArr[Shader_Deferred], "EdgeDection", 0 );

	ENDRT( L"RenderTarget_EdgeDetecting" );
}

void CRenderer::Render_FrameBuffer( void )
{
#ifndef VIEWPROJMATRIXSETTING_ONCE
	SettingViewProjMatrix( m_pEffectArr[Shader_Deferred] );
#endif

	if ( true == Get_ShaderOptionOnOff( ShaderOption_Light ) )
	{
		SETRT( m_pEffectArr[Shader_Deferred], L"RenderTarget_Color", "g_Src0Texture" );
		SETRT( m_pEffectArr[Shader_Deferred], L"RenderTarget_Shade", "g_Src1Texture" );
		SETRT( m_pEffectArr[Shader_Deferred], L"RenderTarget_Specular", "g_Src2Texture" );
		SETRT( m_pEffectArr[Shader_Deferred], L"RenderTarget_EdgeDetecting", "g_Src3Texture" );
	
		Shader_BeginDrawEnd( m_pEffectArr[Shader_Deferred], "LightRendering", 0 );
	}
	else
	{
		SETRT( m_pEffectArr[Shader_Deferred], L"RenderTarget_Color", "g_Src0Texture" );
		SETRT( m_pEffectArr[Shader_Deferred], L"RenderTarget_EdgeDetecting", "g_Src1Texture" );
	
		Shader_BeginDrawEnd( m_pEffectArr[Shader_Deferred], "NoLightRendering", 2 );
	}
}

void CRenderer::Calc_HDRProgress( void )
{
	Scene_To_ReductionBuffer();		// 축소 버퍼에 담음..
#ifdef USEAVGLUMINANCE
	Compute_AvgLuminance();			// 평균 휘도 구함..
#endif
	Extract_BrightPass();			// 강한 색 추출..
	ToneMapping();					// 톤 매핑..
}

void CRenderer::Scene_To_ReductionBuffer( void )
{
	// ---------------- 축소 버퍼에 Scene 버퍼를 담아둔다.. ---------------- //
	BEGINRT( L"RenderTarget_HDRFilter1_X4" );

	SETRT( m_pEffectArr[Shader_Filter], m_pLastRTName, "g_Src0Texture" );

#ifndef VIEWPROJMATRIXSETTING_ONCE
	SettingViewProjMatrix( m_pEffectArr[Shader_HDR] );
#endif

	m_pEffectArr[Shader_Filter]->SetVectorArray( "g_avSampleOffsets", m_vSampleOffsetsArr, 16 );

	Shader_BeginDrawEnd( m_pEffectArr[Shader_Filter], "DownFilter", 0 );

	ENDRT( L"RenderTarget_HDRFilter1_X4" );
	m_pLastRTName = L"RenderTarget_HDRFilter1_X4";
}

void CRenderer::Compute_AvgLuminance( void )
{
#ifndef VIEWPROJMATRIXSETTING_ONCE
	SettingViewProjMatrix( m_pEffectArr[Shader_Filter] );
#endif

	_tchar szName[3][128] = { L"RenderTarget_LumFilter64x64", L"RenderTarget_LumFilter16x16", L"RenderTarget_LumFilter4x4" };

	float tU, tV;
	int x, y, index;
	tU = 1.0f / ( 3.0f * 64.f );
	tV = 1.0f / ( 3.0f * 64.f );

	D3DXVECTOR4 avSampleOffsets[16];

	index = 0;
	for( x = -1; x <= 1; x++ )
	{
		for( y = -1; y <= 1; y++ )
		{
			avSampleOffsets[index].x = x * tU;
			avSampleOffsets[index].y = y * tV;

			index++;
		}
	}

	const _tchar* pTemp = m_pTempRTName;
	m_pTempRTName = m_pLastRTName;

	Shader_Begin( m_pEffectArr[Shader_Filter], "AvgLuminance" );

	// Scene Texture 에서 Luminance 를 추출한다..
	BEGINRT( szName[0] );

	m_pEffectArr[Shader_Filter]->SetVectorArray( "g_avSampleOffsets", avSampleOffsets, 16 );

	SETRT( m_pEffectArr[Shader_Filter], m_pLastRTName, "g_Src0Texture" );

	Shader_Draw( m_pEffectArr[Shader_Filter], 0 );

	ENDRT( szName[0] );


	// 추출한 Luminance 를 계속 다운 샘플링한다 ( 1x1 텍스쳐에 옮기기 전까지 )..
	_float fScale = 64.f;
	for ( _uint i = 1; i < 3; ++i )
	{
		GetSampleOffsets_DownScale4x4( (_ulong)fScale, (_ulong)fScale, avSampleOffsets );

		BEGINRT( szName[i] );

		SETRT( m_pEffectArr[Shader_Filter], szName[i - 1], "g_Src0Texture" );
		m_pEffectArr[Shader_Filter]->SetVectorArray( "g_avSampleOffsets", avSampleOffsets, 16 );

		m_pEffectArr[Shader_Filter]->CommitChanges();

		Shader_Draw( m_pEffectArr[Shader_Filter], 1 );

		ENDRT( szName[i] );

		fScale /= 4.f;
	}


	// 최종 Luminance 텍스쳐(1x1 사이즈의 텍스쳐) 에 옮긴다..
	GetSampleOffsets_DownScale4x4( (_ulong)fScale, (_ulong)fScale, avSampleOffsets );
	BEGINRT( L"RenderTarget_LumFilter1x1" );

	SETRT( m_pEffectArr[Shader_Filter], szName[2], "g_Src0Texture" );
	m_pEffectArr[Shader_Filter]->SetVectorArray( "g_avSampleOffsets", avSampleOffsets, 16 );

	Shader_Draw( m_pEffectArr[Shader_Filter], 2 );

	ENDRT( L"RenderTarget_LumFilter1x1" );


	// 전 프레임의 평균 루미넌스와 현재 루미넌스를 조합한다..
	BEGINRT( L"RenderTarget_AdpaterLumFilter1x1" );

	_float fTimeDelta = CTimer_Manager::GetInstance()->Get_MainGameTimer()->Get_TimeDelta();

	SETRT( m_pEffectArr[Shader_Filter], L"RenderTarget_LastAdpaterLumFilter1x1", "g_Src0Texture" );
	SETRT( m_pEffectArr[Shader_Filter], L"RenderTarget_LastLumFilter1x1", "g_Src1Texture" );

	m_pEffectArr[Shader_Filter]->SetFloat( "g_fElapsedTime", fTimeDelta );
	m_pEffectArr[Shader_Filter]->CommitChanges();

	Shader_Draw( m_pEffectArr[Shader_Filter], 3 );

	m_pEffectArr[Shader_Filter]->End();

	ENDRT( L"RenderTarget_AdpaterLumFilter1x1" );


	BEGINRT( L"RenderTarget_LastAdpaterLumFilter1x1" );

	SETRT( m_pEffectArr[Shader_Deferred], L"RenderTarget_AdpaterLumFilter1x1", "g_Src0Texture" );

	Shader_BeginDrawEnd( m_pEffectArr[Shader_Deferred], "NoLightRendering", 0 );

	ENDRT( L"RenderTarget_LastAdpaterLumFilter1x1" );
}

void CRenderer::Extract_BrightPass( void )
{
	// ---------------- 밝은 곳 추출하기.. ---------------- //
	// 출력할 곳의 정보로부터 렌더링할 크기 설정
	RECT rectDest = { 0, 0, 1920 / 4 + 2, 1080 / 4 + 2 };
	InflateRect( &rectDest, -1, -1 );
	m_pGraphicDev->SetRenderState( D3DRS_SCISSORTESTENABLE, TRUE );
	m_pGraphicDev->SetScissorRect( &rectDest );

	// 전체화면 복사
	BEGINRT( L"RenderTarget_BrightPass" );

	SETRT( m_pEffectArr[Shader_Filter], m_pLastRTName, "g_Src0Texture" );
#ifdef USEAVGLUMINANCE
	SETRT( m_pEffectArr[Shader_Filter], L"RenderTarget_LumFilter1x1", "g_Src1Texture" );
#endif

	Shader_BeginDrawEnd( m_pEffectArr[Shader_Filter], "BrightPass", 0 );

	m_pGraphicDev->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE );
	ENDRT( L"RenderTarget_BrightPass" );

	Filter_To_BrightPass( 2 );		// 추출한 색을 강하게 블러나 블룸 등을 먹인다..
	Render_Star();					// Cross Filter 그림..

	m_pTempRTName = L"RenderTarget_ResultBuffer";
}

void CRenderer::Filter_To_BrightPass( const _uint& _iBlurCount )
{
	// ---------------- Blur(or Bloom) 먹이기.. ---------------- //
	m_pLastRTName = L"RenderTarget_BrightPass";
	m_pTempRTName = L"RenderTarget_BrightPass2";

	Shader_Begin( m_pEffectArr[Shader_Filter], "GaussianFilter" );

	const _tchar* pTemp = nullptr;
	for ( _uint i = 0; i < _iBlurCount; ++i )
	{
		BEGINRT( m_pTempRTName );

		SETRT( m_pEffectArr[Shader_Filter], m_pLastRTName, "g_Src0Texture" );

		Shader_Draw( m_pEffectArr[Shader_Filter], i % 2 );

		ENDRT( m_pTempRTName );

		pTemp = m_pLastRTName;
		m_pLastRTName = m_pTempRTName;
		m_pTempRTName = pTemp;
	}

	m_pEffectArr[Shader_Filter]->End();
}

void CRenderer::Render_Star( void )
{
	int i, d, p, s;

	LPDIRECT3DTEXTURE9		m_pStarTex[12] = { nullptr };

	for ( _uint i = 0; i < 12; ++i )
	{
		_tchar szName[256] = L"";
		swprintf_s( szName, L"RenderTarget_Star%d", i );
		m_pStarTex[i] = m_pTarget_Manager->Get_Texture( szName );
	}

	LPDIRECT3DSURFACE9 pSurfStar = NULL;
	m_pStarTex[0]->GetSurfaceLevel( 0, &pSurfStar );

	// Clear the star texture
	m_pGraphicDev->ColorFill( pSurfStar, NULL, D3DCOLOR_ARGB( 0, 0, 0, 0 ) );
	Safe_Release( pSurfStar );

	// Initialize the constants used during the effect
	const CStarDef& starDef = m_pGlareDef->m_starDef;
	const float fTanFoV = atanf( D3DX_PI / 8 );
	const D3DXVECTOR4 vWhite( 1.0f, 1.0f, 1.0f, 1.0f );
	static const int s_maxPasses = 3;
	static const int nSamples = 8;
	static D3DXVECTOR4 s_aaColor[s_maxPasses][8];
	static const D3DXCOLOR s_colorWhite( 0.63f, 0.63f, 0.63f, 0.0f );

	D3DXVECTOR4 avSampleWeights[16];
	D3DXVECTOR4 avSampleOffsets[16];

	m_pGraphicDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
	m_pGraphicDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

	PDIRECT3DSURFACE9 pSurfSource = NULL;

	// Set aside all the star texture surfaces as a convenience
	PDIRECT3DSURFACE9 apSurfStar[12] = {0};
	for( i = 0; i < 12; i++ )
	{
		m_pStarTex[i]->GetSurfaceLevel( 0, &apSurfStar[i] );
	}

	// Get the source texture dimensions
	LPDIRECT3DTEXTURE9 pStarSourceTex = m_pTarget_Manager->Get_Texture( m_pLastRTName );

	pStarSourceTex->GetSurfaceLevel( 0, &pSurfSource );

	D3DSURFACE_DESC desc;
	pSurfSource->GetDesc( &desc );

	Safe_Release( pSurfSource );

	float srcW;
	srcW = ( FLOAT )desc.Width;
	float srcH;
	srcH = ( FLOAT )desc.Height;

	_tchar szName[256] = L"";

	for( p = 0; p < s_maxPasses; p ++ )
	{
		float ratio;
		ratio = ( float )( p + 1 ) / ( float )s_maxPasses;

		for( s = 0; s < nSamples; s ++ )
		{
			D3DXCOLOR chromaticAberrColor;
			D3DXColorLerp( &chromaticAberrColor,
						   &( CStarDef::GetChromaticAberrationColor( s ) ),
						   &s_colorWhite,
						   ratio );

			D3DXColorLerp( ( D3DXCOLOR* )&( s_aaColor[p][s] ),
						   &s_colorWhite, &chromaticAberrColor,
						   m_pGlareDef->m_fChromaticAberration );
		}
	}

	float radOffset;
	radOffset = m_pGlareDef->m_fStarInclination + starDef.m_fInclination;


	PDIRECT3DTEXTURE9 pTexSource;


	// Direction loop
	for( d = 0; d < starDef.m_nStarLines; d ++ )
	{
		CONST STARLINE& starLine = starDef.m_pStarLine[d];

		pTexSource = pStarSourceTex;

		float rad;
		rad = radOffset + starLine.fInclination;
		float sn, cs;
		sn = sinf( rad ), cs = cosf( rad );
		D3DXVECTOR2 vtStepUV;
		vtStepUV.x = sn / srcW * starLine.fSampleLength;
		vtStepUV.y = cs / srcH * starLine.fSampleLength;

		float attnPowScale;
		attnPowScale = ( fTanFoV + 0.1f ) * 1.0f *
			( 160.0f + 120.0f ) / ( srcW + srcH ) * 1.2f;

		// 1 direction expansion loop
		m_pGraphicDev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

		int iWorkTexture;
		iWorkTexture = 1;
		for( p = 0; p < starLine.nPasses; p ++ )
		{
			if( p == starLine.nPasses - 1 )
			{
				// Last pass move to other work buffer
				swprintf_s( szName, L"RenderTarget_Star%d", d + 4 );
			}
			else
			{
				swprintf_s( szName, L"RenderTarget_Star%d", iWorkTexture );
			}

			// Sampling configration for each stage
			for( i = 0; i < nSamples; i ++ )
			{
				float lum;
				lum = powf( starLine.fAttenuation, attnPowScale * i );

				avSampleWeights[i] = s_aaColor[starLine.nPasses - 1 - p][i] *
					lum * ( p + 1.0f ) * 0.5f;


				// Offset of sampling coordinate
				avSampleOffsets[i].x = vtStepUV.x * i;
				avSampleOffsets[i].y = vtStepUV.y * i;
				if( fabs( avSampleOffsets[i].x ) >= 0.9f ||
					fabs( avSampleOffsets[i].y ) >= 0.9f )
				{
					avSampleOffsets[i].x = 0.0f;
					avSampleOffsets[i].y = 0.0f;
					avSampleWeights[i] *= 0.0f;
				}

			}


			m_pEffectArr[Shader_Filter]->SetVectorArray( "g_avSampleOffsets", avSampleOffsets, 16 );
			m_pEffectArr[Shader_Filter]->SetVectorArray( "g_avSampleWeights", avSampleWeights, nSamples );

			BEGINRT( szName );

			m_pGraphicDev->SetTexture( 0, pTexSource );

			m_pEffectArr[Shader_Filter]->CommitChanges();

			Shader_BeginDrawEnd( m_pEffectArr[Shader_Filter], "ClossFilter", 0 );

			ENDRT( szName );

			// Setup next expansion
			vtStepUV *= (_float)nSamples;
			attnPowScale *= nSamples;

			// Set the work drawn just before to next texture source.
			pTexSource = m_pStarTex[iWorkTexture];

			iWorkTexture += 1;
			if( iWorkTexture > 2 )
			{
				iWorkTexture = 1;
			}

		}
	}

	for( i = 0; i < starDef.m_nStarLines; i++ )
	{
		m_pGraphicDev->SetTexture( i, m_pStarTex[i + 4] );

		avSampleWeights[i] = vWhite * 1.0f / ( FLOAT )starDef.m_nStarLines;
	}

	m_pEffectArr[Shader_Filter]->SetVectorArray( "g_avSampleWeights", avSampleWeights, starDef.m_nStarLines );

	BEGINRT( L"RenderTarget_Star0" );

	m_pEffectArr[Shader_Filter]->CommitChanges();

	Shader_BeginDrawEnd( m_pEffectArr[Shader_Filter], "ClossFilter", 1 );

	ENDRT( L"RenderTarget_Star0" );

	for( i = 0; i < starDef.m_nStarLines; i++ )
		m_pGraphicDev->SetTexture( i, NULL );

	for( i = 0; i < 12; i++ )
	{
		Safe_Release( apSurfStar[i] );
	}
}

void CRenderer::ToneMapping( void )
{
#ifndef VIEWPROJMATRIXSETTING_ONCE
	SettingViewProjMatrix( m_pEffectArr[Shader_Filter] );
#endif

	BEGINRT( m_pTempRTName );

	SETRT( m_pEffectArr[Shader_Filter], L"RenderTarget_FrameBuffer", "g_Src0Texture" );
	SETRT( m_pEffectArr[Shader_Filter], L"RenderTarget_Luminance1x1", "g_Src1Texture" );
	SETRT( m_pEffectArr[Shader_Filter], m_pLastRTName, "g_Src2Texture" );

	Shader_BeginDrawEnd( m_pEffectArr[Shader_Filter], "Tonemap", 0 );

	ENDRT( m_pTempRTName );

	m_pLastRTName = L"RenderTarget_FrameBuffer";
}
#endif

void CRenderer::Render_LightOcclusion( void )
{
#ifndef VIEWPROJMATRIXSETTING_ONCE
	SettingViewProjMatrix( m_pEffectArr[Shader_PostProcess] );
#endif

	BEGINRT( L"RenderTarget_LightOcclusion" );

	SETRT( m_pEffectArr[Shader_PostProcess], m_pLastRTName, "g_FrameBufferTexture" );

	Shader_BeginDrawEnd( m_pEffectArr[Shader_PostProcess], "LightShaft", 0 );

	ENDRT( L"RenderTarget_LightOcclusion" );
}

void CRenderer::Render_SumBuffer( void )
{
#ifndef RENDEREREFFECTHDR
	// Render Priority Group..
#ifndef VIEWPROJMATRIXSETTING_ONCE
	SettingViewProjMatrix( m_pEffectArr[Shader_Deferred] );
#endif

	SETRT( m_pEffectArr[Shader_Deferred], L"RenderTarget_Priority", "g_Src0Texture" );

	Shader_Begin( m_pEffectArr[Shader_Deferred], nullptr, "NoLightRendering" );
	Shader_Draw( m_pEffectArr[Shader_Deferred], 3 );
#endif


	// Render Screen Buffer..
	SETRT( m_pEffectArr[Shader_Deferred], m_pLastRTName, "g_Src0Texture" );

	m_pEffectArr[Shader_Deferred]->CommitChanges();

#ifdef RENDEREREFFECTHDR
	Shader_BeginDrawEnd( m_pEffectArr[Shader_Deferred], nullptr, "NoLightRendering", 0 );
#else
	Shader_Draw( m_pEffectArr[Shader_Deferred], 1 );
#endif

	m_pEffectArr[Shader_Deferred]->End();
}

void CRenderer::Calc_PostProcessing( void )
{
	if ( true == Get_ShaderOptionOnOff( ShaderOption_SSAO ) )
		Compute_SSAO();

	if ( true == Get_ShaderOptionOnOff( ShaderOption_DOF ) )
		Compute_DOF();

	if ( true == Get_ShaderOptionOnOff( ShaderOption_Fog ) )
		Compute_Fog();

	if ( true == Get_ShaderOptionOnOff( ShaderOption_MotionBlur ) )	// 최종 결과 타겟을 흔들어야함?????
		Compute_MotionBlur();

	if ( true == Get_ShaderOptionOnOff( ShaderOption_LightShaft ) )
		Compute_LightShaft();

	//if ( true == Get_ShaderOptionOnOff( ShaderOption_RainAmbience ) )	// 비 온 느낌을 주기 위해 추가..
	//	Compute_RainAmbience();
}

void CRenderer::Compute_MotionBlur( void )
{
#ifndef VIEWPROJMATRIXSETTING_ONCE
	SettingViewProjMatrix( m_pEffectArr[Shader_PostProcess] );
#endif

#ifdef USEVELOCITYTARGET
	// 속도 버퍼를 만든다..
	BEGINRT( L"RenderTarget_Velocity" );

	SETRT( m_pEffectArr[Shader_PostProcess], L"RenderTarget_Depth", "g_DepthTexture" );
	SETRT( m_pEffectArr[Shader_PostProcess], L"RenderTarget_Normal", "g_NormalTexture" );

	m_pEffectArr[Shader_PostProcess]->SetFloat( "g_fCamFar", m_CamData.fCamFar );
	m_pEffectArr[Shader_PostProcess]->SetMatrix( "g_matInvView", &m_CamData.matInvView );
	m_pEffectArr[Shader_PostProcess]->SetMatrix( "g_matInvProj", &m_CamData.matInvProj );

	m_pEffectArr[Shader_PostProcess]->SetMatrix( "g_matCamView", &m_CamData.matView );
	m_pEffectArr[Shader_PostProcess]->SetMatrix( "g_matCamProj", &m_CamData.matProj );
	m_pEffectArr[Shader_PostProcess]->SetMatrix( "g_matPrevView", &m_CamData.matPrevView );

	m_pEffectArr[Shader_PostProcess]->CommitChanges();

	Shader_BeginDrawEnd( m_pEffectArr[Shader_PostProcess], "MotionBlur", 0 );

	ENDRT( L"RenderTarget_Velocity" );


	// 속도 버퍼를 가지고 모션 블러 수행..
	BEGINRT( m_pTempRTName );

	SETRT( m_pEffectArr[Shader_PostProcess], L"RenderTarget_Velocity", "g_Src1Texture" );
	SETRT( m_pEffectArr[Shader_PostProcess], m_pLastRTName, "g_FrameBufferTexture" );

	m_pEffectArr[Shader_PostProcess]->CommitChanges();

	Shader_BeginDrawEnd( m_pEffectArr[Shader_PostProcess], "MotionBlur", 1 );

	ENDRT( m_pTempRTName );
#else
	// 모션 블러 수행..
	BEGINRT( m_pTempRTName );

	SETRT( m_pEffectArr[Shader_PostProcess], L"RenderTarget_Depth", "g_DepthTexture" );
	SETRT( m_pEffectArr[Shader_PostProcess], L"RenderTarget_Normal", "g_NormalTexture" );
	SETRT( m_pEffectArr[Shader_PostProcess], m_pLastRTName, "g_FrameBufferTexture" );

	m_pEffectArr[Shader_PostProcess]->SetFloat( "g_fCamFar", m_CamData.fCamFar );
	m_pEffectArr[Shader_PostProcess]->SetMatrix( "g_matInvView", &m_CamData.matInvView );
	m_pEffectArr[Shader_PostProcess]->SetMatrix( "g_matInvProj", &m_CamData.matInvProj );

	m_pEffectArr[Shader_PostProcess]->SetMatrix( "g_matCamView", &m_CamData.matView );
	m_pEffectArr[Shader_PostProcess]->SetMatrix( "g_matCamProj", &m_CamData.matProj );
	m_pEffectArr[Shader_PostProcess]->SetMatrix( "g_matPrevView", &m_CamData.matPrevView );

	m_pEffectArr[Shader_PostProcess]->CommitChanges();

	Shader_BeginDrawEnd( m_pEffectArr[Shader_PostProcess], "MotionBlur", 0 );

	ENDRT( m_pTempRTName );
#endif

	const _tchar* pTemp = m_pTempRTName;
	m_pTempRTName = m_pLastRTName;
	m_pLastRTName = pTemp;
}

void CRenderer::Compute_RainAmbience( void )
{
#ifndef VIEWPROJMATRIXSETTING_ONCE
	SettingViewProjMatrix( m_pEffectArr[Shader_PostProcess] );
#endif

	_uint iBloomCount = 2;
	for ( _uint i = 0; i < iBloomCount; ++i )
	{
		BEGINRT( m_pTempRTName );

		SETRT( m_pEffectArr[Shader_Filter], m_pLastRTName, "g_Src0Texture" );

		m_pEffectArr[Shader_Filter]->CommitChanges();

		Shader_BeginDrawEnd( m_pEffectArr[Shader_Filter], "GaussianFilter", i % 2 );

		ENDRT( m_pTempRTName );

		const _tchar* pTemp = m_pTempRTName;
		m_pTempRTName = m_pLastRTName;
		m_pLastRTName = pTemp;
	}
}

void CRenderer::Compute_SSAO( void )
{
#ifndef VIEWPROJMATRIXSETTING_ONCE
	SettingViewProjMatrix( m_pEffectArr[Shader_Deferred] );
#endif

	BEGINRT( L"RenderTarget_SSAO" );

	SETRT( m_pEffectArr[Shader_Deferred], L"RenderTarget_Depth", "g_DepthTexture" );

	m_pEffectArr[Shader_Deferred]->SetTexture( "randomTexture", m_pRandomTexture );
	m_pEffectArr[Shader_Deferred]->SetMatrix( "Projection", &m_CamData.matProj );
	m_pEffectArr[Shader_Deferred]->SetMatrix( "g_matInvProj", &m_CamData.matInvProj );

	Shader_BeginDrawEnd( m_pEffectArr[Shader_Deferred], "SSAO", 0 );

	ENDRT( L"RenderTarget_SSAO" );

	//BEGINRT( L"RenderTarget_SSAO" );
	//
	//SETRT( m_pEffectArr[Shader_Filter], L"RenderTarget_Filter1_X4", "g_Src0Texture" );
	//
	//Shader_BeginDrawEnd( m_pEffectArr[Shader_Filter], "DownFilter", 1 );
	//
	//ENDRT( L"RenderTarget_SSAO" );
}

void CRenderer::Compute_DOF( void )
{
	// ------------- 뭉개는 중.... ------------- //
#ifndef VIEWPROJMATRIXSETTING_ONCE
	SettingViewProjMatrix( m_pEffectArr[Shader_Filter] );
#endif

	Shader_Begin( m_pEffectArr[Shader_Filter], "DownFilter" );

	m_pEffectArr[Shader_Filter]->SetMatrix( "g_matCamProj", &m_CamData.matProj );

	BEGINRT( L"RenderTarget_Filter1_X4" );

	SETRT( m_pEffectArr[Shader_Filter], m_pLastRTName, "g_Src0Texture" );

	m_pEffectArr[Shader_Filter]->CommitChanges();

	Shader_Draw( m_pEffectArr[Shader_Filter], 0 );

	ENDRT( L"RenderTarget_Filter1_X4" );


	// ------------- 계속 뭉개는 중.... ------------- //
	_ubyte iCrushCount = 1;
	for ( size_t i = 1; i < iCrushCount; ++i )
	{
		if ( i % 2 == 0 )
		{
			BEGINRT( L"RenderTarget_Filter1_X4" );
			SETRT( m_pEffectArr[Shader_Filter], L"RenderTarget_Filter2_X4", "g_Src0Texture" );

			m_pEffectArr[Shader_Filter]->CommitChanges();

			Shader_Draw( m_pEffectArr[Shader_Filter], 0 );

			ENDRT( L"RenderTarget_Filter1_X4" );
		}
		else
		{
			BEGINRT( L"RenderTarget_Filter2_X4" );
			SETRT( m_pEffectArr[Shader_Filter], L"RenderTarget_Filter1_X4", "g_Src0Texture" );

			m_pEffectArr[Shader_Filter]->CommitChanges();

			Shader_Draw( m_pEffectArr[Shader_Filter], 0 );

			ENDRT( L"RenderTarget_Filter2_X4" );
		}
	}

	m_pEffectArr[Shader_Filter]->End();


	// ------------- 뭉갠 텍스쳐와 원본 텍스쳐를 조합하여 DOF를 만든다.... ------------- //
#ifndef VIEWPROJMATRIXSETTING_ONCE
	SettingViewProjMatrix( m_pEffectArr[Shader_PostProcess] );
#endif
	m_pEffectArr[Shader_PostProcess]->SetFloat( "g_fCamFar", m_CamData.fCamFar );

	if ( true == Get_ShaderOptionOnOff( ShaderOption_HDR ) )
		SETRT( m_pEffectArr[Shader_PostProcess], m_pLastRTName, "g_Src0Texture" );
	else
		SETRT( m_pEffectArr[Shader_PostProcess], L"RenderTarget_FrameBuffer", "g_Src0Texture" );

	SETRT( m_pEffectArr[Shader_PostProcess], ((iCrushCount % 2 == 0) ? L"RenderTarget_Filter2_X4" : L"RenderTarget_Filter1_X4"), "g_CrushTexture" );
	SETRT( m_pEffectArr[Shader_PostProcess], L"RenderTarget_Depth", "g_DepthTexture" );

	BEGINRT( m_pTempRTName );

	m_pEffectArr[Shader_PostProcess]->CommitChanges();

	Shader_BeginDrawEnd( m_pEffectArr[Shader_PostProcess], "DOF", 0 );

	ENDRT( m_pTempRTName );

	const _tchar* pTemp = m_pTempRTName;
	m_pTempRTName = m_pLastRTName;
	m_pLastRTName = pTemp;
}

void CRenderer::Compute_Fog( void )
{
	BEGINRT( m_pTempRTName );

	SETRT( m_pEffectArr[Shader_Deferred], L"RenderTarget_Depth", "g_DepthTexture" );
	SETRT( m_pEffectArr[Shader_Deferred], m_pLastRTName, "g_Src0Texture" );

	m_pEffectArr[Shader_Deferred]->SetMatrix( "g_matInvProj", &m_CamData.matInvProj );

	Shader_BeginDrawEnd( m_pEffectArr[Shader_Deferred], "FogRendering", 0 );

	ENDRT( m_pTempRTName );

	const _tchar* pTemp = m_pTempRTName;
	m_pTempRTName = m_pLastRTName;
	m_pLastRTName = pTemp;
}

void CRenderer::Compute_LightShaft( void )
{
#ifndef VIEWPROJMATRIXSETTING_ONCE
	SettingViewProjMatrix( m_pEffectArr[Shader_Deferred] );
#endif

	// 디버깅 관련..
	{
		static _vec4 g_vLightShaftValue( 1.f, 1.f, 1.f, 1.f );
		static _vec4 g_vLightShaftValue2( 1920.f / 4.f, 1080.f / 4.f, 1.f, 1.f );
		static _vec4 g_vSunData( 0.f, 0.f, 0.f, 0.f );
		static _float g_iNumSamples = 100.f;

		if ( CInput_Device::GetInstance()->Get_DIKeyState( DIK_Z ) )
		{
			if ( CInput_Device::GetInstance()->Get_DIKeyState( DIK_F5 ) )
			{
				g_vLightShaftValue.x -= 1.f / 30.f;
			}
			if ( CInput_Device::GetInstance()->Get_DIKeyState( DIK_F6 ) )
			{
				g_vLightShaftValue.x += 1.f / 30.f;
			}
			if ( CInput_Device::GetInstance()->Get_DIKeyState( DIK_F7 ) )
			{
				g_vLightShaftValue.y -= 1.f / 30.f;
			}
			if ( CInput_Device::GetInstance()->Get_DIKeyState( DIK_F8 ) )
			{
				g_vLightShaftValue.y += 1.f / 30.f;
			}
			if ( CInput_Device::GetInstance()->Get_DIKeyState( DIK_F9 ) )
			{
				g_vLightShaftValue.z -= 1.f / 30.f;
			}
			if ( CInput_Device::GetInstance()->Get_DIKeyState( DIK_F10 ) )
			{
				g_vLightShaftValue.z += 1.f / 30.f;
			}
			if ( CInput_Device::GetInstance()->Get_DIKeyState( DIK_F11 ) )
			{
				g_vLightShaftValue.w -= 1.f / 30.f;
			}
			if ( CInput_Device::GetInstance()->Get_DIKeyState( DIK_F12 ) )
			{
				g_vLightShaftValue.w += 1.f / 30.f;
			}
		}
		else if ( CInput_Device::GetInstance()->Get_DIKeyState( DIK_X ) )
		{
			if ( CInput_Device::GetInstance()->Get_DIKeyState( DIK_F5 ) )
			{
				g_vLightShaftValue2.z -= 1.f / 30.f;
			}
			if ( CInput_Device::GetInstance()->Get_DIKeyState( DIK_F6 ) )
			{
				g_vLightShaftValue2.z += 1.f / 30.f;
			}
			if ( CInput_Device::GetInstance()->Get_DIKeyState( DIK_F7 ) )
			{
				g_iNumSamples -= 5.f / 30.f;
			}
			if ( CInput_Device::GetInstance()->Get_DIKeyState( DIK_F8 ) )
			{
				g_iNumSamples += 5.f / 30.f;
			}
		}
		else if ( CInput_Device::GetInstance()->Get_DIKeyState( DIK_C ) )
		{
			if ( CInput_Device::GetInstance()->Get_DIKeyState( DIK_F5 ) )
			{
				g_vSunData.x -= 1.f / 30.f;
			}
			if ( CInput_Device::GetInstance()->Get_DIKeyState( DIK_F6 ) )
			{
				g_vSunData.x += 1.f / 30.f;
			}
			if ( CInput_Device::GetInstance()->Get_DIKeyState( DIK_F7 ) )
			{
				g_vSunData.y -= 1.f / 30.f;
			}
			if ( CInput_Device::GetInstance()->Get_DIKeyState( DIK_F8 ) )
			{
				g_vSunData.y += 1.f / 30.f;
			}
			if ( CInput_Device::GetInstance()->Get_DIKeyState( DIK_F9 ) )
			{
				g_vSunData.z -= 1.f / 30.f;
			}
			if ( CInput_Device::GetInstance()->Get_DIKeyState( DIK_F10 ) )
			{
				g_vSunData.z += 1.f / 30.f;
			}
			if ( CInput_Device::GetInstance()->Get_DIKeyState( DIK_F11 ) )
			{
				g_vSunData.w -= 1.f / 30.f;
			}
			if ( CInput_Device::GetInstance()->Get_DIKeyState( DIK_F12 ) )
			{
				g_vSunData.w += 1.f / 30.f;
			}
		}

		m_pEffectArr[Shader_PostProcess]->SetVector( "g_vLightShaftValue", &g_vLightShaftValue );
		m_pEffectArr[Shader_PostProcess]->SetVector( "g_vLightShaftValue2", &g_vLightShaftValue2 );
		m_pEffectArr[Shader_PostProcess]->SetVector( "g_vSunData", &g_vSunData );
		m_pEffectArr[Shader_PostProcess]->SetInt( "g_iNumSamples", (_int)g_iNumSamples );

		_float fStartX = 1300.f;
		_float fStartY = 400.f;
		_float fHeight = 30.f;
		_uint iCount = 0;

		static char szName[256] = "";
		sprintf_s( szName, "(Density) - Up : Z + F5, Down : Z + F6" );
		CDebug::GetInstance()->Add_DebugText( fStartX, fStartY + fHeight * (_float)iCount++, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), szName );

		static char szName1[256] = "";
		sprintf_s( szName1, "(Decay) - Up : Z + F7, Down : Z + F8" );
		CDebug::GetInstance()->Add_DebugText( fStartX, fStartY + fHeight * (_float)iCount++, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), szName1 );

		static char szName2[256] = "";
		sprintf_s( szName2, "(Weight) - Up : Z + F9, Down : Z + F10" );
		CDebug::GetInstance()->Add_DebugText( fStartX, fStartY + fHeight * (_float)iCount++, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), szName2 );

		static char szName3[256] = "";
		sprintf_s( szName3, "(Exposure) - Up : Z + F11, Down : Z + F12" );
		CDebug::GetInstance()->Add_DebugText( fStartX, fStartY + fHeight * (_float)iCount++, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), szName3 );

		static char szName4[256] = "";
		sprintf_s( szName4, "(IlluminationDecay) - Up : X + F5, Down : X + F6" );
		CDebug::GetInstance()->Add_DebugText( fStartX, fStartY + fHeight * (_float)iCount++, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), szName4 );

		static char szName5[256] = "";
		sprintf_s( szName5, "Loop Count - Up : X + F7, Down : X + F8" );
		CDebug::GetInstance()->Add_DebugText( fStartX, fStartY + fHeight * (_float)iCount++, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), szName5 );

		static char szName6[256] = "";
		sprintf_s( szName6, "Sun Screen Pos(X) - Up : C + F5, Down : C + F6" );
		CDebug::GetInstance()->Add_DebugText( fStartX, fStartY + fHeight * (_float)iCount++, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), szName6 );

		static char szName7[256] = "";
		sprintf_s( szName7, "Sun Screen Pos(Y) - Up : C + F7, Down : C + F8" );
		CDebug::GetInstance()->Add_DebugText( fStartX, fStartY + fHeight * (_float)iCount++, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), szName7 );

		static char szName8[256] = "";
		sprintf_s( szName8, "Sun Dir(X) - Up : C + F9, Down : C + F10" );
		CDebug::GetInstance()->Add_DebugText( fStartX, fStartY + fHeight * (_float)iCount++, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), szName8 );

		static char szName9[256] = "";
		sprintf_s( szName9, "Sun Dir(Y) - Up : C + F11, Down : C + F12" );
		CDebug::GetInstance()->Add_DebugText( fStartX, fStartY + fHeight * (_float)iCount++, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), szName9 );

		static char szName10[256] = "";
		sprintf_s( szName10, "(Density) : %f", g_vLightShaftValue.x );
		CDebug::GetInstance()->Add_DebugText( fStartX, fStartY + fHeight * (_float)iCount++, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), szName10 );

		static char szName11[256] = "";
		sprintf_s( szName11, "(Decay) : %f", g_vLightShaftValue.y );
		CDebug::GetInstance()->Add_DebugText( fStartX, fStartY + fHeight * (_float)iCount++, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), szName11 );

		static char szName12[256] = "";
		sprintf_s( szName12, "(Weight) : %f", g_vLightShaftValue.z );
		CDebug::GetInstance()->Add_DebugText( fStartX, fStartY + fHeight * (_float)iCount++, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), szName12 );

		static char szName13[256] = "";
		sprintf_s( szName13, "(Exposure) : %f", g_vLightShaftValue.w );
		CDebug::GetInstance()->Add_DebugText( fStartX, fStartY + fHeight * (_float)iCount++, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), szName13 );

		static char szName14[256] = "";
		sprintf_s( szName14, "(IlluminationDecay) : %f", g_vLightShaftValue2.z );
		CDebug::GetInstance()->Add_DebugText( fStartX, fStartY + fHeight * (_float)iCount++, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), szName14 );

		static char szName15[256] = "";
		sprintf_s( szName15, "Loop Count : %d", (_int)g_iNumSamples );
		CDebug::GetInstance()->Add_DebugText( fStartX, fStartY + fHeight * (_float)iCount++, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), szName15 );

		static char szName16[256] = "";
		sprintf_s( szName16, "Sun Screen Pos : { %f, %f }", g_vSunData.x, g_vSunData.y );
		CDebug::GetInstance()->Add_DebugText( fStartX, fStartY + fHeight * (_float)iCount++, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), szName16 );

		static char szName17[256] = "";
		sprintf_s( szName17, "Sun Dir : { %f, %f }", g_vSunData.z, g_vSunData.w );
		CDebug::GetInstance()->Add_DebugText( fStartX, fStartY + fHeight * (_float)iCount++, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), szName17 );
	}

	// Occlusion 타겟을 축소 버퍼에 담는다..
	BEGINRT( L"RenderTarget_Filter1_X4" );

	SETRT( m_pEffectArr[Shader_Filter], L"RenderTarget_LightOcclusion", "g_Src0Texture" );

	Shader_BeginDrawEnd( m_pEffectArr[Shader_Filter], "DownFilter", 0 );

	ENDRT( L"RenderTarget_Filter1_X4" );


	// Light Shaft 렌더..
	BEGINRT( L"RenderTarget_Filter2_X4" );
	
	SETRT( m_pEffectArr[Shader_PostProcess], L"RenderTarget_Filter1_X4", "g_FrameBufferTexture" );

	Shader_BeginDrawEnd( m_pEffectArr[Shader_PostProcess], "LightShaft", 1 );
	
	ENDRT( L"RenderTarget_Filter2_X4" );


	// 축소 버퍼에서 다시 원본으로 옮기기..
	BEGINRT( L"RenderTarget_LightShaft" );

	SETRT( m_pEffectArr[Shader_Filter], L"RenderTarget_Filter2_X4", "g_Src0Texture" );

	Shader_BeginDrawEnd( m_pEffectArr[Shader_Filter], "DownFilter", 1 );

	ENDRT( L"RenderTarget_LightShaft" );


	// Light Shaft 의 결과를 Scene Buffer 에 적용..
	BEGINRT( m_pTempRTName );

	SETRT( m_pEffectArr[Shader_PostProcess], L"RenderTarget_LightShaft", "g_Src0Texture" );
	SETRT( m_pEffectArr[Shader_PostProcess], m_pLastRTName, "g_FrameBufferTexture" );

	Shader_BeginDrawEnd( m_pEffectArr[Shader_PostProcess], "LightShaft", 2 );

	ENDRT( m_pTempRTName );

	const _tchar* pTemp = m_pTempRTName;
	m_pTempRTName = m_pLastRTName;
	m_pLastRTName = pTemp;
}

void CRenderer::Render_Final( void )
{
#ifdef RENDEREREFFECTHDR
	// Priority Group Render..
#ifndef VIEWPROJMATRIXSETTING_ONCE
	SettingViewProjMatrix( m_pEffectArr[Shader_Deferred] );
#endif

	SETRT( m_pEffectArr[Shader_Deferred], L"RenderTarget_Priority", "g_Src0Texture" );
	Shader_BeginDrawEnd( m_pEffectArr[Shader_Deferred], "PriorityRendering", bFogOn );
#endif

#ifndef VIEWPROJMATRIXSETTING_ONCE
	SettingViewProjMatrix( m_pEffectArr[Shader_Deferred] );
#endif

	BEGINRT( m_pTempRTName );

	// ----------------- Debug Target Render.. ----------------- //
	if ( RenderBackBuffer_DebugRenderTarget == m_iPassNum )
	{
#ifdef RENDERERDEBUGING
		m_pEffectArr[Shader_Deferred]->SetInt( "g_iDebugOption", m_iDebugTargetOption );
#endif
		SETRT( m_pEffectArr[Shader_Deferred], m_pDebugRenderTargetTag, "g_Src0Texture" );
		Shader_BeginDrawEnd( m_pEffectArr[Shader_Deferred], "Debug", 0 );
	}
	else
	{
		// ----------------- Final Target Render.. ----------------- //
		SETRT( m_pEffectArr[Shader_Deferred], m_pLastRTName, "g_Src0Texture" );
		//if ( GetAsyncKeyState( VK_SPACE ) )
		//	m_pEffectArr[Shader_Deferred]->SetTexture( "g_Src1Texture", m_pLUTTexture );

#ifdef RENDEREREFFECTHDR
		Shader_BeginDrawEnd( m_pEffectArr[Shader_Deferred], "ColorGrading", 1 );
#else
		//if ( GetAsyncKeyState( VK_SPACE ) )
		//	Shader_BeginDrawEnd( m_pEffectArr[Shader_Deferred], "ColorGrading", 0 );
		//else
			Shader_BeginDrawEnd( m_pEffectArr[Shader_Deferred], "NoLightRendering", 0 );
#endif
	}

	ENDRT( m_pTempRTName );
}

void CRenderer::Render_FXAA( void )
{
	// 안티 에일리어싱 적용..
	BEGINRT( m_pTempRTName );

	SETRT( m_pEffectArr[Shader_FXAA], m_pLastRTName, "g_ScreenTexture" );
	SETRT( m_pEffectArr[Shader_FXAA], m_pLastRTName, "g_LumaTexture" );

	Shader_BeginDrawEnd( m_pEffectArr[Shader_FXAA], "Default", 0 );

	ENDRT( m_pTempRTName );
}

void CRenderer::Render_BackBuffer( void )
{
	// 최종 버퍼를 백버퍼에 그리는 작업..
	SETRT( m_pEffectArr[Shader_Deferred], m_pLastRTName, "g_Src0Texture" );

	Shader_BeginDrawEnd( m_pEffectArr[Shader_Deferred], "NoLightRendering", 0 );
}

#ifdef KEYBOARDCHECK
void CRenderer::KeyCheck_DebugRenderTarget( void )
{
#ifdef RENDERERDEBUGING
	if ( true == m_bIsDebugMode )
	{
		CDebug::GetInstance()->Add_DebugText( 1500.f, 30.f, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), "NUMPAD3 Click -> End Rendering Debug" );
		m_ActFunction();
	}
	else
	{
		CDebug::GetInstance()->Add_DebugText( 1500.f, 30.f, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), "NUMPAD3 Click -> Start Rendering Debug" );
	}

	if ( m_pInput_Device->Get_DownKeyState( DIK_NUMPAD3 ) )
	{
		m_bIsDebugMode = !m_bIsDebugMode;
		m_iSelect = 0;

		if ( m_bIsDebugMode )
			m_ActFunction = [&]() { Debug_ShowSelectOption(); };
		else
		{
			m_ActFunction = nullptr;
			m_ActNodeList.clear();
		}
	}
#endif
}
#endif

void CRenderer::ModifyCameraData( void )
{
	if ( nullptr != m_pCam_Manager->Get_CurrentCamera() )
	{
		m_CamData.fCamFar = m_pCam_Manager->Get_Far();
		m_CamData.matPrevView = m_CamData.matView;
		m_CamData.matView = m_pCam_Manager->Get_ViewMatrix();
		m_CamData.matProj = m_pCam_Manager->Get_ProjMatrix();
		m_CamData.matInvView = m_pCam_Manager->Get_InverseViewMatrix();
		m_CamData.matInvProj = m_pCam_Manager->Get_InverseProjMatrix();
		m_CamData.vPosition = *(_vec4*)&m_CamData.matInvView.m[3][0];
		m_CamData.vDirection = _vec4( m_pCam_Manager->Get_Direction(), 0.f );
	}
	else
	{
		D3DVIEWPORT9 Viewport;
		m_pGraphicDev->GetViewport( &Viewport );

		m_CamData.fCamFar = Viewport.MaxZ;
		m_CamData.matPrevView = m_CamData.matView;
		m_pGraphicDev->GetTransform( D3DTS_VIEW, &m_CamData.matView );
		m_pGraphicDev->GetTransform( D3DTS_PROJECTION, &m_CamData.matProj );
		D3DXMatrixInverse( &m_CamData.matInvView, nullptr, &m_CamData.matView );
		D3DXMatrixInverse( &m_CamData.matInvProj, nullptr, &m_CamData.matProj );
		m_CamData.vPosition = *(_vec4*)&m_CamData.matInvView.m[3][0];
		m_CamData.vDirection = _vec4( 1.f, 1.f, 1.f, 0.f );
	}
}

void CRenderer::ModifyShaderData( void )
{
	for ( _uint i = 0; i < Shader_End; ++i )
	{
		SetUp_ShaderData( m_pEffectArr[i], &m_ShaderVarData.pEffectVarData[i] );
#ifdef VIEWPROJMATRIXSETTING_ONCE
		SettingViewProjMatrix( m_pEffectArr[i] );
#endif
	}
}

void CRenderer::Render_VIBuffer( void )
{
	m_pGraphicDev->SetStreamSource( 0, m_pVB, 0, sizeof( VTXTEX ) );
	m_pGraphicDev->SetFVF( VTXTEX::FVF );
	m_pGraphicDev->SetIndices( m_pIB );
	m_pGraphicDev->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2 );
}

void CRenderer::SettingViewProjMatrix( LPD3DXEFFECT _pEffect )
{
	_pEffect->SetMatrix( "g_matView", &m_matOrthoView );
	_pEffect->SetMatrix( "g_matProj", &m_matOrthoProj );
}

void CRenderer::Shader_Begin( LPD3DXEFFECT _pEffect, const char * _pTechniqueName )
{
	_pEffect->SetTechnique( _pTechniqueName );
	_pEffect->Begin( nullptr, 0 );
}

void CRenderer::Shader_Begin( LPD3DXEFFECT _pEffect, Engine::CRenderer::EFFECTVARDATA* _pEffectVarData, const char * _pTechniqueName )
{
	if ( nullptr != _pEffectVarData )
		SetUp_ShaderData( _pEffect, _pEffectVarData );

	_pEffect->SetTechnique( _pTechniqueName );
	_pEffect->Begin( nullptr, 0 );
}

void CRenderer::Shader_Draw( LPD3DXEFFECT _pEffect, const _uint & _iPassNum )
{
	_pEffect->BeginPass( _iPassNum );

	Render_VIBuffer();

	_pEffect->EndPass();
}

void CRenderer::Shader_BeginDrawEnd( LPD3DXEFFECT _pEffect, const char * _pTechniqueName, const _uint & _iPassNum )
{
	_pEffect->SetTechnique( _pTechniqueName );
	_pEffect->Begin( nullptr, 0 );
	_pEffect->BeginPass( _iPassNum );

	Render_VIBuffer();

	_pEffect->EndPass();
	_pEffect->End();
}

void CRenderer::Shader_BeginDrawEnd( LPD3DXEFFECT _pEffect, Engine::CRenderer::EFFECTVARDATA* _pEffectVarData, const char * _pTechniqueName, const _uint & _iPassNum )
{
	if ( nullptr != _pEffectVarData )
		SetUp_ShaderData( _pEffect, _pEffectVarData );

	_pEffect->SetTechnique( _pTechniqueName );
	_pEffect->Begin( nullptr, 0 );
	_pEffect->BeginPass( _iPassNum );

	Render_VIBuffer();

	_pEffect->EndPass();
	_pEffect->End();
}

void CRenderer::SetUp_ShaderData( LPD3DXEFFECT _pEffect, Engine::CRenderer::EFFECTVARDATA* _pEffectVarData )
{
	for ( _uint i = 0; i < _pEffectVarData->iEffectVarCount; ++i )
	{
		switch ( _pEffectVarData->pEffectVar[i].eType )
		{
			case Engine::CRenderer::ShaderVar_Float:
				_pEffect->SetFloat( _pEffectVarData->pEffectVar[i].szName, _pEffectVarData->pEffectVar[i].mData.m[0][0] );
				break;
			case Engine::CRenderer::ShaderVar_Int:
				_pEffect->SetInt( _pEffectVarData->pEffectVar[i].szName, (_int)_pEffectVarData->pEffectVar[i].mData.m[0][0] );
				break;
			case Engine::CRenderer::ShaderVar_Bool:
				_pEffect->SetBool( _pEffectVarData->pEffectVar[i].szName, (_bool)(_pEffectVarData->pEffectVar[i].mData.m[0][0] == 0) );
				break;
			case Engine::CRenderer::ShaderVar_Vector:
				_pEffect->SetVector( _pEffectVarData->pEffectVar[i].szName, (_vec4*)&_pEffectVarData->pEffectVar[i].mData.m[0] );
				break;
			case Engine::CRenderer::ShaderVar_Matrix:
				_pEffect->SetMatrix( _pEffectVarData->pEffectVar[i].szName, &_pEffectVarData->pEffectVar[i].mData );
				break;
			case Engine::CRenderer::ShaderVar_Texture:
				_pEffect->SetTexture( _pEffectVarData->pEffectVar[i].szName, _pEffectVarData->pEffectVar[i].pTexture );
			default:
				continue;
		}
	}
}

void CRenderer::SettingAlphaBlendRenderObjectSortValue( list<REDNEROBJECT>& _RenderObjectList )
{
	if ( nullptr == m_pGraphicDev )
		return;

	const _vec3* pPos = nullptr;

	_vec3 vViewPos = _vec3( 0.f, 0.f, 0.f );

	for ( auto& RenderObject : _RenderObjectList )
	{
		CTransform* pTransform = RenderObject.pGameObject->Get_Transform();
		if ( nullptr == pTransform )
		{
			RenderObject.fSort = 0.f;
			continue;
		}

		pPos = pTransform->Get_Infomation( Engine::CTransform::INFO_POSITION );

		if ( nullptr == pPos )
		{
			RenderObject.fSort = 0.f;
			continue;
		}

		D3DXVec3TransformCoord( &vViewPos, pPos, &m_CamData.matView );
		RenderObject.fSort = vViewPos.z;
	}
}

_int CRenderer::Compute_CascadeIndex( _vec3 _vPos, const _float& _fRadius )
{
	D3DXVec3TransformCoord( &_vPos, &_vPos, &m_CamData.matView );

	_float fDist = D3DXVec3Length( &_vPos );

	for ( _uint i = 0; i < Cascade_End; ++i )
	{
		if ( fDist + _fRadius <= m_fCasCadeRange[i] )
		{
			return i;
		}
	}

	return -1;
}

void CRenderer::Render_Deferred( void )
{
	// --------------- Shadow Object Render.. --------------- //
	if ( true == Get_ShaderOptionOnOff( ShaderOption_Shadow ) )
	{
		Render_Shadow();
	}


	// --------- Non Alpha Object Render.. --------- //
	BEGINMRT( L"MRT_Deferred" );

	Render_NonAlpha();

	ENDMRT( L"MRT_Deferred" );


	// --------- Light Render.. --------- //
	if ( true == Get_ShaderOptionOnOff( ShaderOption_Light ) )
	{
		BEGINMRT( L"MRT_LightAcc" );

		Render_LightAcc();

		ENDMRT( L"MRT_LightAcc" );
	}
}

void CRenderer::Render_LightAcc( void )
{
	SETRT( m_pEffectArr[Shader_Light], L"RenderTarget_Normal", "g_NormalTexture" );
	SETRT( m_pEffectArr[Shader_Light], L"RenderTarget_Depth", "g_DepthTexture" );
	SETRT( m_pEffectArr[Shader_Light], L"RenderTarget_SSAO", "g_SSAOTexture" );
	SETRT( m_pEffectArr[Shader_Light], L"RenderTarget_SpecularIntensity", "g_SpecularIntensityTexture" );
	SETRT( m_pEffectArr[Shader_Light], L"RenderTarget_Color", "g_ColorTexture" );

	m_pEffectArr[Shader_Light]->SetVector( "g_vCamPosition", &m_CamData.vPosition );
	m_pEffectArr[Shader_Light]->SetVector( "g_vCamDir", &m_CamData.vDirection );
	m_pEffectArr[Shader_Light]->SetMatrix( "g_matInvView", &m_CamData.matInvView );
	m_pEffectArr[Shader_Light]->SetMatrix( "g_matInvProj", &m_CamData.matInvProj );
	m_pEffectArr[Shader_Light]->SetFloat( "g_fCamFar", m_CamData.fCamFar );

	m_pEffectArr[Shader_Light]->Begin( nullptr, 0 );

	_matrix matViewProj = m_CamData.matView * m_CamData.matProj;
	m_pLight_Manager->Render_Light( matViewProj, m_pFrustum, m_pEffectArr[Shader_Light] );

	m_pEffectArr[Shader_Light]->End();
}

void CRenderer::Render_Blend_Distortion(void)
{
	LPD3DXEFFECT pEffect = m_pShader_Distort->Get_EffectHandle();
	if (nullptr == pEffect)
		return;

	m_pTarget_Manager->SetUp_OnShader(pEffect, m_pLastRTName, "g_SceneTexture"); // By.KJY
	m_pTarget_Manager->SetUp_OnShader(pEffect, L"Target_Distort", "g_DistortTexture"); // By.KJY

	pEffect->SetMatrix("g_matView", &m_matOrthoView);
	pEffect->SetMatrix("g_matProj", &m_matOrthoProj);

	pEffect->Begin(nullptr, 0);
	pEffect->BeginPass(0);

	m_pGraphicDev->SetStreamSource(0, m_pVB, 0, sizeof(VTXTEX));
	m_pGraphicDev->SetFVF(VTXTEX::FVF);
	m_pGraphicDev->SetIndices(m_pIB);
	m_pGraphicDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

	pEffect->EndPass();
	pEffect->End();
}

void CRenderer::Clear_RenderGroup(void)
{
	for (auto& RenderList : m_RenderGroup)
	{
		for (auto& RenderObject : RenderList)
		{
			Engine::Safe_Release(RenderObject.pGameObject);
		}
		RenderList.clear();
	}

	for ( _uint i = 0; i < Cascade_End; ++i )
	{
		for ( auto& RenderObject : m_ShadowRenderGroup[i] )
		{
			Engine::Safe_Release(RenderObject);
		}
		m_ShadowRenderGroup[i].clear();
	}
}

CComponent * CRenderer::Clone(void)
{
	AddRef();

	return this;
}

CRenderer * CRenderer::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CRenderer *		pInstance = new CRenderer(pGraphicDev);

	if (FAILED(pInstance->Ready_Renderer()))
	{
		MSG_BOX("CRenderer Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}
#ifndef _FINAL
void CRenderer::Debug_Render()
{
	CDebug::GetInstance()->Add_DebugText("CRenderer");
}
#endif
_ulong CRenderer::Free(void)
{
	_ulong dwRefCnt = 0;

#ifdef KMH_MODIFY // By.KMH
	for ( size_t i = 0; i < Shader_End; ++i )
	{
		dwRefCnt = Engine::Safe_Release( m_pEffectArr[i] );
		dwRefCnt = Engine::Safe_Release( m_pShaderArr[i] );
	}

	for ( _uint i = 0; i < m_ShaderVarData.iEffectVarDataCount; ++i )
	{
		Engine::Safe_Delete_Array( m_ShaderVarData.pEffectVarData[i].pEffectVar );
	}
	Engine::Safe_Delete_Array( m_ShaderVarData.pEffectVarData );

	Engine::Safe_Delete( m_pGlareDef );

	Engine::Safe_Release( m_pRandomTexture );
	Engine::Safe_Release( m_pLUTTexture );
#ifdef ADD_GETTER_TARGETANDSHADER
	for ( size_t i = 0; i < m_iTargetCount; ++i )
		Engine::Safe_Delete_Array( m_pAllTagetNameArr[i] );
	Engine::Safe_Delete_Array( m_pAllTagetNameArr );
	Engine::Safe_Delete_Array( m_pAllEffectArr );
#endif
#else
	Safe_Release(m_pShader_Blend);
	Safe_Release(m_pShader_Light);
#endif
	Safe_Release(m_pShader_Distort); // By.KJY

	Clear_RenderGroup();

	Safe_Release(m_pVB);
	Safe_Release(m_pIB);

	Safe_Release(m_pTarget_Manager);

	dwRefCnt = CComponent::Free();

	return dwRefCnt;
}
