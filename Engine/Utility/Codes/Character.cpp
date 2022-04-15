#include "..\Headers\Character.h"
#include "Component_Manager.h"
#include "Equipment.h"
#include "Camera_Manager.h"
#include "Collider_Manager.h"
#include "Effect_Manager.h"
#include "Management.h"
#include "Scene.h"

USING(Engine)

void tagColliderGroup::Release( void )
{
	for ( _uint i = 0; i < iColliderCount; ++i )
	{
		Engine::Safe_Release( ppCollider[i] );
	}
	Engine::Safe_Delete_Array( ppCollider );

	iColliderCount = 0;
}

CCharacter::CCharacter(LPDIRECT3DDEVICE9 pGraphicDev)
	:CGameObject(pGraphicDev)
	//, m_pTransformCom(nullptr)
	, m_pRendererCom(nullptr)
	, m_pMeshCom(nullptr)
	, m_pNaviCom(nullptr)
	, m_pShaderCom(nullptr)
	, m_pCollider_Manager( CCollider_Manager::GetInstance() )	// Add By.KMH
#ifndef CHARACTER_COLLIDERMODIFY
	, m_pColliderCom(nullptr)
#endif
	, m_pController(nullptr)
	
{
	m_pCollider_Manager->AddRef();	// Add By.KMH
	m_tInfo.m_MoveDelta = _vec3(0.f, 0.f, 0.f);
#ifndef _FINAL
	Tag(L"CCharacter", std::bind(&CCharacter::Debug_Render, this));
#endif
}

_int CCharacter::Update_GameObject(const _float& fTimeDelta)
{
	_int iReturn = CGameObject::Update_GameObject(fTimeDelta);

	if (m_bNavi)
		Move_Navigation();

	if (!m_tInfo.m_WorldState.getVariable(CH_STATE_DIE))
	{
		if (m_pController != nullptr)
		{
			//if(m_pController->Get_Target() == this)
			m_pController->Update_Component(fTimeDelta);
		}
	}
	else
	{
		if (m_pMeshCom->is_Finish_Animation() >= 1)
		{
			//m_bIsDead = true;
			Set_IsDead(true);
		}
	}

	if (m_pMeshCom != nullptr
		&& m_pTransformCom != nullptr
		)
	{	// 루트 모션 사용시, 애니메이션 포지션값 구해와서 트랜스폼에 적용

			m_pMeshCom->Play_Animation(fTimeDelta);
			if (m_pMeshCom->Get_CurAnimationInfo().bRoot_Move)
			{
				m_tInfo.m_vecAniMoveDelta = m_pMeshCom->Get_MoveDelta();
				if (D3DXVec3Length(&m_tInfo.m_vecAniMoveDelta) >= SMALL_NUMBER)
				{
					//CDebug::GetInstance()->Add_ConsoleDebugText(m_tInfo.m_vecAniMoveDelta);
					//D3DXVec3TransformNormal(&m_tInfo.m_vecAniMoveDelta, &m_tInfo.m_vecAniMoveDelta, &m_pTransformCom->Get_WorldMatrix_FORCED());
					D3DXVec3TransformNormal(&m_tInfo.m_vecAniMoveDelta, &m_tInfo.m_vecAniMoveDelta, &m_pTransformCom->Get_WorldMatrix());
					m_pTransformCom->Go_Direction_NotNormalize(m_tInfo.m_vecAniMoveDelta, 1.f, 1.f);
				}

				m_tInfo.m_vecAniAngleDelta = m_pMeshCom->Get_AngleDelta();
				m_tInfo.m_vecAniAngleDelta.x = 0.f;
				if (D3DXVec3Length(&m_tInfo.m_vecAniAngleDelta) >= SMALL_NUMBER)
				{
					//CDebug::GetInstance()->Add_ConsoleDebugText(m_tInfo.m_vecAniAngleDelta);
					m_pTransformCom->Add_Infomation(CTransform::INFO_ANGLE, &m_tInfo.m_vecAniAngleDelta);
				}
			}
	}

	//_int iReturn = CGameObject::Update_GameObject(fTimeDelta);
	

	for (auto pEquip : m_pEquipVector)
	{
		pEquip->Update_GameObject(fTimeDelta);
	}

#ifdef CHARACTER_COLLIDERMODIFY
	if ( true == m_tInfo.m_WorldState.getVariable( CH_STATE_IDLE ) )
	{
		if ( m_ColliderGroups.iGroupCount > m_tInfo.m_iMoveColliderGroupCount )
		{
			for ( _uint i = 0; i < m_ColliderGroups.pGroup[m_tInfo.m_iMoveColliderGroupCount].iColliderCount; ++i )
			{
				if ( m_ColliderGroups.pGroup[m_tInfo.m_iMoveColliderGroupCount].ppCollider[i] )
					m_ColliderGroups.pGroup[m_tInfo.m_iMoveColliderGroupCount].ppCollider[i]->Modify_StaticOption( STATICOPTIONDATA( PushOption_Fixed ), StaticOption_Push );
			}
		}
	}
	else
	{
		if ( m_ColliderGroups.iGroupCount > m_tInfo.m_iMoveColliderGroupCount )
		{
			for ( _uint i = 0; i < m_ColliderGroups.pGroup[m_tInfo.m_iMoveColliderGroupCount].iColliderCount; ++i )
			{
				if ( m_ColliderGroups.pGroup[m_tInfo.m_iMoveColliderGroupCount].ppCollider[i] )
					m_ColliderGroups.pGroup[m_tInfo.m_iMoveColliderGroupCount].ppCollider[i]->Modify_StaticOption( STATICOPTIONDATA( PushOption_BePushedOut ), StaticOption_Push );
			}
		}
	}
#else
	if ( GetAsyncKeyState( 'Z' ) )
	{
		_bool bIsUpdateWorld = m_pTransformCom->Get_IsUpdateWorld();

		if ( true == m_tInfo.m_WorldState.getVariable( CH_STATE_IDLE ) )
		{
			m_pColliderCom->Modify_StaticOption(CCollider::STATICOPTIONDATA(PushOption_Fixed), StaticOption_Push);
			CDebug::GetInstance()->Add_ConsoleDebugText("bIsUpdateWorld = true");
		}
		else
		{
			m_pColliderCom->Modify_StaticOption(CCollider::STATICOPTIONDATA(PushOption_BePushedOut), StaticOption_Push);
			CDebug::GetInstance()->Add_ConsoleDebugText("bIsUpdateWorld = false");
		}

		//if ( bIsUpdateWorld )
		//{
		//	m_pColliderCom->Modify_StaticOption( CCollider::STATICOPTIONDATA( PushOption_Fixed ), StaticOption_Push );
		//	CDebug::GetInstance()->Add_ConsoleDebugText( "bIsUpdateWorld = true" );
		//}
		//else
		//{
		//	m_pColliderCom->Modify_StaticOption( CCollider::STATICOPTIONDATA( PushOption_BePushedOut ), StaticOption_Push );
		//	CDebug::GetInstance()->Add_ConsoleDebugText( "bIsUpdateWorld = false" );
		//}
	}
	else
	{
		m_pColliderCom->Modify_StaticOption(CCollider::STATICOPTIONDATA(PushOption_BePushedOut), StaticOption_Push);
		CDebug::GetInstance()->Add_ConsoleDebugText("bIsUpdateWorld = false");
	}
#endif

	return iReturn;
}

_int CCharacter::LastUpdate_GameObject(const _float & fTimeDelta)
{
	//m_pColliderMgr->Add_Character(this);
	
	_int iReturn = CGameObject::LastUpdate_GameObject(fTimeDelta);

	if (m_pController != nullptr)
	{
		m_pController->LastUpdate_Component(fTimeDelta);
	}
	m_tInfo.m_bHit = false; // false로 초기화

	for (auto pEquip : m_pEquipVector)
		pEquip->LastUpdate_GameObject(fTimeDelta);

	return iReturn;
}

void CCharacter::Render_Shadow( LPD3DXEFFECT pEffect, const char * pConstantWorldMatrixName )
{
	if ( nullptr != m_pTransformCom )
	{
		m_pTransformCom->SetUp_OnShader( pEffect, pConstantWorldMatrixName );
		pEffect->CommitChanges();

		if ( nullptr != m_pMeshCom )
			m_pMeshCom->Render_Mesh();
	}
}

void CCharacter::Reset_WorldState()
{
	CWorldState NewState;

	// ---- ACT ----
	NewState.setVariable(ACT_TYPE_START, ACT_TYPE_END, false);
	// ---- ANI_SLIMTHUG ----
	NewState.setVariable(ANI_SLIMTHUG_TYPE_START, ANI_SLIMTHUG_TYPE_END, false);
	// ---- Signal ----
	NewState.setVariable(SIGNAL_TYPE_START, SIGNAL_TYPE_END, false);
	NewState.setVariable(CH_STATE_TYPE_START, CH_STATE_TYPE_END, false);
	NewState.setVariable(TARGET_TYPE_START, TARGET_TYPE_END, false);
	
	NewState.setVariable(SIGNAL_CHANGE_ANIMATION, true);
	NewState.setVariable(ACT_IDLE, true);
	NewState.setVariable(CH_STATE_PEACE, true);
	NewState.setVariable(CH_STATE_FOUND_ENEMY, false);
	NewState.setVariable(TARGET_NONE, true);

	m_tInfo.m_WorldState = NewState;
}

void CCharacter::Act_AnimationEvent( const ANI_STATUS & _AniStatus, const _uint & _iEventIndex )
{
	if ((_uint)_AniStatus.iArrSize <= _iEventIndex )
		return;

	switch ( _AniStatus.m_AniEvent[_iEventIndex].pEvnetArr )
	{
		case Engine::COLLIDER_ON:
			Event_ColliderActOn( _AniStatus, _iEventIndex );
			break;

		case Engine::COLLIDER_OFF:
			Event_ColliderActOff( _AniStatus, _iEventIndex );
			break;

		case Engine::SOUND_ON:
			
			break;

		case Engine::SOUND_OFF:
			
			break;

		case Engine::EFFECT_ON:
		//{
		//	_matrix* pBone = m_pMeshCom->Find_FrameMatrixPointer(_AniStatus.m_AniEvent[_iEventIndex].effectEventInfo.BoneTag);
		//
		//	switch (_AniStatus.m_AniEvent[_iEventIndex].effectEventInfo.iTransType)
		//	{
		//	case EFFECT_INFO::TRANS_TYPE::TYPE_POS_FIXED:
		//		CEffect_Manager::GetInstance()->Use_Effect(
		//			CManagement::GetInstance()->Get_CurSceneIndex(),
		//			_AniStatus.m_AniEvent[_iEventIndex].effectEventInfo.EffectTag,
		//			*m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION),
		//			this,
		//			pBone
		//		);
		//		break;
		//	case EFFECT_INFO::TRANS_TYPE::TYPE_MATRIX_FIXED:
		//		CEffect_Manager::GetInstance()->Use_Effect(
		//			CManagement::GetInstance()->Get_CurSceneIndex(),
		//			_AniStatus.m_AniEvent[_iEventIndex].effectEventInfo.EffectTag,
		//			m_pTransformCom->Get_WorldMatrix(),
		//			this,
		//			pBone
		//		);
		//		break;
		//	case EFFECT_INFO::TRANS_TYPE::TYPE_POS_UPDATED:
		//	case EFFECT_INFO::TRANS_TYPE::TYPE_MATRIX_UPDATED:
		//	case EFFECT_INFO::TRANS_TYPE::TYPE_POINTER:
		//		CEffect_Manager::GetInstance()->Use_Effect(
		//			CManagement::GetInstance()->Get_CurSceneIndex(),
		//			_AniStatus.m_AniEvent[_iEventIndex].effectEventInfo.EffectTag,
		//			this,
		//			pBone
		//		);
		//		break;
		//	}
		//}
			break;
		case Engine::EFFECT_OFF:
			//CEffect_Manager::GetInstance()->End_Effect(
			//	CManagement::GetInstance()->Get_CurSceneIndex(),
			//	_AniStatus.m_AniEvent[_iEventIndex].effectEventInfo.EffectTag,
			//	this->GetKey()
			//);
			break;

		case Engine::SPEED_CHANGE:
			
			break;

		default:
			break;
	}
}

void CCharacter::OnInteraction( const INTERACTIONINFO& _InteractionInfo, const _uint& _iInteractionIndex )
{
	if ( _iInteractionIndex >= _InteractionInfo.iInteractionCount )
		return;

	const INTERACTIONDATA& InteractionData = _InteractionInfo.pInteractionData[_iInteractionIndex];

	for ( _ubyte i = 0; i < InteractionData.iOptionCount; ++i )
	{
		m_tInfo.m_WorldState.setVariable( InteractionData.pOptionData[i].OptionData.dwOption, true );

		if ( InteractionData.pOptionData[i].OptionData.DetailDataList.empty() == false )
			m_tInfo.m_WorldState.setVariable( InteractionData.pOptionData[i].OptionData.DetailDataList.front(), true );
	}

	// 시작 지점으로 위치 이동..
	_vec3 vDstPos = *_InteractionInfo.pCharacter->Get_Transform()->Get_Infomation( Engine::CTransform::INFO_POSITION );
	_vec3 vDstLook = *_InteractionInfo.pCharacter->Get_Transform()->Get_WorldLookVec_FORCED(); vDstLook.y = 0.f;
	_vec3 vPos = vDstPos + vDstLook * InteractionData.fDist;
	m_pTransformCom->Set_Infomation( Engine::CTransform::INFO_POSITION, &vPos );
}

_bool CCharacter::IsCanInteraction( void )
{
	if ( true == m_tInfo.m_WorldState.getVariable( CH_STATE_HIT ) ||
		 true == m_tInfo.m_WorldState.getVariable( CH_STATE_INTERACTION ) )
		return false;

	return true;
}

HRESULT CCharacter::Ready_Character_Component()
{
	CComponent* pComponent = nullptr;

	// For.Transform
	pComponent = m_pTransformCom = (Engine::CTransform*)CComponent_Manager::GetInstance()->Clone_Component(0, L"CTransform");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_DYNAMIC].insert(MAPCOMPONENT::value_type(L"Com_Transform", pComponent));
	m_pTransformCom->AddRef();

	// For.Renderer
	pComponent = m_pRendererCom = (Engine::CRenderer*)CComponent_Manager::GetInstance()->Clone_Component(0, L"CRenderer");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Renderer", pComponent));
	m_pRendererCom->AddRef();


	return NOERROR;

//exception:
//	MSG_BOX("CCharacter Component Ready Failed");
//	return E_FAIL;
}

HRESULT CCharacter::Ready_LoadCollider( const _tchar * _pFilePath, Engine::CCollider* _pCollder )
{
	DWORD dwByte = 0;

	HANDLE hFile = CreateFile(
		_pFilePath,		//파일 경로.
		GENERIC_READ,	//어떤 모드로? : 쓰기전용
		NULL,		//공유모드
		NULL,
		OPEN_EXISTING,	//파일옵션 : 파일이 있으면 덮어쓰기 -> 없으면 새로 만들라는 인자값.
		FILE_ATTRIBUTE_NORMAL,		//기본값으로 파일을 생성하라는옵션.
		NULL
	);

	if ( hFile == INVALID_HANDLE_VALUE )
		return E_FAIL;

	// 콜리더 최종 개수 받아오기..
	_int iCount = 0;
	ReadFile( hFile, &iCount, sizeof( int ), &dwByte, NULL );

	_tchar szCmp[256] = L"";
	_tchar szColGroupName[256] = L"";
	_tchar szColComName[256] = L"";
	COLLIDERGROUP ColliderGroup;
	Engine::CCollider* pCollider = nullptr;

	for ( int i = 0; i < iCount; ++i )
	{
		COLLIDER_STATUS* pCollider_Status = new COLLIDER_STATUS;

		ReadFile( hFile, pCollider_Status, sizeof( COLLIDER_STATUS ), &dwByte, NULL );

		pCollider = (CCollider*)_pCollder->Clone();

		// 파싱한 데이터를 가지고 콜라이더 옵션 세팅하는 부분..
		CCollider::RESIZEDATA	ResizeData;
		CCollider::TYPE			eType;

		char szTempName[256] = "";
		int len = WideCharToMultiByte( CP_ACP, 0, pCollider_Status->BoneName, -1, NULL, 0, NULL, NULL );
		WideCharToMultiByte( CP_ACP, 0, pCollider_Status->BoneName, -1, szTempName, len, NULL, NULL );

		ResizeData.pTransformCom = m_pTransformCom;
		ResizeData.pParent = m_pMeshCom->Find_FrameMatrixPointer( szTempName );
		ResizeData.vOriginalMin = pCollider_Status->vMin;
		ResizeData.vOriginalMax = pCollider_Status->vMax;
		ResizeData.vOffset = pCollider_Status->vOffset;

		switch ( pCollider_Status->eType )
		{
			case CIRCLE:
				eType = CCollider::TYPE_SPHERE;
				break;
			case AABB:
				eType = CCollider::TYPE_AABB;
				break;
			case OBB:
				eType = CCollider::TYPE_OBB;
				break;
		}

		pCollider->ReSizing( eType, ResizeData );

		// 만약 현재 검사한 콜리더 그룹이 전에 검사했던 그룹과 다른 그룹이라면..
		if ( 0 != lstrcmp( szCmp, pCollider_Status->GroupName ) )
		{
			if ( ColliderGroup.iColliderCount > 0 )	// 콜리더 그룹 저장..
				m_ColliderGroups.Add_ColliderGroup( ColliderGroup );

			// 그룹 정보 다시 세팅..
			ZeroMemory( &ColliderGroup, sizeof( COLLIDERGROUP ) );
			lstrcpy( szCmp, pCollider_Status->GroupName );
			swprintf_s( szColGroupName, L"%s%d", pCollider_Status->GroupName, GetKey() );
			ColliderGroup.pGroupName = CString_Manager::GetInstance()->GetStr( szColGroupName );
		}

		ColliderGroup.Add_Collider( pCollider );

		// 생성한 콜라이더를 Dynamic map 에 보관..
		swprintf_s( szColComName, L"Com_Collider%d", i );
		m_mapComponent[Engine::CComponent::TYPE_DYNAMIC].insert( MAPCOMPONENT::value_type( CString_Manager::GetInstance()->GetStr( szColComName ), pCollider ) );
		pCollider->AddRef();

		Safe_Delete( pCollider_Status );
	}

	m_ColliderGroups.Add_ColliderGroup( ColliderGroup );

	CloseHandle( hFile );

	return NOERROR;
}

HRESULT CCharacter::Ready_CollidersToColliderManager( const _int & _iGroupType, Engine::CGameObject * _pGameObject )
{
	if ( nullptr == m_pCollider_Manager )
		return E_FAIL;

	for ( _uint i = 0; i < m_ColliderGroups.iGroupCount; ++i )
	{
		for ( _uint j = 0; j < m_ColliderGroups.pGroup[i].iColliderCount; ++j )
		{
			if ( FAILED( m_pCollider_Manager->Add_Collider( CCollider_Manager::GROUPKIND( _iGroupType ), m_ColliderGroups.pGroup[i].pGroupName, 
															m_ColliderGroups.pGroup[i].ppCollider[j], _pGameObject ) ) )
				return E_FAIL;
		}
	}

	return NOERROR;
}

_bool CCharacter::Colliders_Add_ColliderLayer( const _int & _iGroupType, const _tchar * _pLayerTag, const _uint & _iGroupIndex )
{
	if ( m_ColliderGroups.iGroupCount <= _iGroupIndex )
		return false;

	// 콜리더들을 레이어에 등록한다..
	if ( FAILED( m_pCollider_Manager->Add_ColliderGroup( CCollider_Manager::GROUPKIND( _iGroupType ), _pLayerTag, m_ColliderGroups.pGroup[_iGroupIndex].pGroupName ) ) )
		return false;

	m_ColliderGroups.pGroup[_iGroupIndex].pLayerName = _pLayerTag;

	return true;
}

_bool CCharacter::Colliders_Exclude_ColliderLayer( const _int & _iGroupType, const _uint & _iGroupIndex )
{
	if ( m_ColliderGroups.iGroupCount <= _iGroupIndex )
		return false;

	// 콜리더 레이어에서 뺀다..
	if ( false == m_pCollider_Manager->Exclude_ColliderGroupInLayer( CCollider_Manager::GROUPKIND( _iGroupType ), m_ColliderGroups.pGroup[_iGroupIndex].pGroupName, m_ColliderGroups.pGroup[_iGroupIndex].pLayerName ) )
		return false;

	return true;
}

HRESULT CCharacter::Event_ColliderActOn( const ANI_STATUS& _AniStatus, const _uint& _iEventIndex )
{
	_uint iIndex = (_uint)(_AniStatus.m_AniEvent[_iEventIndex].fEventSpeed + 0.01f);	// 혹시 모를 소수점 오차로 생각지 않은 값을 가져오는 것 방지..

	if ( m_ColliderGroups.iGroupCount <= iIndex )
		return E_FAIL;

	if ( FAILED( Colliders_Add_ColliderLayer( CCollider_Manager::GROUP_STATIC, m_ColliderGroups.pGroup[iIndex].pLayerName, iIndex ) ) )
		return E_FAIL;

	return NOERROR;
}

HRESULT CCharacter::Event_ColliderActOff( const ANI_STATUS& _AniStatus, const _uint& _iEventIndex )
{
	_uint iIndex = (_uint)(_AniStatus.m_AniEvent[_iEventIndex].fEventSpeed + 0.01f);	// 혹시 모를 소수점 오차로 생각지 않은 값을 가져오는 것 방지..

	if ( m_ColliderGroups.iGroupCount <= iIndex )
		return E_FAIL;

	if ( FAILED( Colliders_Exclude_ColliderLayer( CCollider_Manager::GROUP_STATIC, iIndex ) ) )
		return E_FAIL;

	return NOERROR;
}

void CCharacter::Move_Navigation()
{
	if (nullptr == m_pNaviCom)
		return;

	m_pTransformCom->Move_OnNavigation(m_pNaviCom);
}
void CCharacter::LookTargetLookDir( CCharacter* _pCharacter, const _float & _fOffset )
{
	if ( nullptr == _pCharacter )
		return;

	CTransform* pTransform = _pCharacter->Get_Transform();
	if ( nullptr == pTransform )
		return;

	_vec3 vLook = *pTransform->Get_WorldLookVec_FORCED();

	if ( D3DXVec3Length( &vLook ) <= 0.0001f )
		return;

	vLook.y = 0.f;
	D3DXVec3Normalize( &vLook, &vLook );

	_float fAngle = D3DXToDegree( acosf( D3DXVec3Dot( &_vec3( 0.f, 0.f, 1.f ), &vLook ) ) );
	if ( vLook.x < 0.f )
		fAngle *= -1.f;

	fAngle += _fOffset;

	if ( fAngle < 0.f )
		fAngle += 360.f;
	else if ( fAngle > 360.f )
		fAngle -= 360.f;

	CDebug::GetInstance()->Add_ConsoleDebugText( L"Interaction On" );
	CDebug::GetInstance()->Add_ConsoleDebugText( fAngle );

	m_pTransformCom->Set_Infomation( Engine::CTransform::INFO_ANGLE, &_vec3( 0.f, fAngle, 0.f ) );
}

void CCharacter::Acc_CurrentHp( const _uint & _iAccHp )
{
	m_tInfo.m_iCurHp += _iAccHp;
	if ( m_tInfo.m_iCurHp < 0 )
		m_tInfo.m_iCurHp = 0;
	else if ( m_tInfo.m_iCurHp > m_tInfo.m_iMaxHp )
		m_tInfo.m_iCurHp = m_tInfo.m_iMaxHp;

	m_tInfo.m_fHpAcc = (_float)m_tInfo.m_iCurHp / (_float)m_tInfo.m_iMaxHp;
}

void CCharacter::Acc_CurrentCp( const _float& _fAccCp )
{
	m_tInfo.m_fCurCp += _fAccCp;

	if ( m_tInfo.m_fCurCp < 0.f )
		m_tInfo.m_fCurCp = 0.f;
	else if ( m_tInfo.m_fCurCp > m_tInfo.m_fMaxCp )
		m_tInfo.m_fCurCp = m_tInfo.m_fMaxCp;

	m_tInfo.m_fCpAcc = m_tInfo.m_fCurCp / m_tInfo.m_fMaxCp;
}

#ifndef _FINAL
void CCharacter::Debug_Render(void)
{
	CDebug::GetInstance()->Add_DebugText("CCharacter");
	CDebug::GetInstance()->Add_DebugText("m_tInfo.m_iCurHp: %d", m_tInfo.m_iCurHp);
	CDebug::GetInstance()->Add_DebugText("m_tInfo.m_iMaxHp: %d", m_tInfo.m_iMaxHp);
	CDebug::GetInstance()->Add_DebugText("m_tInfo.m_fHpAcc: %f", m_tInfo.m_fHpAcc);
	if(m_tInfo.m_bHit)
		CDebug::GetInstance()->Add_DebugText("m_tInfo.m_bHit: true");
	else
		CDebug::GetInstance()->Add_DebugText("m_tInfo.m_bHit: false");
	CDebug::GetInstance()->Add_DebugText("m_tInfo.m_MoveDelta: %f %f %f", m_tInfo.m_MoveDelta.x, m_tInfo.m_MoveDelta.y, m_tInfo.m_MoveDelta.z);
	CDebug::GetInstance()->Add_DebugText("m_tInfo.m_fSpeed: %f", m_tInfo.m_fSpeed);
	CDebug::GetInstance()->Add_DebugText("m_tInfo.m_eCharType: %d", m_tInfo.m_eCharType);
	CDebug::GetInstance()->Add_DebugText("-- RootMotion --", m_tInfo.m_eCharType);
	CDebug::GetInstance()->Add_DebugText("m_tInfo.m_vecAniMoveDelta: %f %f %f", m_tInfo.m_vecAniMoveDelta.x, m_tInfo.m_vecAniMoveDelta.y, m_tInfo.m_vecAniMoveDelta.z);
	CDebug::GetInstance()->Add_DebugText("m_tInfo.m_vecAniAngleDelta: %f %f %f", m_tInfo.m_vecAniAngleDelta.x, m_tInfo.m_vecAniAngleDelta.y, m_tInfo.m_vecAniAngleDelta.z);
	CDebug::GetInstance()->Add_DebugText("----------- WORLD STATE -----------");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(ACT_TYPE_START), "ACT_TYPE_START");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(ACT_END), "ACT_END");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(ACT_IDLE), "ACT_IDLE");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(ACT_WALK), "ACT_WALK");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(ACT_RUN), "ACT_RUN");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(ACT_SPRINT), "ACT_SPRINT");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(ACT_ATTACK), "ACT_ATTACK");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(ACT_KICK), "ACT_KICK");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(ACT_GUARD), "ACT_GUARD");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(ACT_HIT), "ACT_HIT");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(ACT_KNOCKBACK), "ACT_KNOCKBACK");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(ACT_STEALTH), "ACT_STEALTH");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(ACT_SIDESTEP), "ACT_SIDESTEP");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(ACT_INTERACTION), "ACT_INTERACTION");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(ACT_DEATH), "ACT_DEATH");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(ACT_TYPE_END), "ACT_TYPE_END");

	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(CH_STATE_TYPE_START), "CH_STATE_TYPE_START");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(CH_STATE_FOUND_ENEMY), "CH_STATE_FOUND_ENEMY");

	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(CH_STATE_CLOSE_PLAYER), "CH_STATE_CLOSE_PLAYER");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(CH_STATE_NEAR_PLAYER), "CH_STATE_NEAR_PLAYER");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(CH_STATE_MID_PLAYER), "CH_STATE_MID_PLAYER");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(CH_STATE_FAR_PLAYER), "CH_STATE_FAR_PLAYER");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(CH_STATE_TOOFAR_PLAYER), "CH_STATE_TOOFAR_PLAYER");

	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(CH_STATE_CLOSE_NEAR_ENEMY), "CH_STATE_CLOSE_NEAR_ENEMY");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(CH_STATE_NEAR_NEAR_ENEMY), "CH_STATE_NEAR_NEAR_ENEMY");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(CH_STATE_MID_NEAR_ENEMY), "CH_STATE_MID_NEAR_ENEMY");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(CH_STATE_FAR_NEAR_ENEMY), "CH_STATE_FAR_NEAR_ENEMY");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(CH_STATE_TOOFAR_NEAR_ENEMY), "CH_STATE_TOOFAR_NEAR_ENEMY");

	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(CH_STATE_CLOSE_TARGET), "CH_STATE_CLOSE_TARGET");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(CH_STATE_NEAR_TARGET), "CH_STATE_NEAR_TARGET");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(CH_STATE_MID_TARGET), "CH_STATE_MID_TARGET");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(CH_STATE_FAR_TARGET), "CH_STATE_FAR_TARGET");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(CH_STATE_TOOFAR_TARGET), "CH_STATE_TOOFAR_TARGET");

	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(CH_STATE_TYPE_BY_ANI_START), "CH_STATE_TYPE_BY_ANI_START");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(CH_STATE_RUN), "CH_STATE_RUN");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(CH_STATE_IDLE), "CH_STATE_IDLE");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(CH_STATE_PEACE), "CH_STATE_PEACE");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(CH_STATE_BATTLE), "CH_STATE_BATTLE");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(CH_STATE_SPRINT), "CH_STATE_SPRINT");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(CH_STATE_SIDESTEP), "CH_STATE_SIDESTEP");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(CH_STATE_ATTACK), "CH_STATE_ATTACK");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(CH_STATE_HIT), "CH_STATE_HIT");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(CH_STATE_TYPE_BY_ANI_END), "CH_STATE_TYPE_BY_ANI_END");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(CH_STATE_TYPE_END), "CH_STATE_TYPE_END");

	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(TARGET_TYPE_START), "TARGET_TYPE_START");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(TARGET_NEAR_ENEMY), "TARGET_NEAR_ENEMY");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(TARGET_PLAYER), "TARGET_PLAYER");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(TARGET_TYPE_END), "TARGET_TYPE_END");

	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(SIGNAL_TYPE_START), "SIGNAL_TYPE_START");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(SIGNAL_CHANGE_ANGLE), "SIGNAL_CHANGE_ANGLE");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(SIGNAL_CHANGE_ANGLE_FORCED), "SIGNAL_CHANGE_ANGLE_FORCED");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(SIGNAL_SET_ANGLE_TARGET), "SIGNAL_CHANGE_ANGLE_TARGET");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(SIGNAL_SET_ANGLE_CAMERA), "SIGNAL_CHANGE_ANGLE_CAMERA");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(SIGNAL_CHANGE_ANIMATION), "SIGNAL_CHANGE_ANIMATION");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(SIGNAL_CHANGE_ANIMATION_FORCED), "SIGNAL_CHANGE_ANIMATION_FORCED");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(SIGNAL_ACT_INTERACTION), "SIGNAL_ACT_INTERACTION");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(SIGNAL_TYPE_END), "SIGNAL_TYPE_END");

	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(SIGNAL_INTERACTION_COUNTER_FRONT1), "SIGNAL_INTERACTION_COUNTER_FRONT1");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(SIGNAL_INTERACTION_COUNTER_BACK1), "SIGNAL_INTERACTION_COUNTER_BACK1");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(SIGNAL_INTERACTION_COUNTER_DONFRONT1), "SIGNAL_INTERACTION_COUNTER_DONFRONT1");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(SIGNAL_INTERACTION_COUNTER_DONBACK1), "SIGNAL_INTERACTION_COUNTER_DONBACK1");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(SIGNAL_INTERACTION_COUNTER_LEOFRONT1), "SIGNAL_INTERACTION_COUNTER_LEOFRONT1");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(SIGNAL_INTERACTION_COUNTER_LEOBACK1), "SIGNAL_INTERACTION_COUNTER_LEOBACK1");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(SIGNAL_INTERACTION_COUNTER_MIKEFRONT1), "SIGNAL_INTERACTION_COUNTER_MIKEFRONT1");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(SIGNAL_INTERACTION_COUNTER_MIKEBACK1), "SIGNAL_INTERACTION_COUNTER_MIKEBACK1");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(SIGNAL_INTERACTION_COUNTER_RAPHFRONT1), "SIGNAL_INTERACTION_COUNTER_RAPHFRONT1");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(SIGNAL_INTERACTION_COUNTER_RAPHBACK1), "SIGNAL_INTERACTION_COUNTER_RAPHBACK1");

	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(MOVE_DIR_RIGHT), "MOVE_DIR_RIGHT");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(MOVE_DIR_BACK), "MOVE_DIR_BACK");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(MOVE_DIR_FRONT), "MOVE_DIR_FRONT");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(MOVE_DIR_LEFT), "MOVE_DIR_LEFT");

	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(DEST_DIR_0), "DEST_DIR_0");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(DEST_DIR_45), "DEST_DIR_45");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(DEST_DIR_90), "DEST_DIR_90");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(DEST_DIR_135), "DEST_DIR_135");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(DEST_DIR_180), "DEST_DIR_180");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(DEST_DIR_225), "DEST_DIR_225");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(DEST_DIR_270), "DEST_DIR_270");
	CDebug::GetInstance()->Add_DebugText(0, m_tInfo.m_WorldState.getVariable(DEST_DIR_315), "DEST_DIR_315");
	CDebug::GetInstance()->Add_DebugText(m_tInfo.m_fDestAngle, "m_fDestAngle");
	
	
}
#endif

void CCharacter::Set_Controller(CController * pController)
{
	// 이전 컨트롤러 정리
	if (m_pController != nullptr)
	{
		Safe_Release(m_pController);
		for (size_t i = 0; i < Engine::CComponent::TYPE_END; ++i)
		{
			auto& iter = find_if(m_mapComponent[i].begin(), m_mapComponent[i].end(), CFind_Tag(L"Controller"));
			if (iter != m_mapComponent[i].end())
			{
				m_mapComponent[i].erase(iter);
				break;
			}
		}
	}

	// 컨트롤러 교체
	m_pController = pController;
	if (m_pController != nullptr)
	{
		m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Controller", m_pController));
		m_pController->AddRef();
	}
};

HRESULT CCharacter::Set_Camera(CCamera * pCamera)
{
	if (nullptr == pCamera)
		return E_FAIL;

	if (nullptr != m_pCamera)
		Safe_Release(m_pCamera);

	m_pCamera = pCamera;
	m_pCamera->AddRef();

	return NOERROR;
}

HRESULT CCharacter::Equip_Item(CEquipment * pItem, _matrix * pBoneMatrix)
{
	if (pItem == nullptr
		|| m_pTransformCom == nullptr)
		return E_FAIL;

	pItem->Set_Bone(pBoneMatrix);
	pItem->Set_User(this);
	m_pEquipVector.push_back(pItem);

	++m_iItemCount;

	return NOERROR;
}

void CCharacter::Hide_Item(bool bHide, int iIndex)
{
	if (iIndex == -1)
	{
		for (auto pItem : m_pEquipVector)
		{
			pItem->Hide_Item(bHide);
		}
	}
	else if(m_iItemCount < iIndex && iIndex>= 0)
	{
		m_pEquipVector[iIndex]->Hide_Item(bHide);
	}
	
}

EStatus CCharacter::Move_Forward(float fScale)
{
	if (nullptr == m_pTransformCom)
		return EStatus::Failure;

	//if (true == m_bNavi)
	//{
	//	_vec3	vDirection;
	//	_vec3 vNextPos = *m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION) +
	//		*D3DXVec3Normalize(&vDirection, m_pTransformCom->Get_WorldLookVec_FORCED()) * m_tInfo.m_fSpeed * fScale * m_pTimer->Get_TimeDelta();
	//	_vec3 vPushPower = _vec3(0.f, 0.f, 0.f);
	//
	//	if (m_pNaviCom->Check_Navigation(&vPushPower, &vNextPos))
	//	{
	//		m_pTransformCom->Go_Straight(
	//			m_tInfo.m_fSpeed * fScale,
	//			m_pTimer->Get_TimeDelta());
	//	}
	//	else
	//	{
	//		m_pTransformCom->Go_Straight(
	//			-m_tInfo.m_fSpeed * fScale,
	//			m_pTimer->Get_TimeDelta());
	//	}
	//}
	//else
	//{
	//	m_pTransformCom->Go_Straight(
	//		m_tInfo.m_fSpeed * fScale,
	//		m_pTimer->Get_TimeDelta());
	//}

	return EStatus::Success;
}

EStatus CCharacter::Move_Right(float fScale)
{
	if (nullptr == m_pTransformCom)
		return EStatus::Failure;

	//if (true == m_bNavi)
	//{
	//	_vec3	vDirection;
	//	_vec3 vNextPos = *m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION) +
	//		*D3DXVec3Normalize(&vDirection, m_pTransformCom->Get_WorldRightVec_FORCED()) * m_tInfo.m_fSpeed * fScale * m_pTimer->Get_TimeDelta();
	//	_vec3 vPushPower = _vec3(0.f, 0.f, 0.f);
	//
	//	if (m_pNaviCom->Check_Navigation(&vPushPower, &vNextPos))
	//	{
	//		m_pTransformCom->Go_Right(
	//			m_tInfo.m_fSpeed * fScale,
	//			m_pTimer->Get_TimeDelta());
	//	}
	//	else
	//	{
	//		m_pTransformCom->Go_Right(
	//			-m_tInfo.m_fSpeed * fScale,
	//			m_pTimer->Get_TimeDelta());
	//	}
	//}
	//else
	//{
	//	m_pTransformCom->Go_Right(
	//		m_tInfo.m_fSpeed * fScale,
	//		m_pTimer->Get_TimeDelta());
	//}


	return EStatus::Success;
}

_ulong CCharacter::Free()
{
	Engine::Safe_Release( m_pNaviCom );//Jung
	//Engine::Safe_Release(m_pTransformCom);//Jung
	Engine::Safe_Release( m_pRendererCom );//Jung
	Engine::Safe_Release( m_pMeshCom );//Jung
	Engine::Safe_Release( m_pShaderCom );//Jung
#ifdef CHARACTER_COLLIDERMODIFY	// Add By.KMH - 콜리더 그룹들을 삭제..
	// Add By.KMH - 콜리더 매니저에서 제거 시킴..
	for ( _uint i = 0; i < m_ColliderGroups.iGroupCount; ++i )
	{
		for ( _uint j = 0; j < m_ColliderGroups.pGroup[i].iColliderCount; ++j )
		{
			m_pCollider_Manager->Delete_Collider( m_ColliderGroups.pGroup[i].pGroupName, m_ColliderGroups.pGroup[i].ppCollider[j] );
		}

		Colliders_Exclude_ColliderLayer( Engine::CCollider_Manager::GROUP_STATIC, i );
	}

	m_ColliderGroups.Release();
#else
	Engine::Safe_Release(m_pColliderCom);//Jung
#endif
	Engine::Safe_Release(m_pController);//Jung
	Engine::Safe_Release(m_pCamera);//Jung

	Engine::Safe_Release( m_pCollider_Manager );	// Add By.KMH

	for (auto pEquip : m_pEquipVector)
		Safe_Release(pEquip);

	m_pEquipVector.clear();

	return  Engine::CGameObject::Free();
}
