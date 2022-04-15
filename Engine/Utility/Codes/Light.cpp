#include "..\Headers\Light.h"
#include "Debug_Manager.h"
#include "Camera_Manager.h"
#include "Frustum.h"

USING( Engine )

CLight::CLight( LPDIRECT3DDEVICE9 pGraphicDev )
	: m_pGraphicDev( pGraphicDev )
#ifdef KMH_MODIFY
	, m_pCam_Manager( CCamera_Manager::GetInstance() )
#endif
{
	m_pGraphicDev->AddRef();
#ifndef _FINAL
	Tag(L"CLight", std::bind(&CLight::Debug_Render, this));
#endif

}

CLight::~CLight()
{
}

HRESULT CLight::Ready_Light( const D3DLIGHT9 * pLightInfo, const _vec3& _vLightPower )
{
	m_LightInfo = *pLightInfo;

	D3DVIEWPORT9		ViewPort;
	m_pGraphicDev->GetViewport( &ViewPort );

	m_vLightPower = _vec4( _vLightPower, 1.f );

	if ( nullptr == m_pVB )
	{
		if ( FAILED( m_pGraphicDev->CreateVertexBuffer( sizeof( VTXTEX ) * 4, 0, VTXTEX::FVF, D3DPOOL_MANAGED, &m_pVB, nullptr ) ) )
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
	}
	if ( nullptr == m_pIB )
	{
		if ( FAILED( m_pGraphicDev->CreateIndexBuffer( sizeof( INDEX16 ) * 2, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, nullptr ) ) )
			return E_FAIL;

		INDEX16*			pIndex = nullptr;

		m_pIB->Lock( 0, 0, (void**)&pIndex, 0 );

		pIndex[0]._0 = 0;
		pIndex[0]._1 = 1;
		pIndex[0]._2 = 2;

		pIndex[1]._0 = 0;
		pIndex[1]._1 = 2;
		pIndex[1]._2 = 3;

		m_pIB->Unlock();
	}

	if ( FAILED( Ready_Matrix( D3DXToRadian( 90.f ), 1.f, 0.3f, 1000.f ) ) )
		return E_FAIL;

#ifdef LIGHT_DEBUGING
	Engine::Safe_Release( m_pVB_Box );
	Engine::Safe_Release( m_pIB_Box );
	Engine::Safe_Release( m_pSphere );
	Engine::Safe_Release( m_pSphere_Range );
	Engine::Safe_Release( m_pLine );

	if ( FAILED( D3DXCreateLine( m_pGraphicDev, &m_pLine ) ) )
		return E_FAIL;

	if ( FAILED( D3DXCreateSphere( m_pGraphicDev, (m_LightInfo.Range < 0.f) ? 1.f : m_LightInfo.Range, 30, 30, &m_pSphere_Range, nullptr ) ) )
		return E_FAIL;

	if ( FAILED( m_pGraphicDev->CreateVertexBuffer( sizeof( VTXCOL ) * 8, 0, D3DFVF_XYZ | D3DFVF_DIFFUSE, D3DPOOL_MANAGED, &m_pVB_Box, nullptr ) ) )
		return E_FAIL;

	if ( FAILED( m_pGraphicDev->CreateIndexBuffer( sizeof( INDEX16 ) * 12, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB_Box, nullptr ) ) )
		return E_FAIL;

	m_pLine->SetWidth( 3.f );


	VTXCOL*			pVertex = nullptr;


	// Lock : 다른스레드가 접근할 수 없도록 막는다. (값을 갱신하는 중이니까)
	//      : 잠겨진 공간의 주소를 얻어온다.(값을 갱신하려고)
	m_pVB_Box->Lock(0, 0, (void**)&pVertex, 0);

	// 앞면
	pVertex[0].vPosition = _vec3(-0.5f, 0.5f, -0.5f);
	pVertex[0].dwColor = D3DXCOLOR( 0.f, 1.f, 0.f, 1.f );

	pVertex[1].vPosition = _vec3(0.5f, 0.5f, -0.5f);
	pVertex[1].dwColor = D3DXCOLOR( 0.f, 1.f, 0.f, 1.f );

	pVertex[2].vPosition = _vec3(0.5f, -0.5f, -0.5f);
	pVertex[2].dwColor = D3DXCOLOR( 0.f, 1.f, 0.f, 1.f );

	pVertex[3].vPosition = _vec3(-0.5f, -0.5f, -0.5f);
	pVertex[3].dwColor = D3DXCOLOR( 0.f, 1.f, 0.f, 1.f );

	// 뒷면
	pVertex[4].vPosition = _vec3(-0.5f, 0.5f, 0.5f);
	pVertex[4].dwColor = D3DXCOLOR( 0.f, 1.f, 0.f, 1.f );

	pVertex[5].vPosition = _vec3(0.5f, 0.5f, 0.5f);
	pVertex[5].dwColor = D3DXCOLOR( 0.f, 1.f, 0.f, 1.f );

	pVertex[6].vPosition = _vec3(0.5f, -0.5f, 0.5f);
	pVertex[6].dwColor = D3DXCOLOR( 0.f, 1.f, 0.f, 1.f );

	pVertex[7].vPosition = _vec3(-0.5f, -0.5f, 0.5f);
	pVertex[7].dwColor = D3DXCOLOR( 0.f, 1.f, 0.f, 1.f );

	m_pVB_Box->Unlock();

	INDEX16*			pIndex = nullptr;

	m_pIB_Box->Lock(0, 0, (void**)&pIndex, 0);

	// +x 
	pIndex[0]._0 = 1; pIndex[0]._1 = 5; pIndex[0]._2 = 6;
	pIndex[1]._0 = 1; pIndex[1]._1 = 6; pIndex[1]._2 = 2;

	// -x 
	pIndex[2]._0 = 4; pIndex[2]._1 = 0; pIndex[2]._2 = 3;
	pIndex[3]._0 = 4; pIndex[3]._1 = 3; pIndex[3]._2 = 7;

	// +y
	pIndex[4]._0 = 4; pIndex[4]._1 = 5; pIndex[4]._2 = 1;
	pIndex[5]._0 = 4; pIndex[5]._1 = 1; pIndex[5]._2 = 0;

	// -y 
	pIndex[6]._0 = 3; pIndex[6]._1 = 2; pIndex[6]._2 = 6;
	pIndex[7]._0 = 3; pIndex[7]._1 = 6; pIndex[7]._2 = 7;

	// +z
	pIndex[8]._0 = 7; pIndex[8]._1 = 6; pIndex[8]._2 = 5;
	pIndex[9]._0 = 7; pIndex[9]._1 = 5; pIndex[9]._2 = 4;

	// -z 
	pIndex[10]._0 = 0; pIndex[10]._1 = 1; pIndex[10]._2 = 2;
	pIndex[11]._0 = 0; pIndex[11]._1 = 2; pIndex[11]._2 = 3;	

	m_pIB_Box->Unlock();


	_vec3 vPos = m_LightInfo.Position;
	_vec3 vDir = m_LightInfo.Direction;
	_float fScale = 0.75f;

	if ( FAILED( D3DXCreateSphere( m_pGraphicDev, fScale, 30, 30, &m_pSphere, nullptr ) ) )
		return E_FAIL;

	_matrix matTrans, matScale, matWorld;

	D3DXMatrixTranslation( &matTrans, vPos.x, vPos.y, vPos.z );
	D3DXMatrixScaling( &matScale, fScale, fScale, fScale );

	switch ( m_LightInfo.Type )
	{
		case D3DLIGHT_DIRECTIONAL:
		{
			//m_vDir[0] = vPos + _vec3( vDir.x * fScale * 0.5f, 0.f, 0.f ) + _vec3( 0.f, vDir.y * fScale * 0.5f, 0.f );
			//m_vDir[1] = m_vDir[0] + vDir * (fScale * 5.f);
			//
			//m_vDir[2] = vPos - _vec3( vDir.x * fScale * 0.5f, 0.f, 0.f ) + _vec3( 0.f, vDir.y * fScale * 0.5f, 0.f );
			//m_vDir[3] = m_vDir[2] + vDir * (fScale * 5.f);
			//
			//m_vDir[4] = vPos - _vec3( vDir.x * fScale * 0.5f, 0.f, 0.f ) - _vec3( 0.f, vDir.y * fScale * 0.5f, 0.f );
			//m_vDir[5] = m_vDir[4] + vDir * (fScale * 5.f);
			//
			//m_vDir[6] = vPos + _vec3( vDir.x * fScale * 0.5f, 0.f, 0.f ) - _vec3( 0.f, vDir.y * fScale * 0.5f, 0.f );
			//m_vDir[7] = m_vDir[6] + vDir * (fScale * 5.f);

			m_vDir[0] = vPos;
			m_vDir[1] = m_vDir[0] + vDir * (fScale * 5.f);

			m_vDir[2] = vPos;
			m_vDir[3] = m_vDir[2] + vDir * (fScale * 5.f);

			m_vDir[4] = vPos;
			m_vDir[5] = m_vDir[4] + vDir * (fScale * 5.f);

			m_vDir[6] = vPos;
			m_vDir[7] = m_vDir[6] + vDir * (fScale * 5.f);
		}
		break;
		case D3DLIGHT_SPOT:
		{
			//_float fCos = cosf( m_LightInfo.Theta );
			//_float fSin = sinf( m_LightInfo.Theta );
			//_float fRange = m_LightInfo.Range / sqrtf( 2 );
			//
			//m_vDir[0] = vPos;
			//m_vDir[1] = m_vDir[0] + vDir * (fScale * 5.f);
			//
			//m_vDir[2] = vPos;
			//m_vDir[3] = m_vDir[2] + vDir * (fScale * 5.f);
			//
			//m_vDir[4] = vPos;
			//m_vDir[5] = m_vDir[4] + vDir * (fScale * 5.f);
			//
			//m_vDir[6] = vPos;
			//m_vDir[7] = m_vDir[6] + vDir * (fScale * 5.f);

			m_vDir[0] = vPos;
			m_vDir[1] = m_vDir[0] + vDir * m_LightInfo.Range;

			m_vDir[2] = vPos;
			m_vDir[3] = m_vDir[2] + vDir * m_LightInfo.Range;

			m_vDir[4] = vPos;
			m_vDir[5] = m_vDir[4] + vDir * m_LightInfo.Range;

			m_vDir[6] = vPos;
			m_vDir[7] = m_vDir[6] + vDir * m_LightInfo.Range;
		}
		break;
	}

	matWorld = matScale * matTrans;
	m_matWorld = matWorld;
	m_matTrans = matTrans;
	D3DXMatrixInverse( &m_matInvWorld, nullptr, &m_matWorld );
#endif

	return NOERROR;
}

void CLight::Render_Light( LPD3DXEFFECT pEffect )
{
	_uint		iPassIdx = 0;

	if ( D3DLIGHT_DIRECTIONAL == m_LightInfo.Type )
	{
		iPassIdx = 0;
		pEffect->SetVector( "g_vLightDir", &_vec4( m_LightInfo.Direction, 0.f ) );
	}
	else if ( D3DLIGHT_POINT == m_LightInfo.Type )
	{
		iPassIdx = 1;
		pEffect->SetVector( "g_vLightPos", &_vec4( m_LightInfo.Position, 1.f ) );
		pEffect->SetFloat( "g_fRange", m_LightInfo.Range );
	}
	else if ( D3DLIGHT_SPOT == m_LightInfo.Type )
	{
		iPassIdx = 2;
		pEffect->SetVector( "g_vLightPos", &_vec4( m_LightInfo.Position, 1.f ) );
		pEffect->SetVector( "g_vLightDir", &_vec4( m_LightInfo.Direction, 0.f ) );
		pEffect->SetFloat( "g_fRange", m_LightInfo.Range );
		pEffect->SetFloat( "g_fPhi", m_LightInfo.Phi );
		pEffect->SetFloat( "g_fTheta", m_LightInfo.Theta );
	}

	pEffect->SetVector( "g_vLightPower", &m_vLightPower );

	pEffect->SetVector( "g_vLightDiffuse", (_vec4*)(&m_LightInfo.Diffuse) );
	pEffect->SetVector( "g_vLightAmbient", (_vec4*)(&m_LightInfo.Ambient) );
	pEffect->SetVector( "g_vLightSpecular", (_vec4*)(&m_LightInfo.Specular) );
	pEffect->SetMatrix( "g_matView", &m_matView );
	pEffect->SetMatrix( "g_matProj", &m_matOrthoProj );


	pEffect->CommitChanges();

	pEffect->BeginPass( iPassIdx );

	m_pGraphicDev->SetStreamSource( 0, m_pVB, 0, sizeof( VTXTEX ) );
	m_pGraphicDev->SetFVF( VTXTEX::FVF );
	m_pGraphicDev->SetIndices( m_pIB );
	m_pGraphicDev->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2 );

	pEffect->EndPass();
}

#ifdef LIGHT_DEBUGING
void CLight::Render_DebugLight( void )
{
	m_pGraphicDev->SetTransform( D3DTS_WORLD, &m_matWorld );

	_ulong dwLightOption = 0;
	m_pGraphicDev->GetRenderState( D3DRS_LIGHTING, &dwLightOption );

	m_pGraphicDev->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
	m_pGraphicDev->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	m_pGraphicDev->SetRenderState( D3DRS_LIGHTING, FALSE );

	m_pGraphicDev->SetStreamSource( 0, m_pVB_Box, 0, sizeof( VTXCOL ) );
	m_pGraphicDev->SetFVF( D3DFVF_XYZ | D3DFVF_DIFFUSE );
	m_pGraphicDev->SetIndices( m_pIB_Box );
	m_pGraphicDev->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12 );

	m_pGraphicDev->SetTransform( D3DTS_WORLD, &m_matTrans );

	//m_pSphere->DrawSubset( 0 );

	_matrix matIdentity;
	D3DXMatrixIdentity( &matIdentity );

	_vec3 vDir[8];
	memcpy( vDir, m_vDir, sizeof( _vec3 ) * 8 );

	_matrix matView, matProj;
	m_pGraphicDev->GetTransform( D3DTS_VIEW, &matView );
	m_pGraphicDev->GetTransform( D3DTS_PROJECTION, &matProj );

	for ( _uint i = 0; i < 8; ++i )
	{
		D3DXVec3TransformCoord( vDir + i, vDir + i, &matView );

		if ( vDir[i].z < 0.f )
			vDir[i].z = 0.f;

		D3DXVec3TransformCoord( vDir + i, vDir + i, &matProj );
	}

	if ( true == m_bIsDetailDraw )
	{
		switch ( m_LightInfo.Type )
		{
			case D3DLIGHT_DIRECTIONAL:
			{
				m_pLine->DrawTransform( vDir, 2, &matIdentity, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ) );
				m_pLine->DrawTransform( vDir + 2, 2, &matIdentity, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ) );
				m_pLine->DrawTransform( vDir + 4, 2, &matIdentity, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ) );
				m_pLine->DrawTransform( vDir + 6, 2, &matIdentity, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ) );
			}
			break;
			case D3DLIGHT_POINT:
			{
				//_ulong dwCullModeOption = 0;
				//m_pGraphicDev->GetRenderState( D3DRS_CULLMODE, &dwCullModeOption );
				//m_pGraphicDev->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
				//
				//m_pSphere_Range->DrawSubset( 0 );
				//
				//m_pGraphicDev->SetRenderState( D3DRS_CULLMODE, dwCullModeOption );
			}
			break;
			case D3DLIGHT_SPOT:
			{
				m_pLine->DrawTransform( vDir, 2, &matIdentity, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ) );
				m_pLine->DrawTransform( vDir + 2, 2, &matIdentity, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ) );
				m_pLine->DrawTransform( vDir + 4, 2, &matIdentity, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ) );
				m_pLine->DrawTransform( vDir + 6, 2, &matIdentity, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ) );
			}
			break;
		}
	}

	m_pGraphicDev->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
	m_pGraphicDev->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	m_pGraphicDev->SetRenderState( D3DRS_LIGHTING, dwLightOption );
}

_bool CLight::CheckPicking( D3DLIGHT9* _pOut, const _vec3 & _vRayPos, const _vec3 & _vRayDir )
{
	_vec3 vRayPos_Local, vRayDir_Local;

	D3DXVec3TransformCoord( &vRayPos_Local, &_vRayPos, &m_matInvWorld );
	D3DXVec3TransformNormal( &vRayDir_Local, &_vRayDir, &m_matInvWorld );

	BOOL	bHit = false;
	DWORD	dwFaceIndex = 0;
	_float	fDist = 0.f;
	_float	fU = 0.f, fV = 0.f;
	LPD3DXBUFFER pBuffer = nullptr;
	DWORD	dwCountOfHits = 0;

	if ( FAILED( D3DXIntersect( m_pSphere, &vRayPos_Local, &vRayDir_Local, &bHit, &dwFaceIndex, &fU, &fV, &fDist, &pBuffer, &dwCountOfHits ) ) )
		return false;

	if ( bHit == TRUE )
	{
		if ( _pOut == nullptr )
			return false;

		*_pOut = m_LightInfo;
		(*_pOut).Attenuation0 = m_vLightPower.x;
		(*_pOut).Attenuation1 = m_vLightPower.y;
		(*_pOut).Attenuation2 = m_vLightPower.z;
		(*_pOut).Falloff = m_vLightPower.w;

		return true;
	}

	return false;
}
_bool CLight::ActOcculsionTest( Engine::CFrustum* _pFrustum, const _matrix & _matViewProj )
{
	// 방향성 광원인 경우 true 를 무조건 리턴( 태양 같은 조명이기 때문에 컬링을 어떻게 할까??? )..
	if ( D3DLIGHT_DIRECTIONAL == m_LightInfo.Type )
		return true;

	// 먼저 카메라 프러스텀 안에 있는지 체크..
	if ( false == _pFrustum->isIn_Frustum( (_vec3*)&m_LightInfo.Position, 0.f ) )
		return false;

	return true;
}
#endif

HRESULT CLight::Ready_Matrix( const _float& _fFovY, const _float& _fAspect, const _float& _fNear, const _float& _fFar )
{
	D3DVIEWPORT9	Viewport;

	m_pGraphicDev->GetViewport( &Viewport );

	D3DXMatrixIdentity( &m_matView );

	m_matView._11 = (_float)Viewport.Width;
	m_matView._22 = (_float)Viewport.Height;
	m_matView._33 = 1.f;
	m_matView._41 = -0.5f - Viewport.Width * 0.5f + m_matView._11 * 0.5f;
	m_matView._42 = -(-0.5f) + Viewport.Height * 0.5f - m_matView._22 * 0.5f;

	D3DXMatrixOrthoLH( &m_matOrthoProj, (_float)Viewport.Width, (_float)Viewport.Height, 0.f, 1.f );

	return NOERROR;
}

CLight * CLight::Create( LPDIRECT3DDEVICE9 pGraphicDev, const D3DLIGHT9 * pLightInfo, const _vec3& _vLightPower )
{
	CLight *		pInstance = new CLight( pGraphicDev );

	if ( FAILED( pInstance->Ready_Light( pLightInfo, _vLightPower ) ) )
	{
		MSG_BOX( "CLight Create Failed" );
		Safe_Release( pInstance );
	}

	return pInstance;
}
#ifndef _FINAL
void CLight::Debug_Render()
{
	CDebug::GetInstance()->Add_DebugText("CLight");

}
#endif
_ulong CLight::Free( void )
{
	Safe_Release( m_pVB );
	Safe_Release( m_pIB );

#ifdef LIGHT_DEBUGING
	Safe_Release( m_pVB_Box );
	Safe_Release( m_pIB_Box );

	Engine::Safe_Release( m_pSphere );
	Engine::Safe_Release( m_pSphere_Range );
	Engine::Safe_Release( m_pLine );
#endif

	Safe_Release( m_pGraphicDev );

	return _ulong();
}
