#include "..\Headers\Collider.h"
#include "GameObject.h"

USING( Engine )

_bool CCollider::m_bIsShowCollider = false;
_bool CCollider::m_bIsKeyDown = false;

CCollider::CCollider(LPDIRECT3DDEVICE9 pGraphicDev)
	: CVIBuffer(pGraphicDev)
{
#ifndef _FINAL
	Tag(L"CCollider", std::bind(&CCollider::Debug_Render, this));
#endif

}

CCollider::CCollider(const CCollider & rhs)
	: CVIBuffer(rhs)
	, m_InfoData( rhs.m_InfoData )
	, m_ResizeData( rhs.m_ResizeData )
	, m_DebugData( rhs.m_DebugData )
	, m_ColliderData( rhs.m_ColliderData )
{
	m_OptionList = rhs.m_OptionList;
	memcpy( m_StaticOptionArr, rhs.m_StaticOptionArr, sizeof( OPTIONDATA ) * sizeof( StaticOption_End ) );

	for (size_t i = 0; i < COLLIDER_END; i++)
	{
		m_DebugData.pTexture[i] = rhs.m_DebugData.pTexture[i];
		m_DebugData.pTexture[i]->AddRef();
	}

	m_DebugData.pSphereMesh = rhs.m_DebugData.pSphereMesh;
	m_DebugData.pSphereMesh->AddRef();

#ifndef _FINAL
	Tag(L"CCollider", std::bind(&CCollider::Debug_Render, this));
#endif

}

void CCollider::Set_IsActive( const _bool & _bIsActive )
{
	if ( true == _bIsActive && m_InfoData.eState == COLLIDER_NOACTIVE )
		m_InfoData.eState = COLLIDER_NOCOL;
	else if ( false == _bIsActive )
		m_InfoData.eState = COLLIDER_NOACTIVE;
}

Engine::CGameObject * CCollider::Get_GameObject( void )
{
	if ( nullptr == m_InfoData.pGameObject )
		return nullptr;

	return m_InfoData.pGameObject;
}

_vec3 CCollider::Get_Min( void )
{
	return m_ResizeData.vOriginalMin;
}

_vec3 CCollider::Get_Min_WorldNotRot( void )
{
	return m_ColliderData.vMinWorld_NotRot;
}

_vec3 CCollider::Get_Max( void )
{
	return m_ResizeData.vOriginalMax;
}

_vec3 CCollider::Get_Max_WorldNotRot( void )
{
	return m_ColliderData.vMaxWorld_NotRot;
}

_matrix CCollider::Get_WorldMatrix( void )
{
	return m_InfoData.matWorld;
}

_matrix CCollider::Get_WorldMatrix_NotRot( void )
{
	return m_InfoData.matWorld_NotRot;
}

_bool CCollider::Get_ColliderData( COLLIDERDATA* _pOut )
{
	*_pOut = m_ColliderData;

	return true;
}

CCollider::COLLIDER_STATE CCollider::Get_State( void )
{
	return m_InfoData.eState;
}

_bool CCollider::Get_OptionList( list<OPTIONDATA>& _OptionList )
{
	_OptionList = m_OptionList;
	return true;
}

_bool CCollider::Get_StaticOption( STATICOPTIONDATA& _Option, const STATICOPTIONKIND & _eOption )
{
	if ( _eOption >= StaticOption_End )
		return false;

	_Option = m_StaticOptionArr[_eOption];
	return true;
}

CTransform * CCollider::Get_ParentTransform( void )
{
	return m_ResizeData.pTransformCom;
}

CTransform * CCollider::Get_ObjectTransform( void )
{
	return m_InfoData.pObjectTransform;
}

_bool CCollider::Get_Collision( void )
{
	return m_InfoData.bCollision;
}

const _tchar * CCollider::Get_GroupName( void )
{
	return m_InfoData.pGroupTag;
}

void CCollider::Add_Option( const OPTIONDATA& _OptionData )
{
	m_OptionList.push_back( _OptionData );
}

void CCollider::Modify_StaticOption( const STATICOPTIONDATA& _OptionData, const STATICOPTIONKIND& _eOption )
{
	if ( _eOption >= Option_End )
		return;

	m_StaticOptionArr[_eOption] = _OptionData;
}

HRESULT CCollider::Ready_Collider(void)
{
	if ( FAILED( Ready_Buffer() ) )
		return E_FAIL;
	if ( FAILED( Ready_DebugResource() ) )
		return E_FAIL;
	if ( FAILED( Ready_OriginalData() ) )
		return E_FAIL;

	m_InfoData.eState = COLLIDER_NOCOL;

	return NOERROR;
}

void CCollider::Update_Component( const _float & fTimeDelta )
{
	if ( CInput_Device::GetInstance()->Get_DownKeyState( DIK_NUMPAD9 ) && false == m_bIsKeyDown )
	{
		m_bIsShowCollider = !m_bIsShowCollider;
		m_bIsKeyDown = true;
	}

	m_OptionList.clear();
}

void CCollider::LastUpdate_Component( const _float & fTimeDelta )
{
	m_bIsKeyDown = false;

	Ready_OriginalData();
	Update_ColliderData();
	Update_InfoData();
}

void CCollider::Render_Buffer(void)
{
	if ( m_bIsShowCollider == false )
		return;
#ifndef _FINAL
	if (nullptr == m_pVB)
		return;

	_matrix matTempWorld; m_pGraphicDev->GetTransform( D3DTS_WORLD, &matTempWorld );
	_ulong dwTempFillMode; m_pGraphicDev->GetRenderState( D3DRS_FILLMODE, &dwTempFillMode );
	_ulong dwTempLightMode; m_pGraphicDev->GetRenderState( D3DRS_LIGHTING, &dwTempLightMode );

	m_pGraphicDev->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
	m_pGraphicDev->SetRenderState( D3DRS_LIGHTING, FALSE );

	if ( COLLIDER_NOCOL != m_InfoData.eState )
		m_pGraphicDev->SetTexture(0, m_DebugData.pTexture[COLLIDER_COL]);
	else
		m_pGraphicDev->SetTexture(0, m_DebugData.pTexture[COLLIDER_NOCOL]);

	if ( m_InfoData.eType == TYPE_SPHERE )
	{
		m_pGraphicDev->SetTransform( D3DTS_WORLD, &m_DebugData.matSphereWorld );
		m_DebugData.pSphereMesh->DrawSubset( 0 );
	}

	if ( TYPE_AABB == m_InfoData.eType )
		m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_InfoData.matWorld_NotRot);
	else
		m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_InfoData.matWorld);

	CVIBuffer::Render_Buffer();

	m_pGraphicDev->SetRenderState( D3DRS_LIGHTING, dwTempLightMode );
	m_pGraphicDev->SetRenderState( D3DRS_FILLMODE, dwTempFillMode );
	m_pGraphicDev->SetTransform( D3DTS_WORLD, &matTempWorld );

#endif
}

_bool CCollider::Collision_AABB( CCollider * pTargetCollider )
{
	// 충돌 검사를 아예 하지 않는 상태라면 false 리턴..
	if ( m_InfoData.eState == COLLIDER_NOACTIVE ||
		 pTargetCollider->Get_State() == COLLIDER_NOACTIVE )
		return false;

	// 내 Collider 의 Min Max 정보를 가져옴..
	_vec3		vSourMin = m_ColliderData.vMinWorld_NotRot;
	_vec3		vSourMax = m_ColliderData.vMaxWorld_NotRot;

	// 충돌 검사를 할 타겟의 Min, Max 정보를 가져옴..
	_vec3		vTargetMin = pTargetCollider->Get_Min_WorldNotRot();
	_vec3		vTargetMax = pTargetCollider->Get_Max_WorldNotRot();

	// x 축 선상에서 바라봤을때 
	if (max(vSourMin.x, vTargetMin.x) > min(vSourMax.x, vTargetMax.x))
		goto notcoll; 

	// y 축 선상에서 바라봤을때 
	if (max(vSourMin.y, vTargetMin.y) > min(vSourMax.y, vTargetMax.y))
		goto notcoll;

	// z 축 선상에서 바라봤을때 
	if (max(vSourMin.z, vTargetMin.z) > min(vSourMax.z, vTargetMax.z))
		goto notcoll;

	// 으아아아아아아..
	//m_isColl = true;
	//pTargetCollider->Set_Coll(true);

	return true;

notcoll: 
	//m_isColl = false;
	//pTargetCollider->Set_Coll(false);
	return false;
}

_bool CCollider::Collision_OBB( CCollider * pTargetCollider )
{
	// 충돌 검사를 아예 하지 않는 상태라면 false 리턴..
	if ( m_InfoData.eState == COLLIDER_NOACTIVE ||
		 pTargetCollider->Get_State() == COLLIDER_NOACTIVE )
		return false;

	COLLIDERDATA		tOBB[2];

	// 나와 타겟의 ColliderData 를 가져온다..
	tOBB[0] = m_ColliderData;
	if ( false == pTargetCollider->Get_ColliderData( &tOBB[1] ) )
		return false;

	_float		fDistance[3];

	for (size_t i = 0; i < 2; i++)
	{
		for (size_t j = 0; j < 3; j++)
		{
			fDistance[0] = fabs(D3DXVec3Dot(&tOBB[i].vAlign_Axis[j], &tOBB[0].vProj_Axis[0]))
				+ fabs(D3DXVec3Dot(&tOBB[i].vAlign_Axis[j], &tOBB[0].vProj_Axis[1]))
				+ fabs(D3DXVec3Dot(&tOBB[i].vAlign_Axis[j], &tOBB[0].vProj_Axis[2]));

			fDistance[1] = fabs(D3DXVec3Dot(&tOBB[i].vAlign_Axis[j], &tOBB[1].vProj_Axis[0]))
				+ fabs(D3DXVec3Dot(&tOBB[i].vAlign_Axis[j], &tOBB[1].vProj_Axis[1]))
				+ fabs(D3DXVec3Dot(&tOBB[i].vAlign_Axis[j], &tOBB[1].vProj_Axis[2]));

			_vec3		vCenter = tOBB[1].vCenter - tOBB[0].vCenter;
			fDistance[2] = fabs(D3DXVec3Dot(&tOBB[i].vAlign_Axis[j], &vCenter));

			if (fDistance[0] + fDistance[1] < fDistance[2])
			{
				// 으아아아아아아아..
				//m_isColl = false;
				//pTargetCollider->Set_Coll(false);
				return false;
			}
		}
	}

	//m_isColl = true;
	//pTargetCollider->Set_Coll(true);

	return true;	
}

_bool CCollider::Collision_Sphere( CCollider* pTargetCollider, COLLISIONDATA& _CollisionData )
{
	// 충돌 검사를 아예 하지 않는 상태라면 false 리턴..
	if ( m_InfoData.eState == COLLIDER_NOACTIVE ||
		 pTargetCollider->Get_State() == COLLIDER_NOACTIVE )
		return false;

	COLLIDERDATA ColliderData[2];

	ColliderData[0] = m_ColliderData;
	pTargetCollider->Get_ColliderData( &ColliderData[1] );

	_vec3 vDir = (ColliderData[0].vCenter - ColliderData[1].vCenter);
	_float fDistance = D3DXVec3Length( &vDir );
	_float fRadius = ColliderData[0].fRadius + ColliderData[1].fRadius;
	if ( fDistance > fRadius )
		return false;

	_float fTempDistance = fDistance;
	D3DXVec3Normalize( &vDir, &vDir );
	fDistance = fRadius - fDistance;
	_vec3 vDstPos = ColliderData[0].vCenter + -vDir * fDistance;
	_vec3 vSrcPos = ColliderData[1].vCenter + +vDir * fDistance;

	_CollisionData.vDist = _vec3( fTempDistance, fTempDistance, fTempDistance );
	_CollisionData.vRadius = _vec3( fRadius, fRadius, fRadius );
	_CollisionData.vCollisionPos = ( vDstPos + vSrcPos ) * 0.5f;
	_CollisionData.vOverlapSize = _vec3( fDistance, fDistance, fDistance );

	return true;
}

void CCollider::ReSizing(TYPE eType, const _vec3 * pMin, const _vec3 * pMax, const CTransform* pTransformCom)
{
	RESIZEDATA ResizeData;
	ResizeData.vOriginalMin = *pMin;
	ResizeData.vOriginalMax = *pMax;
	ResizeData.pTransformCom = const_cast<CTransform*>(pTransformCom);
	ResizeData.pParent = nullptr;

	ReSizing( eType, ResizeData );
}

void CCollider::ReSizing( const TYPE& eType, const RESIZEDATA& ResizeData )
{
	m_InfoData.eType = eType;

	m_ResizeData = ResizeData;

	LastUpdate_Component( 0.f );
}

HRESULT CCollider::Ready_Buffer( void )
{
	m_iVertexSize = sizeof(VTXTEX);
	m_iVertexCnt = 8;
	m_dwVertexFVF = D3DFVF_XYZ | D3DFVF_TEX1;

	m_iIndexSize = sizeof(INDEX16);
	m_IndexFmt = D3DFMT_INDEX16;
	m_iTriCnt = 12;

	if (FAILED(CVIBuffer::Ready_Buffer()))
		return E_FAIL;

	if ( nullptr == m_pVB ||
		 nullptr == m_pIB )
		return E_FAIL;

	VTXTEX*			pVertex = nullptr;

	// Lock : 다른스레드가 접근할 수 없도록 막는다. (값을 갱신하는 중이니까)
	//      : 잠겨진 공간의 주소를 얻어온다.(값을 갱신하려고)
	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	// 앞면
	pVertex[0].vPosition = _vec3(-0.5f, 0.5f, -0.5f);	 
	pVertex[0].vTexUV = _vec2(0.f, 0.0f);

	pVertex[1].vPosition = _vec3(0.5f, 0.5f, -0.5f);
	pVertex[1].vTexUV = _vec2(0.f, 0.0f);

	pVertex[2].vPosition = _vec3(0.5f, -0.5f, -0.5f);
	pVertex[2].vTexUV = _vec2(0.f, 0.0f);

	pVertex[3].vPosition = _vec3(-0.5f, -0.5f, -0.5f);
	pVertex[3].vTexUV = _vec2(0.f, 0.0f);

	// 뒷면
	pVertex[4].vPosition = _vec3(-0.5f, 0.5f, 0.5f);
	pVertex[4].vTexUV = _vec2(0.f, 0.0f);

	pVertex[5].vPosition = _vec3(0.5f, 0.5f, 0.5f);
	pVertex[5].vTexUV = _vec2(0.f, 0.0f);

	pVertex[6].vPosition = _vec3(0.5f, -0.5f, 0.5f);
	pVertex[6].vTexUV = _vec2(0.f, 0.0f);

	pVertex[7].vPosition = _vec3(-0.5f, -0.5f, 0.5f);
	pVertex[7].vTexUV = _vec2(0.f, 0.0f);

	for ( _uint i = 0; i < 8; ++i )
		m_ColliderData.vOriginalPoint[i] = pVertex[i].vPosition;

	m_pVB->Unlock();

	INDEX16*			pIndex = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

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

	m_pIB->Unlock();

	return NOERROR;
}

HRESULT CCollider::Ready_DebugResource( void )
{
	// 텍스쳐를 생성하자.
	for (size_t i = 0; i < COLLIDER_END; ++i)
	{
		if (FAILED(D3DXCreateTexture(m_pGraphicDev, 100, 100, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_DebugData.pTexture[i])))
			return E_FAIL;
	}

	D3DLOCKED_RECT			Lock_Rect;

	m_DebugData.pTexture[COLLIDER_COL]->LockRect(0, &Lock_Rect, nullptr, 0);
	for (size_t i = 0; i < 100; i++)	
	{
		_byte*	pPixel = (_byte*)Lock_Rect.pBits + Lock_Rect.Pitch * i;

		for (size_t j = 0; j < 100; j++)
		{
			((_ulong*)pPixel)[j] = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);
		}
	}
	m_DebugData.pTexture[COLLIDER_COL]->UnlockRect(0);

	m_DebugData.pTexture[COLLIDER_NOCOL]->LockRect(0, &Lock_Rect, nullptr, 0);
	for (size_t i = 0; i < 100; i++)
	{
		_byte*	pPixel = (_byte*)Lock_Rect.pBits + Lock_Rect.Pitch * i;

		for (size_t j = 0; j < 100; j++)
		{
			((_ulong*)pPixel)[j] = D3DXCOLOR(0.f, 1.f, 0.f, 1.f);
		}
	}
	m_DebugData.pTexture[COLLIDER_NOCOL]->UnlockRect(0);	



	// 원 충돌 시 디버그용으로 띄울 원..
	// 으아아아아아아..
	if ( FAILED( D3DXCreateSphere( m_pGraphicDev, 1.f, 30, 30, &m_DebugData.pSphereMesh, nullptr ) ) )
		return E_FAIL;

	return NOERROR;
}

HRESULT CCollider::Ready_OriginalData( void )
{
	if ( D3DXVec3Length( &(m_ResizeData.vOriginalMin + m_ResizeData.vOriginalMax) ) )
	{
		m_ResizeData.vOriginalMin = _vec3( -m_ResizeData.fOriginalRadius, -m_ResizeData.fOriginalRadius, -m_ResizeData.fOriginalRadius );
		m_ResizeData.vOriginalMax = _vec3( +m_ResizeData.fOriginalRadius, +m_ResizeData.fOriginalRadius, +m_ResizeData.fOriginalRadius );
	}

	// 센터
	_vec3 vCenter = ( m_ResizeData.vOriginalMin + m_ResizeData.vOriginalMax ) * 0.5f + m_ResizeData.vOffset;

	// 반지름
	_vec3 vDist = ( m_ResizeData.vOriginalMax - m_ResizeData.vOriginalMin ) * 0.5f;

	if ( m_ResizeData.fOriginalRadius <= 0.000001f )
	{
		_float fDist = 0.f;
		for ( _uint i = 0; i < 3; ++i )
		{
			fDist = *(((_float*)&vDist.x) + i);
			if ( m_ResizeData.fOriginalRadius < fDist )
				m_ResizeData.fOriginalRadius = fDist;
		}

		m_ResizeData.fOriginalRadius = fDist;
	}

	// 로컬 행렬
	_matrix matScale, matRot, matTrans;
	D3DXMatrixScaling( &matScale, vDist.x, vDist.y, vDist.z );
	D3DXMatrixRotationYawPitchRoll( &matRot, D3DXToRadian( m_ResizeData.vRotation.y ), D3DXToRadian( m_ResizeData.vRotation.x ), D3DXToRadian( m_ResizeData.vRotation.z ) );
	D3DXMatrixTranslation( &matTrans, vCenter.x, vCenter.y, vCenter.z );
	m_InfoData.matLocal = matScale * matRot * matTrans;

	return NOERROR;
}

void CCollider::Update_ColliderData( void )
{
	Modify_WorldMatrix();
	Modify_ColliderData();
}

void CCollider::Modify_WorldMatrix( void )
{
	_matrix matParent, matTransformWorld, matTransformWorldNotRot;

	D3DXMatrixIdentity( &matTransformWorld );
	D3DXMatrixIdentity( &matTransformWorldNotRot );
	D3DXMatrixIdentity( &matParent );

	if ( nullptr != m_ResizeData.pParent )
	{
		matParent = *m_ResizeData.pParent;
	}

	// 부모 행렬 구하는 작업..
	if ( nullptr != m_ResizeData.pTransformCom )
	{
		matTransformWorld = m_ResizeData.pTransformCom->Get_WorldMatrix();
		matTransformWorldNotRot = m_ResizeData.pTransformCom->Get_WorldMatrix_NotRot();
	}

	// Collider 의 최종 월드 행렬을 구함 ( 회전 포함과 미포함 둘 다 )..
	m_InfoData.matWorld = m_InfoData.matLocal * matParent * matTransformWorld;
	m_InfoData.matWorld_NotRot = m_InfoData.matLocal * matParent * matTransformWorldNotRot;

	// --------------------------- 스케일(가장 큰)의 영향을 받은 반지름 업데이트( 뼈 등의 스케일에는 영향을 받지 않을 것 ).. --------------------------- //
	_float fScale = D3DXVec3Length( (_vec3*)&m_InfoData.matWorld.m[0][0] );
	for ( _uint i = 1; i < 3; ++i )
	{
		_float fLength = D3DXVec3Length( (_vec3*)&m_InfoData.matWorld.m[i][0] );
		if ( fScale < fLength )
			fScale = fLength;
	}

	// Sphere 메쉬를 띄우기 위한 월드 행렬 세팅..
	//m_ColliderData.fRadius = m_ResizeData.fOriginalRadius * fScale;
	m_ColliderData.fRadius = fScale;
	_matrix matScale, matTrans;
	D3DXMatrixScaling( &matScale, m_ColliderData.fRadius, m_ColliderData.fRadius, m_ColliderData.fRadius );
	D3DXMatrixTranslation( &matTrans, m_InfoData.matWorld._41, m_InfoData.matWorld._42, m_InfoData.matWorld._43 );
	//D3DXMatrixTranslation( &matTrans, m_ColliderData.vCenter.x, m_ColliderData.vCenter.y, m_ColliderData.vCenter.z );
	m_DebugData.matSphereWorld = matScale * matTrans;
}

void CCollider::Modify_ColliderData( void )
{
	// Collider 의 8 개의 좌표 월드 변환..
	for ( _uint i = 0; i < 8; ++i )
		D3DXVec3TransformCoord( &m_ColliderData.vPoint[i], &m_ColliderData.vOriginalPoint[i], &m_InfoData.matWorld );

	// 센터 좌표 월드 변환..
	_vec3 vOffset;
	D3DXVec3TransformCoord( &m_ColliderData.vCenter, &_vec3( 0.f, 0.f, 0.f ), &m_InfoData.matWorld );

	// Min, Max 좌표 월드(Rotation 정보가 없는) 변환..
	D3DXVec3TransformCoord( &m_ColliderData.vMinWorld_NotRot, &m_ResizeData.vOriginalMin, &m_InfoData.matWorld_NotRot );
	D3DXVec3TransformCoord( &m_ColliderData.vMaxWorld_NotRot, &m_ResizeData.vOriginalMax, &m_InfoData.matWorld_NotRot );


	// --------------------------- OBB 에서 사용할 축들 업데이트.. --------------------------- //
	m_ColliderData.vAlign_Axis[0] = m_ColliderData.vPoint[2] - m_ColliderData.vPoint[3];
	m_ColliderData.vAlign_Axis[1] = m_ColliderData.vPoint[0] - m_ColliderData.vPoint[3];
	m_ColliderData.vAlign_Axis[2] = m_ColliderData.vPoint[7] - m_ColliderData.vPoint[3];

	for ( size_t i = 0; i < 3; ++i )
		D3DXVec3Normalize( &m_ColliderData.vAlign_Axis[i], &m_ColliderData.vAlign_Axis[i] );

	m_ColliderData.vProj_Axis[0] = (m_ColliderData.vPoint[5] + m_ColliderData.vPoint[2]) * 0.5f - m_ColliderData.vCenter;
	m_ColliderData.vProj_Axis[1] = (m_ColliderData.vPoint[5] + m_ColliderData.vPoint[0]) * 0.5f - m_ColliderData.vCenter;
	m_ColliderData.vProj_Axis[2] = (m_ColliderData.vPoint[5] + m_ColliderData.vPoint[7]) * 0.5f - m_ColliderData.vCenter;
}

void CCollider::Update_InfoData( void )
{
	if ( m_InfoData.bCollision )
	{
		m_InfoData.eState = COLLIDER_COL;
		m_InfoData.bCollision = false;
	}
	else if (m_InfoData.eState == COLLIDER_COL )
		m_InfoData.eState = COLLIDER_NOCOL;
}

_bool CCollider::ActCollisionWork( Engine::Collision & _Collision )
{
	return ActPushProcess( _Collision );
}

_bool CCollider::ActPushProcess( Engine::Collision & _Collision )
{
	_bool bOut = false;

	STATICOPTIONDATA& DstStaticOption = m_StaticOptionArr[StaticOption_Push];
	STATICOPTIONDATA SrcStaticOption;

	_Collision.pOtherCollider->Get_StaticOption( SrcStaticOption, StaticOption_Push );

	// 밀어내는 옵션을 사용하지 않는 콜리더들을 걸러낸다..
	if ( DstStaticOption.dwDetailData == PushOption_None || SrcStaticOption.dwDetailData == PushOption_None )
		return false;
	// 둘 다 밀어낼 수 없는 오브젝트 ( ex. 벽이나 건물 ) 인데 충돌을 했다 == 오류??..
	else if ( (DstStaticOption.dwDetailData & SrcStaticOption.dwDetailData) == PushOption_Fixed )
	{
		//MSG_BOX( "밀어낼 수 없는 오브젝트끼리 충돌하는 오류 발생!!!!" );
		return false;
	}

	if ( true == PushObject( _Collision, DstStaticOption.dwDetailData, SrcStaticOption.dwDetailData ) )
		bOut = true;

	return bOut;
}

_bool CCollider::PushObject( Engine::Collision& _Collision, const _ulong & _dwMyPushOption, const _ulong & _dwOtherPushOption )
{
	if ( _dwMyPushOption & PushOption_BePushedOut ) // 밀려야 하는 콜리더 속성이라면 밀려남..
	{
		CTransform* pDstTransform = m_ResizeData.pTransformCom;

		if ( nullptr == pDstTransform )
			return true;

		_vec3 vDstPos = *pDstTransform->Get_Infomation( Engine::CTransform::INFO_POSITION );

		// 겹친 부분의 반만큼 이동할 벡터 구함..
		_vec3 vPos = m_ColliderData.vCenter;
		_vec3 vDir( 0.f, 0.f, 0.f );
		_float fDistance = (_Collision.CollisionData.vOverlapSize.x) * 0.5f;

		D3DXVec3Normalize( &vDir, &(_Collision.CollisionData.vCollisionPos - vPos) );

		_vec3 vMoveDir = vDir * fDistance;
		vMoveDir.y = 0.f;

		// 상대도 밀려야 하는 콜리더 속성이라면 반만..
		if ( _dwOtherPushOption & PushOption_BePushedOut )
		{
			vDstPos -= vMoveDir;
		}
		else
		{
			vDstPos -= vMoveDir * 2.f;
		}

		pDstTransform->Set_Infomation( Engine::CTransform::INFO_POSITION, &vDstPos );
	}

	return true;
}

void CCollider::OnCollisionEvent( std::function<void( const Engine::Collision& )> _pEventFunction, const Collision& _Collision )
{
	Collision CollisionData = _Collision;

	// 충돌 전 밀려나거나 밀어내거나 하는 등의 일을 해야 한다면 실행..
	if ( false == ActCollisionWork( CollisionData ) )
	{
		if ( _pEventFunction )	// 다른 함수를 호출해야 할 때..
			_pEventFunction( _Collision );
		else if ( m_InfoData.pGameObject )
		{
			m_InfoData.pGameObject->OnCollision( _Collision );

			m_InfoData.bCollision = true;
		}
	}
}

void CCollider::Set_GameObject( Engine::CGameObject* _pGameObject )
{
	m_InfoData.pGameObject = _pGameObject;
	m_InfoData.pObjectTransform = dynamic_cast<CTransform*>(_pGameObject->Get_Component( L"Com_Transform" ));
}

CCollider * CCollider::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CCollider *	pInstance = new CCollider(pGraphicDev);

	if (FAILED(pInstance->Ready_Collider()))
	{
		MessageBox(0, L"CCollider Created Failed", nullptr, MB_OK);
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CCollider::Clone(void)
{
	return new CCollider(*this);
}
#ifndef _FINAL
void CCollider::Debug_Render()
{
	CDebug::GetInstance()->Add_DebugText("CCollider");
	CDebug::GetInstance()->Add_DebugText(m_ColliderData.fRadius, "m_ColliderData.fRadius");
	CDebug::GetInstance()->Add_DebugText(m_ColliderData.vAlign_Axis[0], "m_ColliderData.vAlign_Axis[0]");
	CDebug::GetInstance()->Add_DebugText(m_ColliderData.vAlign_Axis[1], "m_ColliderData.vAlign_Axis[1]");
	CDebug::GetInstance()->Add_DebugText(m_ColliderData.vAlign_Axis[2], "m_ColliderData.vAlign_Axis[2]");
	CDebug::GetInstance()->Add_DebugText(m_ColliderData.vCenter, "m_ColliderData.vCenter");
	CDebug::GetInstance()->Add_DebugText(m_ColliderData.vMaxWorld_NotRot, "m_ColliderData.vMaxWorld_NotRot");
	CDebug::GetInstance()->Add_DebugText(m_ColliderData.vMinWorld_NotRot, "m_ColliderData.vMinWorld_NotRot");
	CDebug::GetInstance()->Add_DebugText(m_ColliderData.vOriginalPoint[0], "m_ColliderData.vOriginalPoint[0]");
	CDebug::GetInstance()->Add_DebugText(m_ColliderData.vOriginalPoint[1], "m_ColliderData.vOriginalPoint[1]");
	CDebug::GetInstance()->Add_DebugText(m_ColliderData.vOriginalPoint[2], "m_ColliderData.vOriginalPoint[2]");
	CDebug::GetInstance()->Add_DebugText(m_ColliderData.vOriginalPoint[3], "m_ColliderData.vOriginalPoint[3]");
	CDebug::GetInstance()->Add_DebugText(m_ColliderData.vOriginalPoint[4], "m_ColliderData.vOriginalPoint[4]");
	CDebug::GetInstance()->Add_DebugText(m_ColliderData.vOriginalPoint[5], "m_ColliderData.vOriginalPoint[5]");
	CDebug::GetInstance()->Add_DebugText(m_ColliderData.vOriginalPoint[6], "m_ColliderData.vOriginalPoint[6]");
	CDebug::GetInstance()->Add_DebugText(m_ColliderData.vOriginalPoint[7], "m_ColliderData.vOriginalPoint[7]");
	CDebug::GetInstance()->Add_DebugText(m_ColliderData.vPoint[0], "m_ColliderData.vPoint[0]");
	CDebug::GetInstance()->Add_DebugText(m_ColliderData.vPoint[1], "m_ColliderData.vPoint[1]");
	CDebug::GetInstance()->Add_DebugText(m_ColliderData.vPoint[2], "m_ColliderData.vPoint[2]");
	CDebug::GetInstance()->Add_DebugText(m_ColliderData.vPoint[3], "m_ColliderData.vPoint[3]");
	CDebug::GetInstance()->Add_DebugText(m_ColliderData.vPoint[4], "m_ColliderData.vPoint[4]");
	CDebug::GetInstance()->Add_DebugText(m_ColliderData.vPoint[5], "m_ColliderData.vPoint[5]");
	CDebug::GetInstance()->Add_DebugText(m_ColliderData.vPoint[6], "m_ColliderData.vPoint[6]");
	CDebug::GetInstance()->Add_DebugText(m_ColliderData.vPoint[7], "m_ColliderData.vPoint[7]");
	CDebug::GetInstance()->Add_DebugText(m_ColliderData.vProj_Axis[0], "m_ColliderData.vProj_Axis[0]");
	CDebug::GetInstance()->Add_DebugText(m_ColliderData.vProj_Axis[1], "m_ColliderData.vProj_Axis[1]");
	CDebug::GetInstance()->Add_DebugText(m_ColliderData.vProj_Axis[2], "m_ColliderData.vProj_Axis[2]");


}
#endif

_ulong CCollider::Free(void)
{
	for (size_t i = 0; i < COLLIDER_END; i++)
		Safe_Release(m_DebugData.pTexture[i]);
	Safe_Release( m_DebugData.pSphereMesh );

	return CVIBuffer::Free();
}
