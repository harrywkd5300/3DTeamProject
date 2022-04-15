#include "stdafx.h"
#include "Interaction_Character_UseTool.h"

#include "Component_Manager.h"
#include "Collider_Manager.h"

#include "Staff_UseTool.h"
#include "Sword_UseTool.h"
#include "Sai_UseTool.h"
#include "Knife_UseTool.h"

CInteraction_Character_UseTool::CInteraction_Character_UseTool( LPDIRECT3DDEVICE9 _pGraphicDev )
	: Engine::CCharacter( _pGraphicDev )
	, m_pCollider_Manager( CCollider_Manager::GetInstance() )
{
}

void CInteraction_Character_UseTool::Set_CurAniIndex( const _uint & _iAniIndex )
{
	if ( nullptr != m_pMeshCom )
	{
		Reset_TransformData();

		m_pMeshCom->Set_AnimationSet( _iAniIndex );
	}
}

void CInteraction_Character_UseTool::Set_AniSpeed( const _float & _fAniSpeed )
{
	if ( nullptr != m_pMeshCom )
		m_pMeshCom->Set_Speed( _fAniSpeed );
}

void CInteraction_Character_UseTool::Set_IsStartAni( const _bool & _bIsStartAni )
{
	m_bIsStartAni = _bIsStartAni;

	//Reset_TransformData();
	//
	//_uint iIndex = m_pMeshCom->Get_CurAnimationInfo().iAni_Index;
	//if ( iIndex == 0 )
	//{
	//	if ( m_pMeshCom->Get_MaxAniSet() > 1 )
	//		m_pMeshCom->Set_AnimationSet( 1 );
	//}
	//else
	//	m_pMeshCom->Set_AnimationSet( 0 );
	//
	//m_pMeshCom->Set_AnimationSet( iIndex );
}

void CInteraction_Character_UseTool::Set_StartPosition( const _vec3 & _vPos )
{
	m_vStartPos = _vPos;
}

void CInteraction_Character_UseTool::Set_StartAngle( const _vec3 & _vAngle )
{
	m_vStartAngle = _vAngle;
}

_double CInteraction_Character_UseTool::Get_AniPeriod( void )
{
	if ( nullptr == m_pPeriod )
		return 0;

	return *m_pPeriod;
}

_double CInteraction_Character_UseTool::Get_AniPosition( void )
{
	if ( nullptr == m_pTrackDesc )
		return 0;

	return m_pTrackDesc->Position;
}

void CInteraction_Character_UseTool::LookCharacter( CInteraction_Character_UseTool * _pLookCharacter, const _float & _fOffset )
{
	if ( nullptr == _pLookCharacter )
		return;

	_vec3 vAngle( 0.f, Compute_LookAngle( _pLookCharacter->Get_Transform(), _fOffset ), 0.f );

	m_pTransformCom->Set_Infomation( Engine::CTransform::INFO_ANGLE, &vAngle );
}

void CInteraction_Character_UseTool::PositionUpdate( CInteraction_Character_UseTool* _pLookCharacter, const _float & _fDist )
{
	if ( nullptr == _pLookCharacter )
		return;

	_vec3 vDstPos = *m_pTransformCom->Get_Infomation( Engine::CTransform::INFO_POSITION );
	_vec3 vSrcPos = *_pLookCharacter->Get_Transform()->Get_Infomation( Engine::CTransform::INFO_POSITION );
	_vec3 vSrcLook = *_pLookCharacter->Get_Transform()->Get_WorldLookVec(); vSrcLook.y = 0.f;

	vDstPos = vSrcPos + vSrcLook * _fDist;

	m_pTransformCom->Set_Infomation( Engine::CTransform::INFO_POSITION, &vDstPos );
}

HRESULT CInteraction_Character_UseTool::Ready_GameObject( void )
{
	return NOERROR;
}

_int CInteraction_Character_UseTool::Update_GameObject( const _float & _fTimeDelta )
{
	if ( true == m_bIsDead )
		return 1;

	m_tInfo.m_WorldState.setVariable( CH_STATE_DIE, false );

	if ( m_pMeshCom->is_Finish_Animation() >= 1 )
	{
		Reset_TransformData();

		_uint iIndex = m_pMeshCom->Get_CurAnimationInfo().iAni_Index;
		if ( iIndex == 0 )
		{
			if ( m_pMeshCom->Get_MaxAniSet() > 1 )
				m_pMeshCom->Set_AnimationSet( 1 );
		}
		else
			m_pMeshCom->Set_AnimationSet( 0 );

		m_pMeshCom->Set_AnimationSet( iIndex );

		m_bIsStartAni = false;
	}

	_int iExitCode = 0;

	if ( true == m_bIsStartAni )
		iExitCode = Engine::CCharacter::Update_GameObject( _fTimeDelta );
	else
		iExitCode = Engine::CCharacter::Update_GameObject( 0.f );

	_float fSpeed = m_fSpeed;
	if ( CInput_Device::GetInstance()->Get_DIKeyState( DIK_LSHIFT ) )
		fSpeed = m_fSpeed * 2.f;

	if ( CInput_Device::GetInstance()->Get_DIKeyState( DIK_RIGHTARROW ) )
		m_pTransformCom->Add_Infomation( CTransform::INFO_ANGLE, &_vec3( 0.f, m_fRotSpeed * _fTimeDelta, 0.f ) );
	else if ( CInput_Device::GetInstance()->Get_DIKeyState( DIK_LEFTARROW ) )
		m_pTransformCom->Add_Infomation( CTransform::INFO_ANGLE, &_vec3( 0.f, -m_fRotSpeed * _fTimeDelta, 0.f ) );
	if ( CInput_Device::GetInstance()->Get_DIKeyState( DIK_UPARROW ) )
		m_pTransformCom->Go_Straight( fSpeed, _fTimeDelta );
	else if ( CInput_Device::GetInstance()->Get_DIKeyState( DIK_DOWNARROW ) )
		m_pTransformCom->Go_Straight( -fSpeed, _fTimeDelta );

	if ( nullptr == m_pRendererCom )
		return -1;
	if ( FAILED( m_pRendererCom->Add_RenderGroup( Engine::CRenderer::RENDER_NONALPHA, this ) ) )
		return -1;

	return iExitCode;
}

_int CInteraction_Character_UseTool::LastUpdate_GameObject( const _float & _fTimeDelta )
{
	return Engine::CCharacter::LastUpdate_GameObject( _fTimeDelta );
}

void CInteraction_Character_UseTool::Render_GameObject( void )
{
	if ( nullptr == m_pMeshCom )
		return;
	if ( nullptr == m_pGraphicDev || nullptr == m_pEffect )
		return;

//#ifndef _FINAL
//#ifdef CHARACTER_COLLIDERMODIFY	// Add By.KMH - 모든 콜라이더들 렌더 하는 코드 추가..
//	for ( _uint i = 0; i < m_ColliderGroups.iGroupCount; ++i )
//	{
//		for ( _uint j = 0; j < m_ColliderGroups.pGroup[i].iColliderCount; ++j )
//		{
//			m_ColliderGroups.pGroup[i].ppCollider[j]->Render_Buffer();
//		}
//	}
//#else
//	m_pColliderCom->Render_Buffer();
//#endif
//#endif 

	m_pEffect->AddRef();

	if ( FAILED( SetUp_OnConstantTable( m_pEffect ) ) )
		return;

	m_pEffect->Begin( nullptr, 0 );

	m_pMeshCom->Render_Mesh( m_pEffect, "g_BaseTexture", "g_NormalTexture", "g_SpecularTexture" );

	m_pEffect->End();
}

HRESULT CInteraction_Character_UseTool::Ready_Interaction_Character( const CHARACTERKIND& _eCharKind )
{
	if ( FAILED( Ready_Component( _eCharKind ) ) )
		return E_FAIL;
	if ( FAILED( Ready_DefaultSetting() ) )
		return E_FAIL;

	return NOERROR;
}

HRESULT CInteraction_Character_UseTool::Ready_Component( const CHARACTERKIND& _eCharKind )
{
	Engine::CComponent* pComponent = nullptr;

	if (FAILED(CCharacter::Ready_Character_Component()))
		return E_FAIL;

	// For.Shader
	pComponent = m_pShaderCom = (Engine::CShader*)CComponent_Manager::GetInstance()->Clone_Component( SCENE_TOOL, L"CShader_Mesh" );
	if ( nullptr == pComponent )
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Shader", pComponent));
	m_pShaderCom->AddRef();

	const _tchar* pMeshComTag = nullptr;

	switch ( _eCharKind )
	{
		case Character_Donatello:
			m_tInfo.m_eCharType = CHARINFO::CHAR_PLAYER;
			pMeshComTag = L"CMesh_Dynamic_Donatello";
			break;
		case Character_Leronard:
			m_tInfo.m_eCharType = CHARINFO::CHAR_PLAYER;
			pMeshComTag = L"CMesh_Dynamic_Leonard";
			break;
		case Character_Michelangelo:
			m_tInfo.m_eCharType = CHARINFO::CHAR_PLAYER;
			pMeshComTag = L"CMesh_Dynamic_Michelangelo";
			break;
		case Character_Raphael:
			m_tInfo.m_eCharType = CHARINFO::CHAR_PLAYER;
			pMeshComTag = L"CMesh_Dynamic_Raphael";
			break;
		case Character_KinfeThug:
			m_tInfo.m_eCharType = CHARINFO::CHAR_MONSTER;
			pMeshComTag = L"CMesh_Dynamic_KnifeThug";
			break;
		case Character_SlimThug:
			m_tInfo.m_eCharType = CHARINFO::CHAR_MONSTER;
			pMeshComTag = L"CMesh_Dynamic_SlimThug";
			break;
		case Character_BigThug:
			m_tInfo.m_eCharType = CHARINFO::CHAR_MONSTER;
			pMeshComTag = L"CMesh_Dynamic_BigThug";
			break;
		case Character_FootSoldier:
			m_tInfo.m_eCharType = CHARINFO::CHAR_MONSTER;
			pMeshComTag = L"CMesh_Dynamic_";
			break;
		default:
			return E_FAIL;
	}

	// For.Mesh
	pComponent = m_pMeshCom = (Engine::CMesh_Dynamic*)CComponent_Manager::GetInstance()->Clone_Component( SCENE_TOOL, pMeshComTag );
	if ( nullptr == pComponent )
	{
		if ( FAILED( Create_Mesh( _eCharKind, pMeshComTag ) ) )
			return E_FAIL;

		pComponent = m_pMeshCom = (Engine::CMesh_Dynamic*)CComponent_Manager::GetInstance()->Clone_Component( SCENE_TOOL, pMeshComTag );
		if ( nullptr == pComponent )
			return E_FAIL;
		m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Mesh", pComponent));
		m_pMeshCom->AddRef();
		m_pMeshCom->Set_RootMotionFrame("Origin");
	}
	else
	{
		m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Mesh", pComponent));
		m_pMeshCom->AddRef();
		m_pMeshCom->Set_RootMotionFrame("Origin");
	}

	// For.Collider - Add By.KMH
	//Engine::CCollider* pCollider = (Engine::CCollider*)CComponent_Manager::GetInstance()->Clone_Component( SCENE_TOOL, L"CCollider" );
	//if ( nullptr == pCollider )
	//	return E_FAIL;
	//
	//if ( FAILED( Character_LoadCollider( _eCharKind, pCollider ) ) )
	//	return E_FAIL;
	//if ( FAILED( Character_ColliderSetting( _eCharKind ) ) )
	//	return E_FAIL;
	if ( FAILED( Ready_Equipment( _eCharKind ) ) )
		return E_FAIL;

	//Engine::Safe_Release( pCollider );

	return NOERROR;
}

HRESULT CInteraction_Character_UseTool::Ready_DefaultSetting( void )
{
	if ( nullptr == m_pShaderCom )
		return E_FAIL;

	m_pEffect = m_pShaderCom->Get_EffectHandle();

	if ( nullptr == m_pEffect )
		return E_FAIL;

	m_pEffect->AddRef();

	if ( nullptr == m_pTransformCom )
		return E_FAIL;

	Reset_TransformData();

	m_tInfo.m_fSpeed = 5.f;

	m_pPeriod = m_pMeshCom->Get_Pointer_Period();
	m_pTrackDesc = m_pMeshCom->Get_TrackInfo();

	if ( nullptr == m_pPeriod || nullptr == m_pTrackDesc )
		return E_FAIL;

	m_pMeshCom->Set_AnimationSet( 325 );

	return NOERROR;
}

HRESULT CInteraction_Character_UseTool::SetUp_OnConstantTable( LPD3DXEFFECT _pEffect )
{
	_pEffect->AddRef();

	m_pTransformCom->SetUp_OnShader( _pEffect, "g_matWorld" );

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform( D3DTS_VIEW, &matView );
	m_pGraphicDev->GetTransform( D3DTS_PROJECTION, &matProj );

	_pEffect->SetMatrix( "g_matView", &matView );
	_pEffect->SetMatrix( "g_matProj", &matProj );

	Safe_Release( _pEffect );

	return NOERROR;
}

HRESULT CInteraction_Character_UseTool::Character_LoadCollider( const CHARACTERKIND & _eCharKind, Engine::CCollider * _pCollder )
{
	switch ( _eCharKind )
	{
		case Character_Donatello:
			Ready_LoadCollider( L"../Bin/Data/ColliderData/Don_Collider.Colldat", _pCollder );
			break;
		case Character_Leronard:
			Ready_LoadCollider( L"../Bin/Data/ColliderData/Leo_Collider.Colldat", _pCollder );
			break;
		case Character_Michelangelo:
			Ready_LoadCollider( L"../Bin/Data/ColliderData/Mike_Collider.Colldat", _pCollder );
			break;
		case Character_Raphael:
			Ready_LoadCollider( L"../Bin/Data/ColliderData/Raph_Collider.Colldat", _pCollder );
			break;
		case Character_KinfeThug:
			Ready_LoadCollider( L"../Bin/Data/ColliderData/Knife_Collider.Colldat", _pCollder );
			break;
		case Character_SlimThug:
			Ready_LoadCollider( L"../Bin/Data/ColliderData/Slim_Collider.Colldat", _pCollder );
			break;
		case Character_BigThug:
			Ready_LoadCollider( L"../Bin/Data/ColliderData/Big_Collider.Colldat", _pCollder );
			break;
		case Character_FootSoldier:
			Ready_LoadCollider( L"../Bin/Data/ColliderData/Don_Collider.Colldat", _pCollder );
			break;
		default:
			AfxMessageBox( L"Strange Character Kind" );
			return E_FAIL;
	}

	return NOERROR;
}

HRESULT CInteraction_Character_UseTool::Character_ColliderSetting( const CHARACTERKIND & _eCharKind )
{
	switch ( _eCharKind )
	{
		case Character_Donatello:
		{
			// -------------- 콜리더를 콜리더 매니저에 그룹별로 등록하는 작업.. -------------- //
			if ( FAILED( Ready_CollidersToColliderManager( CCollider_Manager::GROUP_STATIC, this ) ) )
				return E_FAIL;

			// -------------- 콜리더 그룹을 콜리더 레이어에 넣어서 충돌 연산을 활성화하기.. -------------- //
			_tchar szLayerName[256] = L"";
			swprintf_s( szLayerName, L"Layer_Don_Hit%d", GetKey() );
			const _tchar* pLayerName = CString_Manager::GetInstance()->GetStr( szLayerName );

			if ( false == Colliders_Add_ColliderLayer( CCollider_Manager::GROUP_STATIC, pLayerName, DonColGroup_Hit ) )
				return E_FAIL;
			if ( false == Colliders_Add_ColliderLayer( CCollider_Manager::GROUP_STATIC, L"Layer_Player_Attack", DonColGroup_RWeapon ) )
				return E_FAIL;
			if ( false == Colliders_Add_ColliderLayer( CCollider_Manager::GROUP_STATIC, L"Layer_Player_Attack", DonColGroup_RFoot ) )
				return E_FAIL;
			if ( false == Colliders_Add_ColliderLayer( CCollider_Manager::GROUP_STATIC, L"Layer_Player_Attack", DonColGroup_LFoot ) )
				return E_FAIL;

			// -------------- Hit 용 콜리더와 Attack 용 콜리더는 연산할 필요 없기 때문에 연산하지 못하게.. -------------- //
			m_pCollider_Manager->Ignore_ColliderLayer( L"Layer_Player_Attack", pLayerName );
			m_pCollider_Manager->Ignore_ColliderLayer( L"Layer_Player_Attack", L"Layer_Monster_Attack" );
		}
		break;
		case Character_Leronard:
		{
			// -------------- 콜리더를 콜리더 매니저에 그룹별로 등록하는 작업.. -------------- //
			if ( FAILED( Ready_CollidersToColliderManager( CCollider_Manager::GROUP_STATIC, this ) ) )
				return E_FAIL;

			// -------------- 콜리더 그룹을 콜리더 레이어에 넣어서 충돌 연산을 활성화하기.. -------------- //
			_tchar szLayerName[256] = L"";
			swprintf_s( szLayerName, L"Layer_Leo_Hit%d", GetKey() );
			const _tchar* pLayerName = CString_Manager::GetInstance()->GetStr( szLayerName );

			if ( false == Colliders_Add_ColliderLayer( CCollider_Manager::GROUP_STATIC, pLayerName, LeoColGroup_Hit ) )
				return E_FAIL;
			if ( false == Colliders_Add_ColliderLayer( CCollider_Manager::GROUP_STATIC, L"Layer_Player_Attack", LeoColGroup_RWeapon ) )
				return E_FAIL;
			if ( false == Colliders_Add_ColliderLayer( CCollider_Manager::GROUP_STATIC, L"Layer_Player_Attack", LeoColGroup_LWeapon ) )
				return E_FAIL;
			if ( false == Colliders_Add_ColliderLayer( CCollider_Manager::GROUP_STATIC, L"Layer_Player_Attack", LeoColGroup_LFoot ) )
				return E_FAIL;
			if ( false == Colliders_Add_ColliderLayer( CCollider_Manager::GROUP_STATIC, L"Layer_Player_Attack", LeoColGroup_RFoot ) )
				return E_FAIL;

			// -------------- Hit 용 콜리더와 Attack 용 콜리더는 연산할 필요 없기 때문에 연산하지 못하게..
			m_pCollider_Manager->Ignore_ColliderLayer( L"Layer_Player_Attack", pLayerName );
			m_pCollider_Manager->Ignore_ColliderLayer( L"Layer_Player_Attack", L"Layer_Monster_Attack" );
		}
		break;
		case Character_Michelangelo:
		{
			// -------------- 콜리더를 콜리더 매니저에 그룹별로 등록하는 작업.. -------------- //
			if ( FAILED( Ready_CollidersToColliderManager( CCollider_Manager::GROUP_STATIC, this ) ) )
				return E_FAIL;

			// -------------- 콜리더 그룹을 콜리더 레이어에 넣어서 충돌 연산을 활성화하기.. -------------- //
			_tchar szLayerName[256] = L"";
			swprintf_s( szLayerName, L"Layer_Mike_Hit%d", GetKey() );
			const _tchar* pLayerName = CString_Manager::GetInstance()->GetStr( szLayerName );

			if ( false == Colliders_Add_ColliderLayer( CCollider_Manager::GROUP_STATIC, pLayerName, MikeColGroup_Hit ) )
				return E_FAIL;
			if ( false == Colliders_Add_ColliderLayer( CCollider_Manager::GROUP_STATIC, L"Layer_Player_Attack", MikeColGroup_RWeapon ) )
				return E_FAIL;
			if ( false == Colliders_Add_ColliderLayer( CCollider_Manager::GROUP_STATIC, L"Layer_Player_Attack", MikeColGroup_LWeapon ) )
				return E_FAIL;
			if ( false == Colliders_Add_ColliderLayer( CCollider_Manager::GROUP_STATIC, L"Layer_Player_Attack", MikeColGroup_RKnee ) )
				return E_FAIL;
			if ( false == Colliders_Add_ColliderLayer( CCollider_Manager::GROUP_STATIC, L"Layer_Player_Attack", MikeColGroup_LKnee ) )
				return E_FAIL;

			// -------------- Hit 용 콜리더와 Attack 용 콜리더는 연산할 필요 없기 때문에 연산하지 못하게..
			m_pCollider_Manager->Ignore_ColliderLayer( L"Layer_Player_Attack", pLayerName );
			m_pCollider_Manager->Ignore_ColliderLayer( L"Layer_Player_Attack", L"Layer_Monster_Attack" );
		}
		break;
		case Character_Raphael:
		{
			// -------------- 콜리더를 콜리더 매니저에 그룹별로 등록하는 작업.. -------------- //
			if ( FAILED( Ready_CollidersToColliderManager( CCollider_Manager::GROUP_STATIC, this ) ) )
				return E_FAIL;

			// -------------- 콜리더 그룹을 콜리더 레이어에 넣어서 충돌 연산을 활성화하기.. -------------- //
			_tchar szLayerName[256] = L"";
			swprintf_s( szLayerName, L"Layer_Raph_Hit%d", GetKey() );
			const _tchar* pLayerName = CString_Manager::GetInstance()->GetStr( szLayerName );

			if ( false == Colliders_Add_ColliderLayer( CCollider_Manager::GROUP_STATIC, pLayerName, RaphColGroup_Hit ) )
				return E_FAIL;
			if ( false == Colliders_Add_ColliderLayer( CCollider_Manager::GROUP_STATIC, L"Layer_Player_Attack", RaphColGroup_RWeapon ) )
				return E_FAIL;
			if ( false == Colliders_Add_ColliderLayer( CCollider_Manager::GROUP_STATIC, L"Layer_Player_Attack", RaphColGroup_LWeapon ) )
				return E_FAIL;
			if ( false == Colliders_Add_ColliderLayer( CCollider_Manager::GROUP_STATIC, L"Layer_Player_Attack", RaphColGroup_RFoot ) )
				return E_FAIL;
			if ( false == Colliders_Add_ColliderLayer( CCollider_Manager::GROUP_STATIC, L"Layer_Player_Attack", RaphColGroup_LFoot ) )
				return E_FAIL;
			if ( false == Colliders_Add_ColliderLayer( CCollider_Manager::GROUP_STATIC, L"Layer_Player_Attack", RaphColGroup_RKnee ) )
				return E_FAIL;
			if ( false == Colliders_Add_ColliderLayer( CCollider_Manager::GROUP_STATIC, L"Layer_Player_Attack", RaphColGroup_LKnee ) )
				return E_FAIL;

			// -------------- Hit 용 콜리더와 Attack 용 콜리더는 연산할 필요 없기 때문에 연산하지 못하게..
			m_pCollider_Manager->Ignore_ColliderLayer( L"Layer_Player_Attack", pLayerName );
			m_pCollider_Manager->Ignore_ColliderLayer( L"Layer_Player_Attack", L"Layer_Monster_Attack" );
		}
		break;
		case Character_KinfeThug:
		{
			if ( FAILED( Ready_CollidersToColliderManager( CCollider_Manager::GROUP_STATIC, this ) ) )
				return E_FAIL;

			_tchar szLayerName[256] = L"";
			swprintf_s(szLayerName, L"Layer_KnifeThug_Hit%d", GetKey());
			const _tchar* pLayerName = CString_Manager::GetInstance()->GetStr(szLayerName);

			if ( false == Colliders_Add_ColliderLayer( CCollider_Manager::GROUP_STATIC, pLayerName, KnifeThugColGroup_Hit ) )
				return E_FAIL;
			if ( false == Colliders_Add_ColliderLayer( CCollider_Manager::GROUP_STATIC, L"Layer_Monster_Attack", KnifeThugColGroup_Attack ) )
				return E_FAIL;

			// -------------- Hit 용 콜리더와 Attack 용 콜리더는 연산할 필요 없기 때문에 연산하지 못하게.. -------------- //
			m_pCollider_Manager->Ignore_ColliderLayer( L"Layer_Monster_Attack", pLayerName );
			m_pCollider_Manager->Ignore_ColliderLayer( L"Layer_Player_Attack", L"Layer_Monster_Attack" );
		}
		break;
		case Character_SlimThug:
		{
			if ( FAILED( Ready_CollidersToColliderManager( CCollider_Manager::GROUP_STATIC, this ) ) )
				return E_FAIL;

			_tchar szLayerName[256] = L"";
			swprintf_s( szLayerName, L"Layer_SlimThug_Hit%d", GetKey() );
			const _tchar* pLayerName = CString_Manager::GetInstance()->GetStr( szLayerName );

			if ( false == Colliders_Add_ColliderLayer( CCollider_Manager::GROUP_STATIC, pLayerName, SlimThugColGroup_Hit ) )
				return E_FAIL;
			if ( false == Colliders_Add_ColliderLayer( CCollider_Manager::GROUP_STATIC, L"Layer_Monster_Attack", SlimThugColGroup_Attack ) )
				return E_FAIL;

			// -------------- Hit 용 콜리더와 Attack 용 콜리더는 연산할 필요 없기 때문에 연산하지 못하게.. -------------- //
			m_pCollider_Manager->Ignore_ColliderLayer( L"Layer_Monster_Attack", pLayerName );
			m_pCollider_Manager->Ignore_ColliderLayer( L"Layer_Player_Attack", L"Layer_Monster_Attack" );
		}
		break;
		case Character_BigThug:
		{
			if ( FAILED( Ready_CollidersToColliderManager( CCollider_Manager::GROUP_STATIC, this ) ) )
				return E_FAIL;

			_tchar szLayerName[256] = L"";
			swprintf_s(szLayerName, L"Layer_BigThug_Hit%d", GetKey());
			const _tchar* pLayerName = CString_Manager::GetInstance()->GetStr(szLayerName);

			if ( false == Colliders_Add_ColliderLayer( CCollider_Manager::GROUP_STATIC, pLayerName, BigThugColGroup_Hit ) )
				return E_FAIL;
			if ( false == Colliders_Add_ColliderLayer( CCollider_Manager::GROUP_STATIC, L"Layer_Monster_Attack", BigThugColGroup_Attack ) )
				return E_FAIL;

			// -------------- Hit 용 콜리더와 Attack 용 콜리더는 연산할 필요 없기 때문에 연산하지 못하게.. -------------- //
			m_pCollider_Manager->Ignore_ColliderLayer( L"Layer_Monster_Attack", pLayerName );
			m_pCollider_Manager->Ignore_ColliderLayer( L"Layer_Player_Attack", L"Layer_Monster_Attack" );
		}
		break;
		case Character_FootSoldier:

		break;
		default:
		{
			AfxMessageBox( L"Strange Character Kind" );
		}
		return E_FAIL;
	}

	return NOERROR;
}

HRESULT CInteraction_Character_UseTool::Ready_Equipment( const CHARACTERKIND & _eCharKind )
{
	switch ( _eCharKind )
	{
		case Character_Donatello:
			if ( FAILED( Equip_Item( CStaff_UseTool::Create( m_pGraphicDev ), m_pMeshCom->Find_FrameMatrixPointer( "R_Staff" ) ) ) )
				return E_FAIL;
			break;
		case Character_Leronard:
			if ( FAILED( Equip_Item( CSword_UseTool::Create( m_pGraphicDev ), m_pMeshCom->Find_FrameMatrixPointer( "R_Sword1" ) ) ) )
				return E_FAIL;
			if ( FAILED( Equip_Item( CSword_UseTool::Create( m_pGraphicDev ), m_pMeshCom->Find_FrameMatrixPointer( "L_Sword1" ) ) ) )
				return E_FAIL;
			break;
		case Character_Michelangelo:
			break;
		case Character_Raphael:
			if ( FAILED( Equip_Item( CSai_UseTool::Create( m_pGraphicDev ), m_pMeshCom->Find_FrameMatrixPointer( "R_Sword1" ) ) ) )
				return E_FAIL;
			if ( FAILED( Equip_Item( CSai_UseTool::Create( m_pGraphicDev ), m_pMeshCom->Find_FrameMatrixPointer( "L_Sword1" ) ) ) )
				return E_FAIL;
			break;
		case Character_KinfeThug:
			if ( FAILED( Equip_Item( CKnife_UseTool::Create( m_pGraphicDev ), m_pMeshCom->Find_FrameMatrixPointer( "SaberStaff_Root" ) ) ) )
				return E_FAIL;
			break;
		case Character_SlimThug:
			if ( FAILED( Equip_Item( CKnife_UseTool::Create( m_pGraphicDev ), m_pMeshCom->Find_FrameMatrixPointer( "SaberStaff_Root" ) ) ) )
				return E_FAIL;
			break;
		case Character_BigThug:
			if ( FAILED( Equip_Item( CKnife_UseTool::Create( m_pGraphicDev ), m_pMeshCom->Find_FrameMatrixPointer( "SaberStaff_Root" ) ) ) )
				return E_FAIL;
			break;
		case Character_FootSoldier:
			break;
		default:
			return E_FAIL;
	}

	return NOERROR;
}

HRESULT CInteraction_Character_UseTool::Create_Mesh( const CHARACTERKIND & _eCharKind, const _tchar * _pMeshComTag )
{
	Engine::CComponent* pComponent = nullptr;

	switch ( _eCharKind )
	{
		case Character_Donatello:
		{
			// For.CMesh_Dynamic_Donatello..
			pComponent = Engine::CMesh_Dynamic::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/DynamicMesh/Donatello/", L"Donatello.X", _vec3(270.f, 180.f, 0.f));
			if (nullptr == pComponent)
				return E_FAIL;
			if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_TOOL, _pMeshComTag, pComponent)))
				return E_FAIL;
		}
		break;
		case Character_Leronard:
		{
			// For.CMesh_Dynamic_Leonard..
			pComponent = Engine::CMesh_Dynamic::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/DynamicMesh/Leonardo/", L"Leonardo.X", _vec3(270.f, 180.f, 0.f));
			if (nullptr == pComponent)
				return E_FAIL;
			if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_TOOL, _pMeshComTag, pComponent)))
				return E_FAIL;
		}
		break;
		case Character_Michelangelo:
		{
			// For.CMesh_Dynamic_Michelangelo..
			pComponent = Engine::CMesh_Dynamic::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/DynamicMesh/Michelangelo/", L"Michelangelo.X", _vec3(270.f, 180.f, 0.f));
			if (nullptr == pComponent)
				return E_FAIL;
			if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_TOOL, _pMeshComTag, pComponent)))
				return E_FAIL;
		}
		break;
		case Character_Raphael:
		{
			// For.CMesh_Dynamic_Raphael..
			pComponent = Engine::CMesh_Dynamic::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/DynamicMesh/Raphael/", L"Raphael.X", _vec3(270.f, 180.f, 0.f));
			if (nullptr == pComponent)
				return E_FAIL;
			if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_TOOL, _pMeshComTag, pComponent)))
				return E_FAIL;
		}
		break;
		case Character_KinfeThug:
		{
			// For.Component_Mesh_KnifeThug
			pComponent = Engine::CMesh_Dynamic::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/DynamicMesh/Monster/KnifeThug/", L"KnifeThug.X", _vec3(270.f, 180.f, 0.f));
			if (nullptr == pComponent)
				return E_FAIL;
			if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_TOOL, _pMeshComTag, pComponent)))
				return E_FAIL;
		}
		break;
		case Character_SlimThug:
		{
			// For.Component_Mesh_SlimThug
			pComponent = Engine::CMesh_Dynamic::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/DynamicMesh/Monster/SlimThug/", L"SlimThug.X", _vec3(270.f, 180.f, 0.f));
			if (nullptr == pComponent)
				return E_FAIL;
			if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_TOOL, _pMeshComTag, pComponent)))
				return E_FAIL;
		}
		break;
		case Character_BigThug:
		{
			// For.Component_Mesh_BigThug
			pComponent = Engine::CMesh_Dynamic::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/DynamicMesh/Monster/BigThug/", L"BigThug.X", _vec3(270.f, 180.f, 0.f));
			if (nullptr == pComponent)
				return E_FAIL;
			if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_TOOL, _pMeshComTag, pComponent)))
				return E_FAIL;		
		}
		break;
		case Character_FootSoldier:
		{
			
		}
		break;
	}

	return NOERROR;
}

_float CInteraction_Character_UseTool::Compute_LookAngle( CTransform * _pTransform, const _float & _fOffsetAngle )
{
	// 상대방의 Look 각도 + _fOffsetAngle 을 최종 각도로 사용..
	_vec3 vSrcLook = *_pTransform->Get_WorldLookVec();

	if ( D3DXVec3Length( &vSrcLook ) <= 0.0001f )
		return _fOffsetAngle;

	vSrcLook.y = 0.f;
	D3DXVec3Normalize( &vSrcLook, &vSrcLook );

	_float fAngle = D3DXToDegree( acosf( D3DXVec3Dot( &_vec3( 0.f, 0.f, 1.f ), &vSrcLook ) ) );
	if ( vSrcLook.x < 0.f )
		fAngle *= -1.f;

	fAngle += _fOffsetAngle;

	if ( fAngle < 0.f )
		fAngle += 360.f;
	else if ( fAngle > 360.f )
		fAngle -= 360.f;

	return fAngle;

	// 상대방과 나의 위치 각도 + _fOffsetAngle 을 최종 각도로 사용..
	//_vec3 vDstPos = *m_pTransformCom->Get_Infomation( Engine::CTransform::INFO_POSITION );
	//_vec3 vSrcPos = *_pTransform->Get_Infomation( Engine::CTransform::INFO_POSITION );
	//_vec3 vDirDstToSrc = vSrcPos - vDstPos; vDirDstToSrc.y = 0.f;
	//
	//if ( D3DXVec3Length( &vDirDstToSrc ) <= 0.0001f )
	//	return _fOffsetAngle;
	//
	//D3DXVec3Normalize( &vDirDstToSrc, &vDirDstToSrc );
	//
	//_float fAngle = D3DXToDegree( acosf( D3DXVec3Dot( &_vec3( 0.f, 0.f, 1.f ), &vDirDstToSrc ) ) );
	//if ( vDirDstToSrc.x < 0.f )
	//	fAngle *= -1.f;
	//
	//fAngle += _fOffsetAngle;
	//
	//if ( fAngle < 0.f )
	//	fAngle += 360.f;
	//else if ( fAngle > 360.f )
	//	fAngle -= 360.f;
	//
	//return fAngle;
}

void CInteraction_Character_UseTool::Reset_TransformData( void )
{
	m_pTransformCom->Set_Infomation( Engine::CTransform::INFO_POSITION, &m_vStartPos );
	m_pTransformCom->Set_Infomation( Engine::CTransform::INFO_ANGLE, &m_vStartAngle );
	m_pTransformCom->Set_Infomation( Engine::CTransform::INFO_SCALE, &m_vStartScale );
}

CInteraction_Character_UseTool* CInteraction_Character_UseTool::Create( LPDIRECT3DDEVICE9 _pGraphicDev, const CHARACTERKIND& _eCharKind )
{
	CInteraction_Character_UseTool* pInstance = new CInteraction_Character_UseTool( _pGraphicDev );

	if ( FAILED( pInstance->Ready_Interaction_Character( _eCharKind ) ) )
	{
		CREATE_FAIL( pInstance, "CInteraction_Character_UseTool" );
	}

	return pInstance;
}

_ulong CInteraction_Character_UseTool::Free( void )
{
	_ulong dwRefCnt = 0;

	dwRefCnt = Engine::Safe_Release( m_pEffect );

	return Engine::CCharacter::Free();
}
