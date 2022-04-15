#include "stdafx.h"
#include "..\Headers\Player.h"

#include "Object_Manager.h"
#include "Component_Manager.h"
#include "Clinet_GameObject.h"
#include "Navigation.h"
#include "Graphic_Device.h"

#include "Camera_Player.h"
#include "Clinet_Component.h"
#include "Camera_Manager.h"
#include "Subject_Manager.h"
#include "Collider_Manager.h"

CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphicDev)
	: CCharacter(pGraphicDev)
	//, m_pTargetBufferCom(nullptr)
	//, m_pPickingCom(nullptr)
	//, m_pRSword(nullptr)
	//, m_pLSword(nullptr)
	//, m_pRHandMatrix(nullptr)
	//, m_pLHandMatrix(nullptr)
{
	m_tInfo.m_eCharType = CHARINFO::CHAR_PLAYER;
#ifndef _FINAL
	Tag(L"CPlayer", std::bind(&CPlayer::Debug_Render, this));
#endif
}
//
//HRESULT CPlayer::Set_Camera(CCamera_Player * pCamera)
//{
//	if (nullptr == pCamera)
//		return E_FAIL;
//
//	if (nullptr != m_pPlayerCamera)
//		Safe_Release(m_pPlayerCamera);
//
//	m_pPlayerCamera = pCamera;
//	m_pPlayerCamera->AddRef();
//
//	return NOERROR;
//}

HRESULT CPlayer::Ready_GameObject()
{
	if (FAILED(CCharacter::Ready_Character_Component()))
		return E_FAIL;

	switch (m_eType)
	{
	case PLAYER_TYPE::PLAYER_Donatello:
		if (FAILED(Ready_Donatello()))
			return E_FAIL;
		break;
	case PLAYER_TYPE::PLAYER_Leonardo:
		if (FAILED(Ready_Leonardo()))
			return E_FAIL;
		break;
	case PLAYER_TYPE::PLAYER_Michelangelo:
		if (FAILED(Ready_Michelangelo()))
			return E_FAIL;
		break;
	case PLAYER_TYPE::PLAYER_Raphael:
		if (FAILED(Ready_Raphael()))
			return E_FAIL;
		break;
	default:
		return E_FAIL;
	}

	if (FAILED(Ready_Component()))
		return E_FAIL;

	if (FAILED(Ready_Camera()))
		return E_FAIL;

	if (FAILED(Ready_Controller()))
		return E_FAIL;
	
	m_tInfo.m_fSpeed = 5.f;			// 이동속도
	m_tInfo.m_fAngleSpeed = 5.f;	// 회전속도

	m_pTransformCom->Set_Infomation(Engine::CTransform::INFO_POSITION, &_vec3(100.f+ CRandom::GetInstance()->GetValueF(5.f) , 0.0f, 100.f + CRandom::GetInstance()->GetValueF(5.f)));
	m_pTransformCom->Set_Infomation(Engine::CTransform::INFO_SCALE, &_vec3(0.01f, 0.01f, 0.01f));

	m_pMeshCom->Set_AnimationSet(63);
	m_pMeshCom->Play_Animation(0.f);

	// UI로 정보 넘긴다.
	CSubject_Manager::GetInstance()->AddData(MESSAGE_CURRENT_PLAYER_DATA, CSubject_Manager::SUBJECTTYPE::TYPE_STATIC, this);
	CSubject_Manager::GetInstance()->Notify(MESSAGE_CURRENT_PLAYER_DATA, CSubject_Manager::SUBJECTTYPE::TYPE_STATIC, this);
	CSubject_Manager::GetInstance()->AddData(MESSAGE_PLAYER_DATA, CSubject_Manager::SUBJECTTYPE::TYPE_STATIC, this);
	CSubject_Manager::GetInstance()->Notify(MESSAGE_PLAYER_DATA, CSubject_Manager::SUBJECTTYPE::TYPE_STATIC, this);
	CSubject_Manager::GetInstance()->AddData(MESSAGE_STATE_DATA, CSubject_Manager::SUBJECTTYPE::TYPE_STATIC, &m_tInfo.m_WorldState);
	CSubject_Manager::GetInstance()->Notify(MESSAGE_STATE_DATA, CSubject_Manager::SUBJECTTYPE::TYPE_STATIC, &m_tInfo.m_WorldState);
	CSubject_Manager::GetInstance()->AddData(MESSAGE_CURRENT_PLAYER_DATA,CSubject_Manager::TYPE_STATIC, this);

	return NOERROR;
}

_int CPlayer::Update_GameObject(const _float & fTimeDelta)
{
	CCharacter::Update_GameObject(fTimeDelta);
	


	//if (m_tInfo.m_WorldState.getVariable(CH_STATE_DIE))
	//{
	//	int a = 0;
	//}
	//if (m_bIsDead)
	//{
	//	int a = 0;
	//	m_bIsDead = true;
	//}

	CSubject_Manager::GetInstance()->Notify(MESSAGE_PLAYER_DATA, CSubject_Manager::SUBJECTTYPE::TYPE_STATIC, this);

	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_NONALPHA, this);
		m_pRendererCom->Add_ShadowGroup( this, *m_pTransformCom->Get_Infomation( Engine::CTransform::INFO_POSITION ), 3.f );
	}

	return 0;
}

_int CPlayer::LastUpdate_GameObject(const _float & fTimeDelta)
{
	CCharacter::LastUpdate_GameObject(fTimeDelta);

	if ( m_eType == PLAYER_Donatello )
	{
		if ( m_tInfo.m_WorldState.getVariable( ACT_ATTACK ) == true )
		{
			for ( _uint i = 0; i < m_ColliderGroups.pGroup[DonColGroup_RWeapon].iColliderCount; ++i )
			{
				OPTIONDATA OptionData;
				OptionData.dwOption = SIGNAL_HIT_ENEMY;
				OptionData.DetailDataList.push_back( 0 );
				m_ColliderGroups.pGroup[DonColGroup_RWeapon].ppCollider[i]->Add_Option( OptionData );
			}
		}
	}

	return _int();
}

void CPlayer::Render_GameObject(void)
{
	if (nullptr == m_pMeshCom)
		return;		

	if (nullptr == m_pGraphicDev || nullptr == m_pShaderCom)
		return;

#ifndef _FINAL
#ifdef CHARACTER_COLLIDERMODIFY	// Add By.KMH - 모든 콜라이더들 렌더 하는 코드 추가..
	for ( _uint i = 0; i < m_ColliderGroups.iGroupCount; ++i )
	{
		for ( _uint j = 0; j < m_ColliderGroups.pGroup[i].iColliderCount; ++j )
		{
			m_ColliderGroups.pGroup[i].ppCollider[j]->Render_Buffer();
		}
	}
#else
	m_pColliderCom->Render_Buffer();
#endif
#endif 

	LPD3DXEFFECT	pEffect = m_pShaderCom->Get_EffectHandle();
	if (nullptr == pEffect)
		return;
	pEffect->AddRef();

	//m_pRendererCom->SetUp_ShadowDataOnShader( pEffect, "g_matLightVP", "g_matLightVPB" );
	SetUp_OnConstantTable(pEffect);

	pEffect->Begin(nullptr, 0);

	pEffect->BeginPass(0);

	m_pMeshCom->Render_Mesh(pEffect, "g_BaseTexture", "g_NormalTexture", "g_SpecularTexture");

	pEffect->EndPass();

	pEffect->End();

	Safe_Release(pEffect);

	m_pNaviCom->Render_Navigation();

	
}

void CPlayer::OnCollision( const Engine::Collision& _Collision )
{
	switch ( _Collision.CollisionType )
	{
		case Engine::COLLISION_ENTER:	// 현재 충돌한 콜라이더와 이전 프레임에 충돌을 한 적이 없음..개꿀..
		{
			list<Engine::OPTIONDATA>	OptionList;
			if ( false == _Collision.pOtherCollider->Get_OptionList( OptionList ) )
				return;

			for ( auto& Option : OptionList )
			{
				if ( Option.dwOption == SIGNAL_HIT_ENEMY )
				{
					//if ( true == m_tInfo.m_WorldState.getVariable( CH_STATE_GUARD ) )	// 내가 지금 카운터 상태라면..
					if( true )
					{
						// ----------------------------- 카운터 관련 검사.. ----------------------------- //
						auto& Iter = Option.DetailDataList.begin();
						auto& Iter_End = Option.DetailDataList.end();

						_uint iInteractionIndex = 0;

						// 먼저 무슨 공격 타입인지 검사( 넘어지는 공격, 날라가는 공격, 일반 공격 등등 )..
						if ( Iter_End != Iter )
						{
							_ulong dwAttackType = *Iter;

							if ( dwAttackType == SIGNAL_STRONG_ATK ||
								 dwAttackType == SIGNAL_DOWN_ATK )	// 카운터 모드일 때 강 공격에 맞는다면 회피..
							{

							}
							else		// 일반 공격인 경우.. 
							{
								// 충돌된 상대와의 좌표를 가지고 Hit 애니메이션 인덱스를 구한다..
								_float fAngle = ComputeAngle_HitTargetTransform( _Collision );

								m_tInfo.m_WorldState.setVariable( SIGNAL_INTERACTION_START, SIGNAL_INTERACTION_END, false );

								if ( fAngle <= 180.f )	// 적이 내 앞에 있다..
								{
									m_tInfo.m_WorldState.setVariable( SIGNAL_INTERACTION_COUNTER_FRONT1, true );
									iInteractionIndex = 0;
								}
								else 	// 적이 내 뒤에 있다..
								{
									m_tInfo.m_WorldState.setVariable( SIGNAL_INTERACTION_COUNTER_BACK1, true );
									iInteractionIndex = 1;
								}
							}

							CGameObject* pGameObject = _Collision.pOtherCollider->Get_GameObject();
							if ( nullptr != pGameObject )
							{
								m_InteractionInfo.pCharacter = this;
								m_InteractionInfo.vPos = *m_pTransformCom->Get_Infomation( Engine::CTransform::INFO_POSITION );
								dynamic_cast<CCharacter*>(pGameObject)->OnInteraction( m_InteractionInfo, iInteractionIndex );
							}

							++Iter;
						}

						// 현재 카운터 공격을 하겠다는 신호를 on 으로..
						m_tInfo.m_WorldState.setVariable( SIGNAL_ACT_INTERACTION, true );
					}
					else
					{
						// ----------------------------- 히트 관련 검사.. ----------------------------- //
						auto& Iter = Option.DetailDataList.begin();
						auto& Iter_End = Option.DetailDataList.end();

						// 먼저 무슨 공격 타입인지 검사( 넘어지는 공격, 날라가는 공격, 일반 공격 등등 )..
						if ( Iter_End != Iter )
						{
							_ulong dwAttackType = *Iter;

							if ( dwAttackType == SIGNAL_STRONG_ATK )	// 날라가야 하는 모션인 경우( 쌘 공격 )..
							{
								m_tInfo.m_WorldState.setVariable( SIGNAL_STRONG_ATK, true );
							}
							else if ( dwAttackType == SIGNAL_DOWN_ATK )	// 넘어져야 하는 모션인 경우( 좀 쌘 공격??? ).. 아프다..
							{
								m_tInfo.m_WorldState.setVariable( SIGNAL_DOWN_ATK, true );
							}
							else		// 일반 공격인 경우..
							{
								// 충돌된 상대와의 좌표를 가지고 Hit 애니메이션 인덱스를 구한다..
								_uint iIndex = Compute_HitAniIndex( _Collision );

								if ( iIndex <= HitIndex_End )
									m_tInfo.m_WorldState.setVariable( m_iHitAniArr[iIndex], true );
							}

							++Iter;
						}

						// 데미지를 가져온다..
						if ( Iter_End != Iter )
						{
							_ulong dwDamage = *Iter;
						}

						m_tInfo.m_WorldState.setVariable( Option.dwOption, true );	// 맞았다는 신호를 보낸다..
					}

					// 나를 때린 적을 멤버에 보관한다..
					m_tInfo.m_pEnemy_Attacked = dynamic_cast<CCharacter*>(_Collision.pOtherCollider->Get_GameObject());
				}
			}
		}
		break;
		case Engine::COLLISION_STAY:	// 현재 충돌한 콜라이더와 이전 프레임에 충돌을 했음..
		{
		}
		break;
		default:
		{
			MSG_BOX( "잘못된 충돌 타입" );
		}
		return;
	}
}

HRESULT CPlayer::Ready_Component(void)
{
	Engine::CComponent*			pComponent = nullptr;
		

	// For.Navigation
	pComponent = m_pNaviCom = (Engine::CNavigation*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"CNavigation");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Navigation", pComponent));
	m_pNaviCom->AddRef();
	m_pNaviCom->Set_Currrent_Index(0);

	// For.Shader
	pComponent = m_pShaderCom = (Engine::CShader*)CComponent_Manager::GetInstance()->Clone_Component(0, L"CShader_Mesh");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Shader", pComponent));
	m_pShaderCom->AddRef();

	// For.Collider
#ifndef CHARACTER_COLLIDERMODIFY
	pComponent = m_pColliderCom = (Engine::CCollider*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"CCollider");
	if (nullptr == pComponent)
		return E_FAIL;
#endif

#ifndef CHARACTER_COLLIDERMODIFY
	CCollider::RESIZEDATA Data;
	
	//Data.fOriginalRadius = 70.f;
	Data.pTransformCom = m_pTransformCom;
	Data.pParent = m_pMeshCom->Find_FrameMatrixPointer("spine01");
	Data.vOriginalMin = _vec3(-70.f, -70.f, -70.f);
	Data.vOriginalMax = _vec3(70.f, 70.f, 70.f);

	m_pColliderCom->ReSizing(CCollider::TYPE_SPHERE, Data);
	m_mapComponent[Engine::CComponent::TYPE_DYNAMIC].insert(MAPCOMPONENT::value_type(L"Com_Collider", pComponent));
	m_pColliderCom->AddRef();
#endif


	return NOERROR;
}

HRESULT CPlayer::Ready_Donatello(void)
{
	Engine::CComponent*			pComponent = nullptr;

	// For.Mesh
	pComponent = m_pMeshCom = (Engine::CMesh_Dynamic*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC,L"CMesh_Dynamic_Donatello");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Mesh", pComponent));
	m_pMeshCom->AddRef();
	m_pMeshCom->Set_RootMotionFrame("Origin");
	lstrcpy(m_tInfo.m_StartAniName, L"ANI_DON_TYPE_START");
	m_tInfo.m_StartAniNum = ANI_DON_TYPE_START;
	m_tInfo.m_EndAniNum = ANI_DON_TYPE_END;

	
	if (FAILED(Equip_Item(CStaff::Create(m_pGraphicDev), m_pMeshCom->Find_FrameMatrixPointer("R_Staff"))))
		return E_FAIL;

	// For.Collider - Add By.KMH
	Engine::CCollider* pCollider = (Engine::CCollider*)CComponent_Manager::GetInstance()->Clone_Component( SCENE_STAGE, L"CCollider" );
	if ( nullptr == pCollider )
		return E_FAIL;
	Ready_LoadCollider( L"../Bin/Data/ColliderData/Don_Collider.Colldat", pCollider );
	Engine::Safe_Release( pCollider );

	// -------------------------------- Hit 애니메이션을 차례대로 넣어준다.. -------------------------------- //
	{
		m_iHitAniArr[HitIndex_Front] = ANI_DON_Don_Dmg_F;
		m_iHitAniArr[HitIndex_Right] = ANI_DON_Don_Dmg_R;
		m_iHitAniArr[HitIndex_Back] = ANI_DON_Don_Dmg_B;
		m_iHitAniArr[HitIndex_Left] = ANI_DON_Don_Dmg_L;
	}

	// -------------------------------- 상호 작용 관련 정보 로드.. -------------------------------- //
	if ( FAILED( Ready_LoadInteractionData( L"../Bin/Data/InteractionData/Interaction_Donatello.dat" ) ) )
		return E_FAIL;

	// -------------------------------- 캐릭터의 정보를 세팅.. -------------------------------- //
	{
		m_tInfo.m_fRange_Close = 1.5f;
		m_tInfo.m_fRange_Near = 3.f;
		m_tInfo.m_fRange_Mid = 5.f;
		m_tInfo.m_fRange_Far = 15.f;
		m_tInfo.m_fRange_TooFar = 30.f;
		m_tInfo.m_fRange_FoundEnemy = 40.f;
	}

	// -------------------------------- 캐릭터의 상호작용 정보를 세팅.. -------------------------------- //
	{
		
	}

	/////////////////////////////// 콜리더 그룹 별로 매니저에 등록.. ///////////////////////////////
	{
		// 콜리더를 등록할 때는 공격 관련 콜리더는 플레이어, 몬스터 별로 같은 레이어에 등록해야되요..
		// 충돌 or 기타 등등의 콜리더는 생성되는 객체 당 하나 씩..

		/////////////////////////////// 콜리더 그룹 별로 매니저에 등록.. ///////////////////////////////
		// 콜리더를 등록할 때는 공격 관련 콜리더는 플레이어, 몬스터 별로 같은 레이어에 등록해야되요..
		// 충돌 or 기타 등등의 콜리더는 생성되는 객체 당 하나 씩..

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
		m_tInfo.m_iMoveColliderGroupCount = DonColGroup_Hit;	// Hit 용 콜리더를 캐릭터의 연산에 따라 갱신될 수 있도록 몇 번째 그룹인지 설정..
	}
	/////////////////////////////// 콜리더 그룹 별로 매니저에 등록 끝.. ///////////////////////////////
	

	// %%%%%%%%%%%%%%%%%%%%%%%%%% 캐릭터 월드 스테이트 변경 %%%%%%%%%%%%%%%%%%%%%%%%%%
	// ---- ACT ----
	m_tInfo.m_WorldState.setVariable(ACT_TYPE_START, ACT_TYPE_END, false);
	// ---- ANI_DON ----
	m_tInfo.m_WorldState.setVariable(ANI_DON_TYPE_START, ANI_DON_TYPE_END, false);
	// ---- Signal ----
	m_tInfo.m_WorldState.setVariable(SIGNAL_TYPE_START, SIGNAL_TYPE_END, false);
	m_tInfo.m_WorldState.setVariable(CH_STATE_TYPE_START, CH_STATE_TYPE_END, false);
	m_tInfo.m_WorldState.setVariable(TARGET_TYPE_START, TARGET_TYPE_END, false);

	m_tInfo.m_WorldState.setVariable(SIGNAL_CHANGE_ANIMATION, true);
	m_tInfo.m_WorldState.setVariable(ACT_IDLE, true);
	m_tInfo.m_WorldState.setVariable(CH_STATE_PEACE, true);
	m_tInfo.m_WorldState.setVariable(CH_STATE_FOUND_ENEMY, false);
	m_tInfo.m_WorldState.setVariable(SIGNAL_CHANGE_TARGET_PLAYER, true);


	// %%%%%%%%%%%%%%%%%%%%%%%%%% 시그널 처리 액션 시작 %%%%%%%%%%%%%%%%%%%%%%%%%%
	// $$$$$$$$ 시그널 처리 액션은 시작 조건에 바꿀 조건의 반대조건을 무조건 넣기 $$$$$$$
	// $$$$$$$$ (AI_SIGNAL_SUCCESS,true) 무조건 넣기 $$$$$$$

	// ----- 적 발견 -----
	CAction AI_SIGNAL_FOUND_ENEMY("SIGNAL_FOUND_ENEMY", 10);
	AI_SIGNAL_FOUND_ENEMY.setPrecondition(CH_STATE_FOUND_ENEMY, false); // 바꿀 조건의 반대조건
	AI_SIGNAL_FOUND_ENEMY.setPrecondition(CH_STATE_NEAR_NEAR_ENEMY, true);	//

	AI_SIGNAL_FOUND_ENEMY.setEffect(CH_STATE_FOUND_ENEMY, true);	// 바꿀 조건
	AI_SIGNAL_FOUND_ENEMY.setEffect(AI_SIGNAL_SUCCESS, true);
	m_tInfo.m_vecAction_Ani.push_back(AI_SIGNAL_FOUND_ENEMY);
	// ----- 적 발견 -----

	// ----- 플레이어 접근 -----
	CAction AI_SIGNAL_NEAR_TO_MID_RUN_PLAYER("SIGNAL_NEAR_TO_MID_RUN_PLAYER", 10);
	AI_SIGNAL_NEAR_TO_MID_RUN_PLAYER.setPrecondition(TRIGER_MID_TO_NEAR_PLAYER_RUN, false); // 바꿀 조건의 반대조건
	AI_SIGNAL_NEAR_TO_MID_RUN_PLAYER.setPrecondition(CH_STATE_NEAR_PLAYER, true);	//
	AI_SIGNAL_NEAR_TO_MID_RUN_PLAYER.setPrecondition(CH_STATE_CLOSE_PLAYER, false);	//

	AI_SIGNAL_NEAR_TO_MID_RUN_PLAYER.setEffect(TRIGER_MID_TO_NEAR_PLAYER_RUN, true);	// 바꿀 조건
	AI_SIGNAL_NEAR_TO_MID_RUN_PLAYER.setEffect(AI_SIGNAL_SUCCESS, true);
	m_tInfo.m_vecAction_Ani.push_back(AI_SIGNAL_NEAR_TO_MID_RUN_PLAYER);
	// -----플레이어 접근 -----

	// ----- 플레이어 접근 -----
	CAction AI_SIGNALRUN_STOP_PLAYER("SIGNAL_NEAR_TO_MID_RUN_PLAYER", 9); 
	AI_SIGNALRUN_STOP_PLAYER.setPrecondition(TRIGER_MID_TO_NEAR_PLAYER_RUN, true); // 바꿀 조건의 반대조건
	AI_SIGNALRUN_STOP_PLAYER.setPrecondition(CH_STATE_MID_PLAYER, false);	//
	//AI_SIGNALRUN_STOP_PLAYER.setPrecondition(CH_STATE_N_PLAYER, true);	//

	AI_SIGNALRUN_STOP_PLAYER.setEffect(TRIGER_MID_TO_NEAR_PLAYER_RUN, false);	// 바꿀 조건
	AI_SIGNALRUN_STOP_PLAYER.setEffect(AI_SIGNAL_SUCCESS, true);
	m_tInfo.m_vecAction_Ani.push_back(AI_SIGNALRUN_STOP_PLAYER);
	// -----플레이어 접근 -----

	// %%%%%%%%%%%%%%%%%%%%%%%%%% 시그널 처리 액션 끝%%%%%%%%%%%%%%%%%%%%%%%%%%


	// %%%%%%%%%%%%%%%%%%%%%%%%%% 임시 액션(골스테이트로 가기위한) %%%%%%%%%%%%%%%%%%%%%%%%%%
	// ----- 애니메이션 강제 변환 -----
	CAction ACT_SIGNAL_CHANGE_ANIMATION_FORCED("SIGNAL_CHANGE_ANIMATION_FORCED", 1);
	ACT_SIGNAL_CHANGE_ANIMATION_FORCED.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);
	ACT_SIGNAL_CHANGE_ANIMATION_FORCED.setPrecondition(SIGNAL_CHANGE_ANIMATION_FORCED, true);
	
	ACT_SIGNAL_CHANGE_ANIMATION_FORCED.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	//ACT_SIGNAL_CHANGE_ANIMATION_FORCED.setEffect(SIGNAL_CHANGE_ANIMATION_FORCED, false);
	m_tInfo.m_vecAction_Ani.push_back(ACT_SIGNAL_CHANGE_ANIMATION_FORCED);
	// ----- 애니메이션 강제 변환 -----
	// %%%%%%%%%%%%%%%%%%%%%%%%%% 임시 액션 %%%%%%%%%%%%%%%%%%%%%%%%%%

	
	

	// %%%%%%%%%%%%%%%%%%%%%%%%%% 행동 결정 액션 %%%%%%%%%%%%%%%%%%%%%%%%%%

	CAction AI_ACT_Idle("AI_ACT_Idle", 300);
	//ACT_ANI_DON_Don_Idle_Pose.setPrecondition(ACT_WAIT, true);
	AI_ACT_Idle.setPrecondition(CH_STATE_FOUND_ENEMY, false);	// 적을 발견 못하면
	//AI_ACT_Idle.setPrecondition(CH_STATE_MID_PLAYER, false);
	AI_ACT_Idle.setPrecondition(CH_STATE_FAR_PLAYER, true);		// 가까운 거리 ~ 중간 거리

	
	AI_ACT_Idle.setEffect(SIGNAL_CHANGE_TARGET_PLAYER, true);
	AI_ACT_Idle.setEffect(CH_STATE_PEACE, true);
	AI_ACT_Idle.setEffect(ACT_IDLE, true);
	m_tInfo.m_vecAction_Act.push_back(AI_ACT_Idle);

	//CAction AI_ACT_Found_Target("AI_ACT_Found_Target", 2000);
	//AI_ACT_Found_Target.setPrecondition(TARGET_NONE, true);

	//AI_ACT_Found_Target.setEffect(TARGET_NEAR_ENEMY, true);
	//AI_ACT_Found_Target.setEffect(SIGNAL_CHANGE_TARGET, true);
	//AI_ACT_Found_Target.setEffect(ACT_IDLE, true);
	//m_tInfo.m_vecAction_Act.push_back(AI_ACT_Found_Target);

	//CAction AI_ACT_Found_Target_Battle("AI_ACT_Found_Target_Battle", 1900);
	//AI_ACT_Found_Target_Battle.setPrecondition(TARGET_NONE, true);
	//AI_ACT_Found_Target_Battle.setPrecondition(CH_STATE_BATTLE, true);

	//AI_ACT_Found_Target_Battle.setEffect(TARGET_NEAR_ENEMY, true);
	//AI_ACT_Found_Target_Battle.setEffect(SIGNAL_CHANGE_TARGET, true);
	//AI_ACT_Found_Target_Battle.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
	//AI_ACT_Found_Target_Battle.setEffect(SIGNAL_CHANGE_ANGLE, true);
	//AI_ACT_Found_Target_Battle.setEffect(SIGNAL_CHANGE_ANIMATION_FORCED, true);
	//AI_ACT_Found_Target_Battle.setEffect(ACT_IDLE, true);
	//m_tInfo.m_vecAction_Act.push_back(AI_ACT_Found_Target_Battle);

	//CAction AI_ACT_RUN_START_TO_PLAYER("AI_ACT_RUN_TO_PLAYER", 200);
	//AI_ACT_RUN_START_TO_PLAYER.setPrecondition(CH_STATE_FOUND_ENEMY, false);
	//AI_ACT_RUN_START_TO_PLAYER.setPrecondition(CH_STATE_MID_PLAYER, true);


	//AI_ACT_RUN_START_TO_PLAYER.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
	//AI_ACT_RUN_START_TO_PLAYER.setEffect(SIGNAL_CHANGE_ANGLE, true);

	//AI_ACT_RUN_START_TO_PLAYER.setEffect(SIGNAL_CHANGE_TARGET, true);
	//AI_ACT_RUN_START_TO_PLAYER.setEffect(TARGET_PLAYER, true);
	//AI_ACT_RUN_START_TO_PLAYER.setEffect(ACT_RUN, true);

	CAction AI_ACT_RUN_TO_PLAYER("AI_ACT_RUN_TO_PLAYER", 200);
	AI_ACT_RUN_TO_PLAYER.setPrecondition(TRIGER_MID_TO_NEAR_PLAYER_RUN, true);
	AI_ACT_RUN_TO_PLAYER.setPrecondition(CH_STATE_FOUND_ENEMY, false);
	
	//AI_ACT_RUN_TO_PLAYER.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
	AI_ACT_RUN_TO_PLAYER.setEffect(SIGNAL_CHANGE_ANGLE, true);

	AI_ACT_RUN_TO_PLAYER.setEffect(SIGNAL_CHANGE_TARGET_PLAYER, true);
	AI_ACT_RUN_TO_PLAYER.setEffect(ACT_RUN, true);
	m_tInfo.m_vecAction_Act.push_back(AI_ACT_RUN_TO_PLAYER);

	CAction AI_ACT_RUN_TO_ENEMY("AI_ACT_RUN_TO_ENEMY", 195);
	AI_ACT_RUN_TO_ENEMY.setPrecondition(CH_STATE_FOUND_ENEMY, true);
	AI_ACT_RUN_TO_ENEMY.setPrecondition(CH_STATE_CLOSE_TARGET, false);

	//AI_ACT_RUN_TO_ENEMY.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
	AI_ACT_RUN_TO_ENEMY.setEffect(SIGNAL_CHANGE_ANGLE, true);

	AI_ACT_RUN_TO_ENEMY.setEffect(SIGNAL_CHANGE_TARGET_NEAR_ENEMY, true);
	AI_ACT_RUN_TO_ENEMY.setEffect(CH_STATE_BATTLE, true);
	AI_ACT_RUN_TO_ENEMY.setEffect(ACT_RUN, true);
	m_tInfo.m_vecAction_Act.push_back(AI_ACT_RUN_TO_ENEMY);

	CAction AI_ACT_ATTACK("AI_ACT_ATTACK", 100);
	AI_ACT_ATTACK.setPrecondition(CH_STATE_FOUND_ENEMY, true);	// 조건 : 적발견 T
	//AI_ACT_ATTACK.setPrecondition(TARGET_NEAR_ENEMY, true);		// 타겟 : 근처적 T
	AI_ACT_ATTACK.setPrecondition(CH_STATE_CLOSE_TARGET, true);	// 범위 : 바로옆 T

	//AI_ACT_ATTACK.setEffect(TARGET_NEAR_ENEMY, true);
	AI_ACT_ATTACK.setEffect(CH_STATE_BATTLE, true);
	AI_ACT_ATTACK.setEffect(ACT_ATTACK, true);
	m_tInfo.m_vecAction_Act.push_back(AI_ACT_ATTACK);


	// 적에게 맞았다..
	{
		CAction AI_ACT_HIT( "AI_ACT_HIT", 95 );
		AI_ACT_HIT.setPrecondition( SIGNAL_HIT_ENEMY, true );

		AI_ACT_HIT.setEffect( ACT_HIT, true );
		AI_ACT_HIT.setEffect( SIGNAL_HIT_ENEMY, false );
		m_tInfo.m_vecAction_Act.push_back( AI_ACT_HIT );
	}

	// 적에게 카운터 어택을 하겠다..
	{
		CAction AI_ACT_COUNTERATTACK( "AI_ACT_COUNTERATTACK", 1 );
		AI_ACT_COUNTERATTACK.setPrecondition( SIGNAL_ACT_INTERACTION, true );

		AI_ACT_COUNTERATTACK.setEffect( ACT_INTERACTION, true );
		AI_ACT_COUNTERATTACK.setEffect( SIGNAL_ACT_INTERACTION, false );
		m_tInfo.m_vecAction_Act.push_back( AI_ACT_COUNTERATTACK );
	}


	// %%%%%%%%%%%%%%%%%%%%%%%%%% 행동 결정 액션 %%%%%%%%%%%%%%%%%%%%%%%%%%

	// %%%%%%%%%%%%%%%%%%%%%%%%%% 애니메이션 결정 액션 %%%%%%%%%%%%%%%%%%%%%%%%%%
	// ----- 기본 대기 상태 -----
	CAction ACT_ANI_DON_Don_Idle("ANI_DON_Don_Idle", 200);
	ACT_ANI_DON_Don_Idle.setPrecondition(ACT_IDLE, true);
	ACT_ANI_DON_Don_Idle.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_DON_Don_Idle.setEffect(ANI_DON_Don_Idle, true);
	ACT_ANI_DON_Don_Idle.setEffect(SIGNAL_CHANGE_ANIMATION_FORCED, true);
	ACT_ANI_DON_Don_Idle.setEffect(SIGNAL_CHANGE_ANGLE, false);
	//ACT_ANI_DON_Don_Idle.setEffect(CH_STATE_RUN, false);
	ACT_ANI_DON_Don_Idle.setEffect(CH_STATE_IDLE, true);
	//ACT_ANI_DON_Don_Idle.setEffect(CH_STATE_PEACE, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_DON_Don_Idle);

	// ~~~~~~~~~~~~~~~~~~~~~~ ACT_RUN ~~~~~~~~~~~~~~~~~~~~~~
	// ----- 달리기 -----
	CAction ACT_ANI_DON_Don_Move_Run_F("ANI_DON_Don_Move_Run_F", 90);
	ACT_ANI_DON_Don_Move_Run_F.setPrecondition(ACT_RUN, true);
	ACT_ANI_DON_Don_Move_Run_F.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_DON_Don_Move_Run_F.setEffect(ANI_DON_Don_Move_Run_F, true);
	//ACT_ANI_DON_Don_Move_Run_F.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
	ACT_ANI_DON_Don_Move_Run_F.setEffect(SIGNAL_CHANGE_ANIMATION_FORCED, true);
	ACT_ANI_DON_Don_Move_Run_F.setEffect(SIGNAL_CHANGE_ANGLE, true);
	ACT_ANI_DON_Don_Move_Run_F.setEffect(CH_STATE_RUN, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_DON_Don_Move_Run_F);
	// ----- 전력질주 -----
	CAction ACT_ANI_DON_Don_Move_Sprint_F("ANI_DON_Don_Move_Sprint_F", 89);
	ACT_ANI_DON_Don_Move_Sprint_F.setPrecondition(ACT_RUN, true);
	ACT_ANI_DON_Don_Move_Sprint_F.setPrecondition(ACT_SPRINT, true);
	ACT_ANI_DON_Don_Move_Sprint_F.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_DON_Don_Move_Sprint_F.setEffect(ANI_DON_Don_Move_Sprint_F, true);
	//ACT_ANI_DON_Don_Move_Sprint_F.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
	ACT_ANI_DON_Don_Move_Sprint_F.setEffect(SIGNAL_CHANGE_ANIMATION_FORCED, true);
	ACT_ANI_DON_Don_Move_Sprint_F.setEffect(SIGNAL_CHANGE_ANGLE, true);
	ACT_ANI_DON_Don_Move_Sprint_F.setEffect(CH_STATE_RUN, true);
	ACT_ANI_DON_Don_Move_Sprint_F.setEffect(CH_STATE_SPRINT, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_DON_Don_Move_Sprint_F);

	// ~~~~~~~~~~~~~~~~~~~~~~ ACT_HIT ~~~~~~~~~~~~~~~~~~~~~~

	// ----- 앞에 있는 적에게 맞았을 때..
	{
		CAction ACT_ANI_DON_Don_Dmg_F( "ANI_DON_Don_Dmg_F", 90 );
		ACT_ANI_DON_Don_Dmg_F.setPrecondition( ACT_HIT, true );
		ACT_ANI_DON_Don_Dmg_F.setPrecondition( SIGNAL_CHANGE_ANIMATION, true );
		ACT_ANI_DON_Don_Dmg_F.setPrecondition( ANI_DON_Don_Dmg_F, true );

		ACT_ANI_DON_Don_Dmg_F.setEffect( ANI_DON_Don_Dmg_F, true );
		ACT_ANI_DON_Don_Dmg_F.setEffect( SIGNAL_CHANGE_ANIMATION, false );
		ACT_ANI_DON_Don_Dmg_F.setEffect( CH_STATE_HIT, true );
		m_tInfo.m_vecAction_Ani.push_back( ACT_ANI_DON_Don_Dmg_F );
	}

	// ----- 오른쪽에 있는 적에게 맞았을 때..
	{
		CAction ACT_ANI_DON_Don_Dmg_R( "ANI_DON_Don_Dmg_R", 90 );
		ACT_ANI_DON_Don_Dmg_R.setPrecondition( ACT_HIT, true );
		ACT_ANI_DON_Don_Dmg_R.setPrecondition( SIGNAL_CHANGE_ANIMATION, true );
		ACT_ANI_DON_Don_Dmg_R.setPrecondition( ANI_DON_Don_Dmg_R, true );

		ACT_ANI_DON_Don_Dmg_R.setEffect( ANI_DON_Don_Dmg_R, true );
		ACT_ANI_DON_Don_Dmg_R.setEffect( SIGNAL_CHANGE_ANIMATION, false );
		ACT_ANI_DON_Don_Dmg_R.setEffect( CH_STATE_HIT, true );
		m_tInfo.m_vecAction_Ani.push_back( ACT_ANI_DON_Don_Dmg_R );
	}

	// ----- 왼쪽에 있는 적에게 맞았을 때..
	{
		CAction ACT_ANI_DON_Don_Dmg_L( "ANI_DON_Don_Dmg_L", 90 );
		ACT_ANI_DON_Don_Dmg_L.setPrecondition( ACT_HIT, true );
		ACT_ANI_DON_Don_Dmg_L.setPrecondition( SIGNAL_CHANGE_ANIMATION, true );
		ACT_ANI_DON_Don_Dmg_L.setPrecondition( ANI_DON_Don_Dmg_L, true );

		ACT_ANI_DON_Don_Dmg_L.setEffect( ANI_DON_Don_Dmg_L, true );
		ACT_ANI_DON_Don_Dmg_L.setEffect( SIGNAL_CHANGE_ANIMATION, false );
		ACT_ANI_DON_Don_Dmg_L.setEffect( CH_STATE_HIT, true );
		m_tInfo.m_vecAction_Ani.push_back( ACT_ANI_DON_Don_Dmg_L );
	}

	// ----- 뒤에 있는 적에게 맞았을 때..
	{
		CAction ACT_ANI_DON_Don_Dmg_B( "ANI_DON_Don_Dmg_B", 90 );
		ACT_ANI_DON_Don_Dmg_B.setPrecondition( ACT_HIT, true );
		ACT_ANI_DON_Don_Dmg_B.setPrecondition( SIGNAL_CHANGE_ANIMATION, true );
		ACT_ANI_DON_Don_Dmg_B.setPrecondition( ANI_DON_Don_Dmg_B, true );

		ACT_ANI_DON_Don_Dmg_B.setEffect( ANI_DON_Don_Dmg_B, true );
		ACT_ANI_DON_Don_Dmg_B.setEffect( SIGNAL_CHANGE_ANIMATION, false );
		ACT_ANI_DON_Don_Dmg_B.setEffect( CH_STATE_HIT, true );
		m_tInfo.m_vecAction_Ani.push_back( ACT_ANI_DON_Don_Dmg_B );
	}
	
	// ----- 넘어지는 모션..
	{
		CAction ACT_ANI_DON_Don_Knockdown_Start( "ANI_DON_Don_Knockdown_Start", 89 );
		ACT_ANI_DON_Don_Knockdown_Start.setPrecondition( ACT_HIT, true );
		ACT_ANI_DON_Don_Knockdown_Start.setPrecondition( SIGNAL_CHANGE_ANIMATION, true );
		ACT_ANI_DON_Don_Knockdown_Start.setPrecondition( SIGNAL_DOWN_ATK, true );

		ACT_ANI_DON_Don_Knockdown_Start.setEffect( ANI_DON_Don_Knockdown_Start, true );
		ACT_ANI_DON_Don_Knockdown_Start.setEffect( SIGNAL_CHANGE_ANIMATION, false );
		ACT_ANI_DON_Don_Knockdown_Start.setEffect( CH_STATE_HIT, true );
		ACT_ANI_DON_Don_Knockdown_Start.setEffect( SIGNAL_DOWN_ATK, false );
		m_tInfo.m_vecAction_Ani.push_back( ACT_ANI_DON_Don_Knockdown_Start );
	}

	// ----- 넘어진 뒤 일어나는 모션..
	{
		CAction ACT_ANI_DON_Don_Knockdown_Getup( "ANI_DON_Don_Knockdown_Getup", 88 );
		//ACT_ANI_DON_Don_Knockdown_Getup.setPrecondition( ACT_HIT, true );
		ACT_ANI_DON_Don_Knockdown_Getup.setPrecondition( SIGNAL_CHANGE_ANIMATION, true );
		ACT_ANI_DON_Don_Knockdown_Getup.setPrecondition( ANI_DON_Don_Knockdown_Start, true );

		ACT_ANI_DON_Don_Knockdown_Getup.setEffect( ANI_DON_Don_Knockdown_Getup, true );
		ACT_ANI_DON_Don_Knockdown_Getup.setEffect( SIGNAL_CHANGE_ANIMATION, false );
		ACT_ANI_DON_Don_Knockdown_Getup.setEffect( CH_STATE_HIT, true );
		m_tInfo.m_vecAction_Ani.push_back( ACT_ANI_DON_Don_Knockdown_Getup );
	}

	// ----- 대굴대굴 굴러가는 모션..
	{
		CAction ACT_ANI_DON_Trtl_Knockdown_Med_Start( "ANI_DON_Trtl_Knockdown_Med_Start", 89 );
		ACT_ANI_DON_Trtl_Knockdown_Med_Start.setPrecondition( ACT_HIT, true );
		ACT_ANI_DON_Trtl_Knockdown_Med_Start.setPrecondition( SIGNAL_CHANGE_ANIMATION, true );
		ACT_ANI_DON_Trtl_Knockdown_Med_Start.setPrecondition( SIGNAL_STRONG_ATK, true );

		ACT_ANI_DON_Trtl_Knockdown_Med_Start.setEffect( ANI_DON_Trtl_Knockdown_Med_Start, true );
		ACT_ANI_DON_Trtl_Knockdown_Med_Start.setEffect( SIGNAL_CHANGE_ANIMATION, false );
		ACT_ANI_DON_Trtl_Knockdown_Med_Start.setEffect( CH_STATE_HIT, true );
		ACT_ANI_DON_Trtl_Knockdown_Med_Start.setEffect( SIGNAL_STRONG_ATK, false );
		m_tInfo.m_vecAction_Ani.push_back( ACT_ANI_DON_Trtl_Knockdown_Med_Start );
	}

	// ----- 대굴대굴 굴러간 후 일어나는 모션..
	{
		CAction ACT_ANI_DON_Trtl_Knockdown_Getup( "ANI_DON_Trtl_Knockdown_Getup", 88 );
		//ACT_ANI_DON_Trtl_Knockdown_Getup.setPrecondition( ACT_HIT, true );
		ACT_ANI_DON_Trtl_Knockdown_Getup.setPrecondition( SIGNAL_CHANGE_ANIMATION, true );
		ACT_ANI_DON_Trtl_Knockdown_Getup.setPrecondition( ANI_DON_Trtl_Knockdown_Med_Start, true );

		ACT_ANI_DON_Trtl_Knockdown_Getup.setEffect( ANI_DON_Trtl_Knockdown_Getup, true );
		ACT_ANI_DON_Trtl_Knockdown_Getup.setEffect( SIGNAL_CHANGE_ANIMATION, false );
		ACT_ANI_DON_Trtl_Knockdown_Getup.setEffect( CH_STATE_HIT, true );
		m_tInfo.m_vecAction_Ani.push_back( ACT_ANI_DON_Trtl_Knockdown_Getup );
	}

	// ~~~~~~~~~~~~~~~~~~~~~~ ACT_HIT ~~~~~~~~~~~~~~~~~~~~~~

	// ~~~~~~~~~~~~~~~~~~~~~~ ACT_SIDESTEP ~~~~~~~~~~~~~~~~~~~~~~
	// ----- 앞 회피 -----
	CAction ACT_ANI_DON_Don_Evade_F0("ANI_DON_Don_Evade_F0", 70);
	ACT_ANI_DON_Don_Evade_F0.setPrecondition(ACT_SIDESTEP, true);
	ACT_ANI_DON_Don_Evade_F0.setPrecondition(DEST_DIR_0, true);
	ACT_ANI_DON_Don_Evade_F0.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_DON_Don_Evade_F0.setEffect(ANI_DON_Don_Evade_F, true);
	ACT_ANI_DON_Don_Evade_F0.setEffect(SIGNAL_CHANGE_ANGLE, false);
	ACT_ANI_DON_Don_Evade_F0.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	ACT_ANI_DON_Don_Evade_F0.setEffect(CH_STATE_SIDESTEP, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_DON_Don_Evade_F0);

	CAction ACT_ANI_DON_Don_Evade_F45("ANI_DON_Don_Evade_F45", 70);
	ACT_ANI_DON_Don_Evade_F45.setPrecondition(ACT_SIDESTEP, true);
	ACT_ANI_DON_Don_Evade_F45.setPrecondition(DEST_DIR_45, true);
	ACT_ANI_DON_Don_Evade_F45.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_DON_Don_Evade_F45.setEffect(ANI_DON_Don_Evade_F, true);
	ACT_ANI_DON_Don_Evade_F45.setEffect(SIGNAL_CHANGE_ANGLE, false);
	ACT_ANI_DON_Don_Evade_F45.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	ACT_ANI_DON_Don_Evade_F45.setEffect(CH_STATE_SIDESTEP, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_DON_Don_Evade_F45);

	CAction ACT_ANI_DON_Don_Evade_F315("ANI_DON_Don_Evade_F315", 70);
	ACT_ANI_DON_Don_Evade_F315.setPrecondition(ACT_SIDESTEP, true);
	ACT_ANI_DON_Don_Evade_F315.setPrecondition(DEST_DIR_315, true);
	ACT_ANI_DON_Don_Evade_F315.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_DON_Don_Evade_F315.setEffect(ANI_DON_Don_Evade_F, true);
	ACT_ANI_DON_Don_Evade_F315.setEffect(SIGNAL_CHANGE_ANGLE, false);
	ACT_ANI_DON_Don_Evade_F315.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	ACT_ANI_DON_Don_Evade_F315.setEffect(CH_STATE_SIDESTEP, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_DON_Don_Evade_F315);
	// ----- 오른쪽 회피 -----
	CAction ACT_ANI_DON_Don_Evade_R90("ANI_DON_Don_Evade_R90", 70);
	ACT_ANI_DON_Don_Evade_R90.setPrecondition(ACT_SIDESTEP, true);
	ACT_ANI_DON_Don_Evade_R90.setPrecondition(DEST_DIR_90, true);
	ACT_ANI_DON_Don_Evade_R90.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_DON_Don_Evade_R90.setEffect(ANI_DON_Don_Evade_R, true);
	ACT_ANI_DON_Don_Evade_R90.setEffect(SIGNAL_CHANGE_ANGLE, false);
	ACT_ANI_DON_Don_Evade_R90.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	ACT_ANI_DON_Don_Evade_R90.setEffect(CH_STATE_SIDESTEP, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_DON_Don_Evade_R90);
	// ----- 왼쪽 회피 -----
	CAction ACT_ANI_DON_Don_Evade_L270("ANI_DON_Don_Evade_L270", 70);
	ACT_ANI_DON_Don_Evade_L270.setPrecondition(ACT_SIDESTEP, true);
	ACT_ANI_DON_Don_Evade_L270.setPrecondition(DEST_DIR_270, true);
	ACT_ANI_DON_Don_Evade_L270.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_DON_Don_Evade_L270.setEffect(ANI_DON_Don_Evade_L, true);
	ACT_ANI_DON_Don_Evade_L270.setEffect(SIGNAL_CHANGE_ANGLE, false);
	ACT_ANI_DON_Don_Evade_L270.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	ACT_ANI_DON_Don_Evade_L270.setEffect(CH_STATE_SIDESTEP, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_DON_Don_Evade_L270);
	// ----- 뒤쪽 회피 -----
	CAction ACT_ANI_DON_Don_Evade_B180("ANI_DON_Don_Evade_B180", 70);
	ACT_ANI_DON_Don_Evade_B180.setPrecondition(ACT_SIDESTEP, true);
	ACT_ANI_DON_Don_Evade_B180.setPrecondition(DEST_DIR_180, true);
	ACT_ANI_DON_Don_Evade_B180.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_DON_Don_Evade_B180.setEffect(ANI_DON_Don_Evade_B, true);
	ACT_ANI_DON_Don_Evade_B180.setEffect(SIGNAL_CHANGE_ANGLE, false);
	ACT_ANI_DON_Don_Evade_B180.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	ACT_ANI_DON_Don_Evade_B180.setEffect(CH_STATE_SIDESTEP, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_DON_Don_Evade_B180);

	CAction ACT_ANI_DON_Don_Evade_B225("ANI_DON_Don_Evade_B225", 70);
	ACT_ANI_DON_Don_Evade_B225.setPrecondition(ACT_SIDESTEP, true);
	ACT_ANI_DON_Don_Evade_B225.setPrecondition(DEST_DIR_225, true);
	ACT_ANI_DON_Don_Evade_B225.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_DON_Don_Evade_B225.setEffect(ANI_DON_Don_Evade_B, true);
	ACT_ANI_DON_Don_Evade_B225.setEffect(SIGNAL_CHANGE_ANGLE, false);
	ACT_ANI_DON_Don_Evade_B225.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	ACT_ANI_DON_Don_Evade_B225.setEffect(CH_STATE_SIDESTEP, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_DON_Don_Evade_B225);

	CAction ACT_ANI_DON_Don_Evade_B135("ANI_DON_Don_Evade_B135", 70);
	ACT_ANI_DON_Don_Evade_B135.setPrecondition(ACT_SIDESTEP, true);
	ACT_ANI_DON_Don_Evade_B135.setPrecondition(DEST_DIR_135, true);
	ACT_ANI_DON_Don_Evade_B135.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_DON_Don_Evade_B135.setEffect(ANI_DON_Don_Evade_B, true);
	ACT_ANI_DON_Don_Evade_B135.setEffect(SIGNAL_CHANGE_ANGLE, false);
	ACT_ANI_DON_Don_Evade_B135.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	ACT_ANI_DON_Don_Evade_B135.setEffect(CH_STATE_SIDESTEP, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_DON_Don_Evade_B135);




	// ~~~~~~~~~~~~~~~~~~~~~~ ACT_ATTACK ~~~~~~~~~~~~~~~~~~~~~~
	
	// ----- 기본 공격1 -----
	CAction ACT_ANI_DON_Don_Melee_01( "ANI_DON_Don_Melee_01", 80 );
	ACT_ANI_DON_Don_Melee_01.setPrecondition( ACT_ATTACK, true );
	ACT_ANI_DON_Don_Melee_01.setPrecondition( SIGNAL_CHANGE_ANIMATION, true );
	//ACT_ANI_DON_Don_Melee_01.setPrecondition(CH_STATE_CLOSE_TARGET, true);

	ACT_ANI_DON_Don_Melee_01.setEffect( ANI_DON_Don_Melee_01, true );
	ACT_ANI_DON_Don_Melee_01.setEffect( SIGNAL_CHANGE_ANIMATION, false );
	ACT_ANI_DON_Don_Melee_01.setEffect( SIGNAL_CHANGE_ANGLE_ONCE, true );
	ACT_ANI_DON_Don_Melee_01.setEffect( SIGNAL_SET_ANGLE_TARGET, true );
	ACT_ANI_DON_Don_Melee_01.setEffect( TARGET_NEAR_ENEMY, true );
	ACT_ANI_DON_Don_Melee_01.setEffect( CH_STATE_BATTLE, true );
	ACT_ANI_DON_Don_Melee_01.setEffect( CH_STATE_ATTACK, true );
	m_tInfo.m_vecAction_Ani.push_back( ACT_ANI_DON_Don_Melee_01 );
	// ----- 기본 공격2 -----
	CAction ACT_ANI_DON_Don_Melee_02( "ANI_DON_Don_Melee_02", 79 );
	ACT_ANI_DON_Don_Melee_02.setPrecondition( ACT_ATTACK, true );
	ACT_ANI_DON_Don_Melee_02.setPrecondition( ANI_DON_Don_Melee_01, true );
	ACT_ANI_DON_Don_Melee_02.setPrecondition( SIGNAL_CHANGE_ANIMATION, true );
	//ACT_ANI_DON_Don_Melee_02.setPrecondition(CH_STATE_CLOSE_TARGET, true);

	ACT_ANI_DON_Don_Melee_02.setEffect( ANI_DON_Don_Melee_02, true );
	ACT_ANI_DON_Don_Melee_02.setEffect( SIGNAL_CHANGE_ANIMATION, false );
	ACT_ANI_DON_Don_Melee_02.setEffect( SIGNAL_CHANGE_ANGLE_ONCE, true );
	ACT_ANI_DON_Don_Melee_02.setEffect( SIGNAL_SET_ANGLE_TARGET, true );
	ACT_ANI_DON_Don_Melee_02.setEffect( TARGET_NEAR_ENEMY, true );
	ACT_ANI_DON_Don_Melee_02.setEffect( CH_STATE_BATTLE, true );
	ACT_ANI_DON_Don_Melee_02.setEffect( CH_STATE_ATTACK, true );
	m_tInfo.m_vecAction_Ani.push_back( ACT_ANI_DON_Don_Melee_02 );
	// ----- 기본 공격3 -----
	CAction ACT_ANI_DON_Don_Melee_03( "ANI_DON_Don_Melee_03", 79 );
	ACT_ANI_DON_Don_Melee_03.setPrecondition( ACT_ATTACK, true );
	ACT_ANI_DON_Don_Melee_03.setPrecondition( ANI_DON_Don_Melee_02, true );
	ACT_ANI_DON_Don_Melee_03.setPrecondition( SIGNAL_CHANGE_ANIMATION, true );
	//ACT_ANI_DON_Don_Melee_03.setPrecondition(CH_STATE_CLOSE_TARGET, true);

	ACT_ANI_DON_Don_Melee_03.setEffect( ANI_DON_Don_Melee_03, true );
	ACT_ANI_DON_Don_Melee_03.setEffect( SIGNAL_CHANGE_ANIMATION, false );
	ACT_ANI_DON_Don_Melee_03.setEffect( SIGNAL_CHANGE_ANGLE_ONCE, true );
	ACT_ANI_DON_Don_Melee_03.setEffect( SIGNAL_SET_ANGLE_TARGET, true );
	ACT_ANI_DON_Don_Melee_03.setEffect( TARGET_NEAR_ENEMY, true );
	ACT_ANI_DON_Don_Melee_03.setEffect( CH_STATE_BATTLE, true );
	ACT_ANI_DON_Don_Melee_03.setEffect( CH_STATE_ATTACK, true );
	m_tInfo.m_vecAction_Ani.push_back( ACT_ANI_DON_Don_Melee_03 );
	// ----- 기본 공격4 -----
	CAction ACT_ANI_DON_Don_Melee_04( "ANI_DON_Don_Melee_04", 79 );
	ACT_ANI_DON_Don_Melee_04.setPrecondition( ACT_ATTACK, true );
	ACT_ANI_DON_Don_Melee_04.setPrecondition( ANI_DON_Don_Melee_03, true );
	ACT_ANI_DON_Don_Melee_04.setPrecondition( SIGNAL_CHANGE_ANIMATION, true );
	//ACT_ANI_DON_Don_Melee_04.setPrecondition(CH_STATE_CLOSE_TARGET, true);

	ACT_ANI_DON_Don_Melee_04.setEffect( ANI_DON_Don_Melee_04, true );
	ACT_ANI_DON_Don_Melee_04.setEffect( SIGNAL_CHANGE_ANIMATION, false );
	ACT_ANI_DON_Don_Melee_04.setEffect( SIGNAL_CHANGE_ANGLE_ONCE, true );
	ACT_ANI_DON_Don_Melee_04.setEffect( SIGNAL_SET_ANGLE_TARGET, true );
	ACT_ANI_DON_Don_Melee_04.setEffect( TARGET_NEAR_ENEMY, true );
	ACT_ANI_DON_Don_Melee_04.setEffect( CH_STATE_BATTLE, true );
	ACT_ANI_DON_Don_Melee_04.setEffect( CH_STATE_ATTACK, true );
	m_tInfo.m_vecAction_Ani.push_back( ACT_ANI_DON_Don_Melee_04 );
	// ----- 기본 공격5 -----
	CAction ACT_ANI_DON_Don_Melee_05_x( "ANI_DON_Don_Melee_05_x", 79 );
	ACT_ANI_DON_Don_Melee_05_x.setPrecondition( ACT_ATTACK, true );
	ACT_ANI_DON_Don_Melee_05_x.setPrecondition( ANI_DON_Don_Melee_04, true );
	ACT_ANI_DON_Don_Melee_05_x.setPrecondition( SIGNAL_CHANGE_ANIMATION, true );
	//ACT_ANI_DON_Don_Melee_05_x.setPrecondition(CH_STATE_CLOSE_TARGET, true);

	ACT_ANI_DON_Don_Melee_05_x.setEffect( ANI_DON_Don_Melee_05_x, true );
	ACT_ANI_DON_Don_Melee_05_x.setEffect( SIGNAL_CHANGE_ANIMATION, false );
	ACT_ANI_DON_Don_Melee_05_x.setEffect( SIGNAL_CHANGE_ANGLE_ONCE, true );
	ACT_ANI_DON_Don_Melee_05_x.setEffect( SIGNAL_SET_ANGLE_TARGET, true );
	ACT_ANI_DON_Don_Melee_05_x.setEffect( TARGET_NEAR_ENEMY, true );
	ACT_ANI_DON_Don_Melee_05_x.setEffect( CH_STATE_BATTLE, true );
	ACT_ANI_DON_Don_Melee_05_x.setEffect( CH_STATE_ATTACK, true );
	m_tInfo.m_vecAction_Ani.push_back( ACT_ANI_DON_Don_Melee_05_x );

	// ----- 기본 공격2 -----
	CAction ACT_ANI_DON_Don_Melee_02_2( "ANI_DON_Don_Melee_02", 79 );
	ACT_ANI_DON_Don_Melee_02_2.setPrecondition( ACT_ATTACK, true );
	ACT_ANI_DON_Don_Melee_02_2.setPrecondition( ANI_DON_Don_Kick_01, true );
	ACT_ANI_DON_Don_Melee_02_2.setPrecondition( SIGNAL_CHANGE_ANIMATION, true );
	//ACT_ANI_DON_Don_Melee_02.setPrecondition(CH_STATE_CLOSE_TARGET, true);

	ACT_ANI_DON_Don_Melee_02_2.setEffect( ANI_DON_Don_Melee_02, true );
	ACT_ANI_DON_Don_Melee_02_2.setEffect( SIGNAL_CHANGE_ANIMATION, false );
	ACT_ANI_DON_Don_Melee_02_2.setEffect( SIGNAL_CHANGE_ANGLE_ONCE, true );
	ACT_ANI_DON_Don_Melee_02_2.setEffect( SIGNAL_SET_ANGLE_TARGET, true );
	ACT_ANI_DON_Don_Melee_02_2.setEffect( TARGET_NEAR_ENEMY, true );
	ACT_ANI_DON_Don_Melee_02_2.setEffect( CH_STATE_BATTLE, true );
	ACT_ANI_DON_Don_Melee_02_2.setEffect( CH_STATE_ATTACK, true );
	m_tInfo.m_vecAction_Ani.push_back( ACT_ANI_DON_Don_Melee_02_2 );
	// ----- 기본 공격3 -----
	CAction ACT_ANI_DON_Don_Melee_03_2( "ANI_DON_Don_Melee_03", 79 );
	ACT_ANI_DON_Don_Melee_03_2.setPrecondition( ACT_ATTACK, true );
	ACT_ANI_DON_Don_Melee_03_2.setPrecondition( ANI_DON_Don_Kick_02, true );
	ACT_ANI_DON_Don_Melee_03_2.setPrecondition( SIGNAL_CHANGE_ANIMATION, true );
	//ACT_ANI_DON_Don_Melee_03.setPrecondition(CH_STATE_CLOSE_TARGET, true);

	ACT_ANI_DON_Don_Melee_03_2.setEffect( ANI_DON_Don_Melee_03, true );
	ACT_ANI_DON_Don_Melee_03_2.setEffect( SIGNAL_CHANGE_ANIMATION, false );
	ACT_ANI_DON_Don_Melee_03_2.setEffect( SIGNAL_CHANGE_ANGLE_ONCE, true );
	ACT_ANI_DON_Don_Melee_03_2.setEffect( SIGNAL_SET_ANGLE_TARGET, true );
	ACT_ANI_DON_Don_Melee_03_2.setEffect( TARGET_NEAR_ENEMY, true );
	ACT_ANI_DON_Don_Melee_03_2.setEffect( CH_STATE_BATTLE, true );
	ACT_ANI_DON_Don_Melee_03_2.setEffect( CH_STATE_ATTACK, true );
	m_tInfo.m_vecAction_Ani.push_back( ACT_ANI_DON_Don_Melee_03_2 );
	// ----- 기본 공격4 -----
	CAction ACT_ANI_DON_Don_Melee_04_2( "ANI_DON_Don_Melee_04", 79 );
	ACT_ANI_DON_Don_Melee_04_2.setPrecondition( ACT_ATTACK, true );
	ACT_ANI_DON_Don_Melee_04_2.setPrecondition( ANI_DON_Don_Kick_03, true );
	ACT_ANI_DON_Don_Melee_04_2.setPrecondition( SIGNAL_CHANGE_ANIMATION, true );
	//ACT_ANI_DON_Don_Melee_04.setPrecondition(CH_STATE_CLOSE_TARGET, true);

	ACT_ANI_DON_Don_Melee_04_2.setEffect( ANI_DON_Don_Melee_04, true );
	ACT_ANI_DON_Don_Melee_04_2.setEffect( SIGNAL_CHANGE_ANIMATION, false );
	ACT_ANI_DON_Don_Melee_04_2.setEffect( SIGNAL_CHANGE_ANGLE_ONCE, true );
	ACT_ANI_DON_Don_Melee_04_2.setEffect( SIGNAL_SET_ANGLE_TARGET, true );
	ACT_ANI_DON_Don_Melee_04_2.setEffect( TARGET_NEAR_ENEMY, true );
	ACT_ANI_DON_Don_Melee_04_2.setEffect( CH_STATE_BATTLE, true );
	ACT_ANI_DON_Don_Melee_04_2.setEffect( CH_STATE_ATTACK, true );
	m_tInfo.m_vecAction_Ani.push_back( ACT_ANI_DON_Don_Melee_04_2 );
	// ----- 기본 공격5 -----
	CAction ACT_ANI_DON_Don_Melee_05_x_2( "ANI_DON_Don_Melee_05_x", 79 );
	ACT_ANI_DON_Don_Melee_05_x_2.setPrecondition( ACT_ATTACK, true );
	ACT_ANI_DON_Don_Melee_05_x_2.setPrecondition( ANI_DON_Don_Kick_04, true );
	ACT_ANI_DON_Don_Melee_05_x_2.setPrecondition( SIGNAL_CHANGE_ANIMATION, true );
	//ACT_ANI_DON_Don_Melee_05_x.setPrecondition(CH_STATE_CLOSE_TARGET, true);

	ACT_ANI_DON_Don_Melee_05_x_2.setEffect( ANI_DON_Don_Melee_05_x, true );
	ACT_ANI_DON_Don_Melee_05_x_2.setEffect( SIGNAL_CHANGE_ANIMATION, false );
	ACT_ANI_DON_Don_Melee_05_x_2.setEffect( SIGNAL_CHANGE_ANGLE_ONCE, true );
	ACT_ANI_DON_Don_Melee_05_x_2.setEffect( SIGNAL_SET_ANGLE_TARGET, true );
	ACT_ANI_DON_Don_Melee_05_x_2.setEffect( TARGET_NEAR_ENEMY, true );
	ACT_ANI_DON_Don_Melee_05_x_2.setEffect( CH_STATE_BATTLE, true );
	ACT_ANI_DON_Don_Melee_05_x_2.setEffect( CH_STATE_ATTACK, true );
	m_tInfo.m_vecAction_Ani.push_back( ACT_ANI_DON_Don_Melee_05_x_2 );

	// ~~~~~~~~~~~~~~~~~~~~~~ ACT_ATTACK ~~~~~~~~~~~~~~~~~~~~~~

	// ~~~~~~~~~~~~~~~~~~~~~~ ACT_KICK ~~~~~~~~~~~~~~~~~~~~~~

	// ----- 발차기 공격1 -----
	CAction ACT_ANI_DON_Don_Kick_01("ANI_DON_Don_Kick_01", 80);
	ACT_ANI_DON_Don_Kick_01.setPrecondition(ACT_KICK, true);
	ACT_ANI_DON_Don_Kick_01.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);
	//ACT_ANI_DON_Don_Kick_01.setPrecondition(CH_STATE_CLOSE_TARGET, true);

	ACT_ANI_DON_Don_Kick_01.setEffect(ANI_DON_Don_Kick_01, true);
	ACT_ANI_DON_Don_Kick_01.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	ACT_ANI_DON_Don_Kick_01.setEffect(SIGNAL_CHANGE_ANGLE_ONCE, true);
	ACT_ANI_DON_Don_Kick_01.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
	ACT_ANI_DON_Don_Kick_01.setEffect(TARGET_NEAR_ENEMY, true);
	ACT_ANI_DON_Don_Kick_01.setEffect(CH_STATE_BATTLE, true);
	ACT_ANI_DON_Don_Kick_01.setEffect(CH_STATE_ATTACK, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_DON_Don_Kick_01);
	// ----- 발차기 공격2 -----
	CAction ACT_ANI_DON_Don_Kick_02("ANI_DON_Don_Kick_02", 79);
	ACT_ANI_DON_Don_Kick_02.setPrecondition(ACT_KICK, true);
	ACT_ANI_DON_Don_Kick_02.setPrecondition(ANI_DON_Don_Kick_01, true);
	ACT_ANI_DON_Don_Kick_02.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);
	//ACT_ANI_DON_Don_Kick_02.setPrecondition(CH_STATE_CLOSE_TARGET, true);

	ACT_ANI_DON_Don_Kick_02.setEffect(ANI_DON_Don_Kick_02, true);
	ACT_ANI_DON_Don_Kick_02.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	ACT_ANI_DON_Don_Kick_02.setEffect(SIGNAL_CHANGE_ANGLE_ONCE, true);
	ACT_ANI_DON_Don_Kick_02.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
	ACT_ANI_DON_Don_Kick_02.setEffect(CH_STATE_BATTLE, true);
	ACT_ANI_DON_Don_Kick_02.setEffect(CH_STATE_ATTACK, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_DON_Don_Kick_02);
	// ----- 발차기 공격3 -----
	CAction ACT_ANI_DON_Don_Kick_03("ANI_DON_Don_Kick_03", 79);
	ACT_ANI_DON_Don_Kick_03.setPrecondition(ACT_KICK, true);
	ACT_ANI_DON_Don_Kick_03.setPrecondition(ANI_DON_Don_Kick_02, true);
	ACT_ANI_DON_Don_Kick_03.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);
	//ACT_ANI_DON_Don_Kick_03.setPrecondition(CH_STATE_CLOSE_TARGET, true);

	ACT_ANI_DON_Don_Kick_03.setEffect(ANI_DON_Don_Kick_03, true);
	ACT_ANI_DON_Don_Kick_03.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	ACT_ANI_DON_Don_Kick_03.setEffect(SIGNAL_CHANGE_ANGLE_ONCE, true);
	ACT_ANI_DON_Don_Kick_03.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
	ACT_ANI_DON_Don_Kick_03.setEffect(CH_STATE_BATTLE, true);
	ACT_ANI_DON_Don_Kick_03.setEffect(CH_STATE_ATTACK, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_DON_Don_Kick_03);
	// ----- 발차기 공격4 -----
	CAction ACT_ANI_DON_Don_Kick_04("ANI_DON_Don_Kick_04", 79);
	ACT_ANI_DON_Don_Kick_04.setPrecondition(ACT_KICK, true);
	ACT_ANI_DON_Don_Kick_04.setPrecondition(ANI_DON_Don_Kick_03, true);
	ACT_ANI_DON_Don_Kick_04.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);
	//ACT_ANI_DON_Don_Kick_04.setPrecondition(CH_STATE_CLOSE_TARGET, true);

	ACT_ANI_DON_Don_Kick_04.setEffect(ANI_DON_Don_Kick_04, true);
	ACT_ANI_DON_Don_Kick_04.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	ACT_ANI_DON_Don_Kick_04.setEffect(SIGNAL_CHANGE_ANGLE_ONCE, true);
	ACT_ANI_DON_Don_Kick_04.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
	ACT_ANI_DON_Don_Kick_04.setEffect(CH_STATE_BATTLE, true);
	ACT_ANI_DON_Don_Kick_04.setEffect(CH_STATE_ATTACK, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_DON_Don_Kick_04);

	// ----- 발차기 공격2 -----
	CAction ACT_ANI_DON_Don_Kick_02_2("ANI_DON_Don_Kick_02", 79);
	ACT_ANI_DON_Don_Kick_02_2.setPrecondition(ACT_KICK, true);
	ACT_ANI_DON_Don_Kick_02_2.setPrecondition(ANI_DON_Don_Melee_01, true);
	ACT_ANI_DON_Don_Kick_02_2.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);
	//ACT_ANI_DON_Don_Kick_02.setPrecondition(CH_STATE_CLOSE_TARGET, true);

	ACT_ANI_DON_Don_Kick_02_2.setEffect(ANI_DON_Don_Kick_02, true);
	ACT_ANI_DON_Don_Kick_02_2.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	ACT_ANI_DON_Don_Kick_02_2.setEffect(SIGNAL_CHANGE_ANGLE_ONCE, true);
	ACT_ANI_DON_Don_Kick_02_2.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
	ACT_ANI_DON_Don_Kick_02_2.setEffect(CH_STATE_BATTLE, true);
	ACT_ANI_DON_Don_Kick_02_2.setEffect(CH_STATE_ATTACK, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_DON_Don_Kick_02_2);
	// ----- 발차기 공격3 -----
	CAction ACT_ANI_DON_Don_Kick_03_2("ANI_DON_Don_Kick_03", 79);
	ACT_ANI_DON_Don_Kick_03_2.setPrecondition(ACT_KICK, true);
	ACT_ANI_DON_Don_Kick_03_2.setPrecondition(ANI_DON_Don_Melee_02, true);
	ACT_ANI_DON_Don_Kick_03_2.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);
	//ACT_ANI_DON_Don_Kick_03.setPrecondition(CH_STATE_CLOSE_TARGET, true);

	ACT_ANI_DON_Don_Kick_03_2.setEffect(ANI_DON_Don_Kick_03, true);
	ACT_ANI_DON_Don_Kick_03_2.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	ACT_ANI_DON_Don_Kick_03_2.setEffect(SIGNAL_CHANGE_ANGLE_ONCE, true);
	ACT_ANI_DON_Don_Kick_03_2.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
	ACT_ANI_DON_Don_Kick_03_2.setEffect(CH_STATE_BATTLE, true);
	ACT_ANI_DON_Don_Kick_03_2.setEffect(CH_STATE_ATTACK, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_DON_Don_Kick_03_2);
	// ----- 발차기 공격4 -----
	CAction ACT_ANI_DON_Don_Kick_04_2("ANI_DON_Don_Kick_04", 79);
	ACT_ANI_DON_Don_Kick_04_2.setPrecondition(ACT_KICK, true);
	ACT_ANI_DON_Don_Kick_04_2.setPrecondition(ANI_DON_Don_Melee_03, true);
	ACT_ANI_DON_Don_Kick_04_2.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);
	//ACT_ANI_DON_Don_Kick_04.setPrecondition(CH_STATE_CLOSE_TARGET, true);

	ACT_ANI_DON_Don_Kick_04_2.setEffect(ANI_DON_Don_Kick_04, true);
	ACT_ANI_DON_Don_Kick_04_2.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	ACT_ANI_DON_Don_Kick_04_2.setEffect(SIGNAL_CHANGE_ANGLE_ONCE, true);
	ACT_ANI_DON_Don_Kick_04_2.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
	ACT_ANI_DON_Don_Kick_04_2.setEffect(CH_STATE_BATTLE, true);
	ACT_ANI_DON_Don_Kick_04_2.setEffect(CH_STATE_ATTACK, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_DON_Don_Kick_04_2);

	// ~~~~~~~~~~~~~~~~~~~~~~ ACT_KICK ~~~~~~~~~~~~~~~~~~~~~~

	// ~~~~~~~~~~~~~~~~~~~~~~ ACT_COUNTERATTACK ~~~~~~~~~~~~~~~~~~~~~~

	// ----- 앞의 적 카운터 공격.. -----
	{
		CAction ACT_ANI_DON_Don_Counter_F2F( "ANI_DON_Don_Counter_F2F", 80 );
		ACT_ANI_DON_Don_Counter_F2F.setPrecondition( ACT_INTERACTION, true );
		ACT_ANI_DON_Don_Counter_F2F.setPrecondition( SIGNAL_CHANGE_ANIMATION, true );
		ACT_ANI_DON_Don_Counter_F2F.setPrecondition( SIGNAL_INTERACTION_COUNTER_FRONT1, true );

		ACT_ANI_DON_Don_Counter_F2F.setEffect( SIGNAL_INTERACTION_COUNTER_FRONT1, false );
		ACT_ANI_DON_Don_Counter_F2F.setEffect( ANI_DON_Don_Counter_F2F, true );
		ACT_ANI_DON_Don_Counter_F2F.setEffect( SIGNAL_CHANGE_ANIMATION, false );
		ACT_ANI_DON_Don_Counter_F2F.setEffect( SIGNAL_CHANGE_ANGLE_ONCE, true );
		ACT_ANI_DON_Don_Counter_F2F.setEffect( SIGNAL_SET_ANGLE_TARGET, true );
		ACT_ANI_DON_Don_Counter_F2F.setEffect( CH_STATE_BATTLE, true );
		ACT_ANI_DON_Don_Counter_F2F.setEffect( CH_STATE_INTERACTION, true );
		m_tInfo.m_vecAction_Ani.push_back( ACT_ANI_DON_Don_Counter_F2F );
	}

	// ----- 뒤의 적 카운터 공격.. -----
	{
		CAction ACT_ANI_DON_Don_Counter_Push_B( "ANI_DON_Don_Counter_Push_B", 80 );
		ACT_ANI_DON_Don_Counter_Push_B.setPrecondition( ACT_INTERACTION, true );
		ACT_ANI_DON_Don_Counter_Push_B.setPrecondition( SIGNAL_CHANGE_ANIMATION, true );
		ACT_ANI_DON_Don_Counter_Push_B.setPrecondition( SIGNAL_INTERACTION_COUNTER_BACK1, true );

		ACT_ANI_DON_Don_Counter_Push_B.setEffect( SIGNAL_INTERACTION_COUNTER_BACK1, false );
		ACT_ANI_DON_Don_Counter_Push_B.setEffect( ANI_DON_Don_Counter_Push_B, true );
		ACT_ANI_DON_Don_Counter_Push_B.setEffect( SIGNAL_CHANGE_ANIMATION, false );
		ACT_ANI_DON_Don_Counter_Push_B.setEffect( SIGNAL_CHANGE_ANGLE_ONCE, true );
		ACT_ANI_DON_Don_Counter_Push_B.setEffect( SIGNAL_SET_ANGLE_TARGET, true );
		ACT_ANI_DON_Don_Counter_Push_B.setEffect( CH_STATE_BATTLE, true );
		ACT_ANI_DON_Don_Counter_Push_B.setEffect( CH_STATE_INTERACTION, true );
		m_tInfo.m_vecAction_Ani.push_back( ACT_ANI_DON_Don_Counter_Push_B );
	}

	// ~~~~~~~~~~~~~~~~~~~~~~ ACT_COUNTERATTACK ~~~~~~~~~~~~~~~~~~~~~~

	// ~~~~~~~~~~~~~~~~~~~~~~ ACT_GUARD ~~~~~~~~~~~~~~~~~~~~~~

	// ----- 가드 동작( 맞으면 카운터가 나감 ).. -----
	{
		CAction ACT_ANI_DON_Don_Idle_Counter( "ANI_DON_Don_Idle_Counter", 80 );
		ACT_ANI_DON_Don_Idle_Counter.setPrecondition( ACT_GUARD, true );
		ACT_ANI_DON_Don_Idle_Counter.setPrecondition( SIGNAL_CHANGE_ANIMATION, true );

		ACT_ANI_DON_Don_Idle_Counter.setEffect( ANI_DON_Don_Idle_Counter, true );
		ACT_ANI_DON_Don_Idle_Counter.setEffect( SIGNAL_CHANGE_ANIMATION, false );
		ACT_ANI_DON_Don_Idle_Counter.setEffect( SIGNAL_CHANGE_ANIMATION_FORCED, true );
		ACT_ANI_DON_Don_Idle_Counter.setEffect( SIGNAL_CHANGE_ANGLE_ONCE, true );
		ACT_ANI_DON_Don_Idle_Counter.setEffect( SIGNAL_SET_ANGLE_TARGET, true );
		ACT_ANI_DON_Don_Idle_Counter.setEffect( CH_STATE_BATTLE, true );
		ACT_ANI_DON_Don_Idle_Counter.setEffect( CH_STATE_GUARD, true );
		m_tInfo.m_vecAction_Ani.push_back( ACT_ANI_DON_Don_Idle_Counter );
	}

	// ~~~~~~~~~~~~~~~~~~~~~~ ACT_GUARD ~~~~~~~~~~~~~~~~~~~~~~

	// %%%%%%%%%%%%%%%%%%%%%%%%%% 애니메이션 결정 액션 %%%%%%%%%%%%%%%%%%%%%%%%%%


	return NOERROR;
}

HRESULT CPlayer::Ready_Leonardo(void)
{
	Engine::CComponent*			pComponent = nullptr;

	// For.Mesh
	pComponent = m_pMeshCom = (Engine::CMesh_Dynamic*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CMesh_Dynamic_Leonardo");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Mesh", pComponent));
	m_pMeshCom->AddRef();
	m_pMeshCom->Set_RootMotionFrame("Origin");
	lstrcpy(m_tInfo.m_StartAniName, L"ANI_LEO_TYPE_START");
	m_tInfo.m_StartAniNum = ANI_LEO_TYPE_START;
	m_tInfo.m_EndAniNum = ANI_LEO_TYPE_END;

	if (FAILED(Equip_Item(CSword::Create(m_pGraphicDev), m_pMeshCom->Find_FrameMatrixPointer("R_Sword1"))))
		return E_FAIL;

	if (FAILED(Equip_Item(CSword::Create(m_pGraphicDev), m_pMeshCom->Find_FrameMatrixPointer("L_Sword1"))))
		return E_FAIL;

	// -------------------------------- Hit 애니메이션을 차례대로 넣어준다.. -------------------------------- //
	{
		m_iHitAniArr[HitIndex_Front] = ANI_LEO_Leo_Dmg_F;
		m_iHitAniArr[HitIndex_Right] = ANI_LEO_Leo_Dmg_R;
		m_iHitAniArr[HitIndex_Back] = ANI_LEO_Leo_Dmg_B;
		m_iHitAniArr[HitIndex_Left] = ANI_LEO_Leo_Dmg_L;
	}

	// -------------------------------- 상호 작용 관련 정보 로드.. -------------------------------- //
	if ( FAILED( Ready_LoadInteractionData( L"../Bin/Data/InteractionData/Interaction_Leonard.dat" ) ) )
		return E_FAIL;

	// For.Collider - Add By.KMH
	Engine::CCollider* pCollider = (Engine::CCollider*)CComponent_Manager::GetInstance()->Clone_Component( SCENE_STAGE, L"CCollider" );
	if ( nullptr == pCollider )
		return E_FAIL;
	Ready_LoadCollider( L"../Bin/Data/ColliderData/Leo_Collider.Colldat", pCollider );
	Engine::Safe_Release( pCollider );

	/////////////////////////////// 콜리더 그룹 별로 매니저에 등록.. ///////////////////////////////
	{
		// 콜리더를 등록할 때는 공격 관련 콜리더는 플레이어, 몬스터 별로 같은 레이어에 등록해야되요..
		// 충돌 or 기타 등등의 콜리더는 생성되는 객체 당 하나 씩..

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
		m_tInfo.m_iMoveColliderGroupCount = LeoColGroup_Hit;	// Hit 용 콜리더를 캐릭터의 연산에 따라 갱신될 수 있도록 몇 번째 그룹인지 설정..
	}
	/////////////////////////////// 콜리더 그룹 별로 매니저에 등록 끝.. ///////////////////////////////


	// %%%%%%%%%%%%%%%%%%%%%%%%%% 캐릭터 월드 스테이트 변경 %%%%%%%%%%%%%%%%%%%%%%%%%%
	// ---- ACT ----
	m_tInfo.m_WorldState.setVariable(ACT_TYPE_START, ACT_TYPE_END, false);
	// ---- ANI_LEO ----
	m_tInfo.m_WorldState.setVariable(ANI_LEO_TYPE_START, ANI_LEO_TYPE_END, false);
	// ---- Signal ----
	m_tInfo.m_WorldState.setVariable(SIGNAL_TYPE_START, SIGNAL_TYPE_END, false);
	m_tInfo.m_WorldState.setVariable(CH_STATE_TYPE_START, CH_STATE_TYPE_END, false);
	m_tInfo.m_WorldState.setVariable(TARGET_TYPE_START, TARGET_TYPE_END, false);


	m_tInfo.m_WorldState.setVariable(SIGNAL_CHANGE_ANIMATION, true);
	m_tInfo.m_WorldState.setVariable(ACT_IDLE, true);
	m_tInfo.m_WorldState.setVariable(CH_STATE_PEACE, true);
	m_tInfo.m_WorldState.setVariable(CH_STATE_FOUND_ENEMY, false);
	m_tInfo.m_WorldState.setVariable(SIGNAL_CHANGE_TARGET_PLAYER, true);


	// %%%%%%%%%%%%%%%%%%%%%%%%%% 시그널 처리 액션 시작 %%%%%%%%%%%%%%%%%%%%%%%%%%
	// ----- 애니메이션 강제 변환 -----
	CAction ACT_SIGNAL_CHANGE_ANIMATION_FORCED("SIGNAL_CHANGE_ANIMATION_FORCED", 1);
	ACT_SIGNAL_CHANGE_ANIMATION_FORCED.setPrecondition(SIGNAL_CHANGE_ANIMATION_FORCED, true);

	ACT_SIGNAL_CHANGE_ANIMATION_FORCED.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	ACT_SIGNAL_CHANGE_ANIMATION_FORCED.setEffect(SIGNAL_CHANGE_ANIMATION_FORCED, false);
	m_tInfo.m_vecAction_Ani.push_back(ACT_SIGNAL_CHANGE_ANIMATION_FORCED);
	//// ----- 애니메이션 강제 변환 -----
	//// ----- 무빙 종료 -----
	//CAction ACT_WALK_EXIT
	//// ----- 무빙 종료 -----
	// %%%%%%%%%%%%%%%%%%%%%%%%%% 시그널 처리 액션 끝 %%%%%%%%%%%%%%%%%%%%%%%%%%


	// %%%%%%%%%%%%%%%%%%%%%%%%%% 행동 결정 액션 %%%%%%%%%%%%%%%%%%%%%%%%%%

	CAction AI_ACT_Idle("AI_ACT_Idle", 300);
	AI_ACT_Idle.setPrecondition(CH_STATE_FOUND_ENEMY, false);
	AI_ACT_Idle.setPrecondition(CH_STATE_NEAR_PLAYER, true);

	AI_ACT_Idle.setEffect(SIGNAL_CHANGE_TARGET_PLAYER, true);
	AI_ACT_Idle.setEffect(CH_STATE_PEACE, true);
	AI_ACT_Idle.setEffect(ACT_IDLE, true);
	m_tInfo.m_vecAction_Act.push_back(AI_ACT_Idle);


	CAction AI_ACT_Found_Target("AI_ACT_Found_Target", 1800);
	AI_ACT_Found_Target.setPrecondition(CH_STATE_FOUND_ENEMY, true);

	AI_ACT_Found_Target.setEffect(SIGNAL_CHANGE_TARGET_NEAR_ENEMY, true);
	AI_ACT_Found_Target.setEffect(ACT_IDLE, true);
	m_tInfo.m_vecAction_Act.push_back(AI_ACT_Found_Target);


	CAction AI_ACT_Found_Enemy("AI_ACT_Found_Target", 2000);
	AI_ACT_Found_Enemy.setPrecondition(TARGET_NONE, true);

	AI_ACT_Found_Enemy.setEffect(SIGNAL_CHANGE_TARGET_NEAR_ENEMY, true);
	AI_ACT_Found_Enemy.setEffect(ACT_IDLE, true);
	m_tInfo.m_vecAction_Act.push_back(AI_ACT_Found_Enemy);

	CAction AI_ACT_Found_Target_Battle("AI_ACT_Found_Target_Battle", 1900);
	AI_ACT_Found_Target_Battle.setPrecondition(TARGET_NONE, true);
	AI_ACT_Found_Target_Battle.setPrecondition(CH_STATE_BATTLE, true);

	AI_ACT_Found_Target_Battle.setEffect(SIGNAL_CHANGE_TARGET_NEAR_ENEMY, true);
	AI_ACT_Found_Target_Battle.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
	AI_ACT_Found_Target_Battle.setEffect(SIGNAL_CHANGE_ANGLE, true);
	AI_ACT_Found_Target_Battle.setEffect(SIGNAL_CHANGE_ANIMATION_FORCED, true);
	AI_ACT_Found_Target_Battle.setEffect(ACT_IDLE, true);
	m_tInfo.m_vecAction_Act.push_back(AI_ACT_Found_Target_Battle);

	CAction AI_ACT_Found_Target_Peace("AI_ACT_Found_Target_Peace", 1800);
	AI_ACT_Found_Target_Peace.setPrecondition(TARGET_NONE, true);
	AI_ACT_Found_Target_Peace.setPrecondition(CH_STATE_TOOFAR_NEAR_ENEMY, false);

	AI_ACT_Found_Target_Peace.setEffect(SIGNAL_CHANGE_TARGET_PLAYER, true);
	AI_ACT_Found_Target_Peace.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
	AI_ACT_Found_Target_Peace.setEffect(SIGNAL_CHANGE_ANGLE, true);
	AI_ACT_Found_Target_Peace.setEffect(SIGNAL_CHANGE_ANIMATION_FORCED, true);
	AI_ACT_Found_Target_Peace.setEffect(ACT_IDLE, true);
	m_tInfo.m_vecAction_Act.push_back(AI_ACT_Found_Target_Peace);


	CAction AI_ACT_RUN_TO_PLAYER("AI_ACT_RUN_TO_PLAYER", 200);
	//AI_ACT_RUN_TO_PLAYER.setPrecondition(CH_STATE_PEACE, true);
	AI_ACT_RUN_TO_PLAYER.setPrecondition(CH_STATE_FOUND_ENEMY, false);
	AI_ACT_RUN_TO_PLAYER.setPrecondition(CH_STATE_NEAR_PLAYER, false);

	//AI_ACT_RUN_TO_PLAYER.setEffect(CH_STATE_NEAR_PLAYER, true);
	//AI_ACT_RUN_TO_PLAYER.setEffect(SIGNAL_MOVE_TARGET, true);
	AI_ACT_RUN_TO_PLAYER.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
	AI_ACT_RUN_TO_PLAYER.setEffect(SIGNAL_CHANGE_ANGLE, true);
	AI_ACT_RUN_TO_PLAYER.setEffect(SIGNAL_CHANGE_TARGET_PLAYER, true);
	AI_ACT_RUN_TO_PLAYER.setEffect(ACT_RUN, true);
	m_tInfo.m_vecAction_Act.push_back(AI_ACT_RUN_TO_PLAYER);

	CAction AI_ACT_RUN_TO_ENEMY("AI_ACT_RUN_TO_ENEMY", 195);
	AI_ACT_RUN_TO_ENEMY.setPrecondition(CH_STATE_FOUND_ENEMY, true);
	AI_ACT_RUN_TO_ENEMY.setPrecondition(CH_STATE_CLOSE_TARGET, false);
	AI_ACT_RUN_TO_ENEMY.setPrecondition(TARGET_PLAYER, false);		

	AI_ACT_RUN_TO_ENEMY.setEffect(SIGNAL_CHANGE_TARGET_NEAR_ENEMY, true);
	AI_ACT_RUN_TO_ENEMY.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
	AI_ACT_RUN_TO_ENEMY.setEffect(SIGNAL_CHANGE_ANGLE, true);
	AI_ACT_RUN_TO_ENEMY.setEffect(ACT_RUN, true);
	m_tInfo.m_vecAction_Act.push_back(AI_ACT_RUN_TO_ENEMY);

	CAction AI_ACT_ATTACK("AI_ACT_ATTACK", 100);
	AI_ACT_ATTACK.setPrecondition(CH_STATE_FOUND_ENEMY, true);	// 조건 : 적발견 T
	AI_ACT_ATTACK.setPrecondition(TARGET_NEAR_ENEMY, true);		// 타겟 : 근처적 T
	AI_ACT_ATTACK.setPrecondition(CH_STATE_CLOSE_TARGET, true);	// 범위 : 바로옆 T

	//AI_ACT_ATTACK.setEffect(TARGET_NEAR_ENEMY, true);
	//AI_ACT_ATTACK.setEffect(CH_STATE_ATTACK, true);
	AI_ACT_ATTACK.setEffect(ACT_ATTACK, true);
	m_tInfo.m_vecAction_Act.push_back(AI_ACT_ATTACK);


	// 적에게 카운터 어택을 하겠다..
	{
		CAction AI_ACT_COUNTERATTACK( "AI_ACT_COUNTERATTACK", 1 );
		AI_ACT_COUNTERATTACK.setPrecondition( SIGNAL_ACT_INTERACTION, true );

		AI_ACT_COUNTERATTACK.setEffect( ACT_INTERACTION, true );
		AI_ACT_COUNTERATTACK.setEffect( SIGNAL_ACT_INTERACTION, false );
		m_tInfo.m_vecAction_Act.push_back( AI_ACT_COUNTERATTACK );
	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%% 행동 결정 액션 %%%%%%%%%%%%%%%%%%%%%%%%%%


	// %%%%%%%%%%%%%%%%%%%%%%%%%% 애니메이션 결정 액션 %%%%%%%%%%%%%%%%%%%%%%%%%%
	// ----- 기본 대기 상태 -----
	CAction ACT_ANI_LEO_Leo_Idle_Pose("ANI_LEO_Leo_Idle", 200);
	ACT_ANI_LEO_Leo_Idle_Pose.setPrecondition(ACT_IDLE, true);
	ACT_ANI_LEO_Leo_Idle_Pose.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_LEO_Leo_Idle_Pose.setEffect(ANI_LEO_Leo_Idle, true);
	ACT_ANI_LEO_Leo_Idle_Pose.setEffect(SIGNAL_CHANGE_ANIMATION_FORCED, true);
	ACT_ANI_LEO_Leo_Idle_Pose.setEffect(SIGNAL_CHANGE_ANGLE, false);
	ACT_ANI_LEO_Leo_Idle_Pose.setEffect(CH_STATE_IDLE, true);
	ACT_ANI_LEO_Leo_Idle_Pose.setEffect(CH_STATE_PEACE, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_LEO_Leo_Idle_Pose);

	// ----- 달리기 -----
	CAction ACT_ANI_LEO_Leo_Move_Run_F("ANI_LEO_Leo_Move_Run_F", 90);
	ACT_ANI_LEO_Leo_Move_Run_F.setPrecondition(ACT_RUN, true);
	ACT_ANI_LEO_Leo_Move_Run_F.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_LEO_Leo_Move_Run_F.setEffect(ANI_LEO_Leo_Move_Run_F, true);
	ACT_ANI_LEO_Leo_Move_Run_F.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
	ACT_ANI_LEO_Leo_Move_Run_F.setEffect(SIGNAL_CHANGE_ANIMATION_FORCED, true);
	ACT_ANI_LEO_Leo_Move_Run_F.setEffect(SIGNAL_CHANGE_ANGLE, true);
	ACT_ANI_LEO_Leo_Move_Run_F.setEffect(CH_STATE_RUN, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_LEO_Leo_Move_Run_F);

	// ~~~~~~~~~~~~~~~~~~~~~~ ACT_ATTACK ~~~~~~~~~~~~~~~~~~~~~~
	// ----- 기본 공격1 -----
	CAction ACT_ANI_LEO_Leo_Melee_01("ANI_LEO_Leo_Melee_01", 80);
	ACT_ANI_LEO_Leo_Melee_01.setPrecondition(ACT_ATTACK, true);
	ACT_ANI_LEO_Leo_Melee_01.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_LEO_Leo_Melee_01.setEffect(ANI_LEO_Leo_Melee_01, true);
	ACT_ANI_LEO_Leo_Melee_01.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	ACT_ANI_LEO_Leo_Melee_01.setEffect(SIGNAL_CHANGE_ANGLE_ONCE, true);
	ACT_ANI_LEO_Leo_Melee_01.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
	ACT_ANI_LEO_Leo_Melee_01.setEffect(TARGET_NEAR_ENEMY, true);
	ACT_ANI_LEO_Leo_Melee_01.setEffect(CH_STATE_BATTLE, true);
	ACT_ANI_LEO_Leo_Melee_01.setEffect(CH_STATE_ATTACK, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_LEO_Leo_Melee_01);
	
	// ----- 기본 공격2 -----
	CAction ACT_ANI_LEO_Leo_Melee_02("ANI_LEO_Leo_Melee_02", 79);
	ACT_ANI_LEO_Leo_Melee_02.setPrecondition(ACT_ATTACK, true);
	ACT_ANI_LEO_Leo_Melee_02.setPrecondition(ANI_LEO_Leo_Melee_01, true);
	ACT_ANI_LEO_Leo_Melee_02.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_LEO_Leo_Melee_02.setEffect(ANI_LEO_Leo_Melee_02, true);
	ACT_ANI_LEO_Leo_Melee_02.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	ACT_ANI_LEO_Leo_Melee_02.setEffect(SIGNAL_CHANGE_ANGLE_ONCE, true);
	ACT_ANI_LEO_Leo_Melee_02.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
	ACT_ANI_LEO_Leo_Melee_02.setEffect(TARGET_NEAR_ENEMY, true);
	ACT_ANI_LEO_Leo_Melee_02.setEffect(CH_STATE_BATTLE, true);
	ACT_ANI_LEO_Leo_Melee_02.setEffect(CH_STATE_ATTACK, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_LEO_Leo_Melee_02);

	// ----- 기본 공격3 -----
	CAction ACT_ANI_LEO_Leo_Melee_03("ANI_LEO_Leo_Melee_03", 79);
	ACT_ANI_LEO_Leo_Melee_03.setPrecondition(ACT_ATTACK, true);
	ACT_ANI_LEO_Leo_Melee_03.setPrecondition(ANI_LEO_Leo_Melee_02, true);
	ACT_ANI_LEO_Leo_Melee_03.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_LEO_Leo_Melee_03.setEffect(ANI_LEO_Leo_Melee_03, true);
	ACT_ANI_LEO_Leo_Melee_03.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	ACT_ANI_LEO_Leo_Melee_03.setEffect(SIGNAL_CHANGE_ANGLE_ONCE, true);
	ACT_ANI_LEO_Leo_Melee_03.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
	ACT_ANI_LEO_Leo_Melee_03.setEffect(TARGET_NEAR_ENEMY, true);
	ACT_ANI_LEO_Leo_Melee_03.setEffect(CH_STATE_BATTLE, true);
	ACT_ANI_LEO_Leo_Melee_03.setEffect(CH_STATE_ATTACK, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_LEO_Leo_Melee_03);
	// ----- 기본 공격4 -----
	CAction ACT_ANI_LEO_Leo_Melee_04("ANI_LEO_Leo_Melee_04", 79);
	ACT_ANI_LEO_Leo_Melee_04.setPrecondition(ACT_ATTACK, true);
	ACT_ANI_LEO_Leo_Melee_04.setPrecondition(ANI_LEO_Leo_Melee_03, true);
	ACT_ANI_LEO_Leo_Melee_04.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_LEO_Leo_Melee_04.setEffect(ANI_LEO_Leo_Melee_04, true);
	ACT_ANI_LEO_Leo_Melee_04.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	ACT_ANI_LEO_Leo_Melee_04.setEffect(SIGNAL_CHANGE_ANGLE_ONCE, true);
	ACT_ANI_LEO_Leo_Melee_04.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
	ACT_ANI_LEO_Leo_Melee_04.setEffect(TARGET_NEAR_ENEMY, true);
	ACT_ANI_LEO_Leo_Melee_04.setEffect(CH_STATE_BATTLE, true);
	ACT_ANI_LEO_Leo_Melee_04.setEffect(CH_STATE_ATTACK, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_LEO_Leo_Melee_04);
	// ~~~~~~~~~~~~~~~~~~~~~~ ACT_ATTACK ~~~~~~~~~~~~~~~~~~~~~~

	// ~~~~~~~~~~~~~~~~~~~~~~ ACT_KICK ~~~~~~~~~~~~~~~~~~~~~~
	// ----- 발차기 공격1 -----
	CAction ACT_ANI_LEO_Leo_Kick_01("ANI_LEO_Leo_Kick_01", 80);
	ACT_ANI_LEO_Leo_Kick_01.setPrecondition(ACT_KICK, true);
	ACT_ANI_LEO_Leo_Kick_01.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_LEO_Leo_Kick_01.setEffect(ANI_LEO_Leo_Kick_01, true);
	ACT_ANI_LEO_Leo_Kick_01.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	ACT_ANI_LEO_Leo_Kick_01.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_LEO_Leo_Kick_01);
	// ----- 발차기 공격2 -----
	CAction ACT_ANI_LEO_Leo_Kick_02("ANI_LEO_Leo_Kick_02_b", 79);
	ACT_ANI_LEO_Leo_Kick_02.setPrecondition(ACT_KICK, true);
	ACT_ANI_LEO_Leo_Kick_02.setPrecondition(ANI_LEO_Leo_Kick_01, true);
	ACT_ANI_LEO_Leo_Kick_02.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_LEO_Leo_Kick_02.setEffect(ANI_LEO_Leo_Kick_02_b, true);
	ACT_ANI_LEO_Leo_Kick_02.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	ACT_ANI_LEO_Leo_Kick_02.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_LEO_Leo_Kick_02);
	// ----- 발차기 공격3 -----
	CAction ACT_ANI_LEO_Leo_Kick_03("ANI_LEO_Leo_Kick_03", 79);
	ACT_ANI_LEO_Leo_Kick_03.setPrecondition(ACT_KICK, true);
	ACT_ANI_LEO_Leo_Kick_03.setPrecondition(ANI_LEO_Leo_Kick_02_b, true);
	ACT_ANI_LEO_Leo_Kick_03.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_LEO_Leo_Kick_03.setEffect(ANI_LEO_Leo_Kick_03, true);
	ACT_ANI_LEO_Leo_Kick_03.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	ACT_ANI_LEO_Leo_Kick_03.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_LEO_Leo_Kick_03);
	// ----- 발차기 공격4 -----
	CAction ACT_ANI_LEO_Leo_Kick_04("ANI_LEO_Leo_Kick_04_b", 79);
	ACT_ANI_LEO_Leo_Kick_04.setPrecondition(ACT_KICK, true);
	ACT_ANI_LEO_Leo_Kick_04.setPrecondition(ANI_LEO_Leo_Kick_03, true);
	ACT_ANI_LEO_Leo_Kick_04.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_LEO_Leo_Kick_04.setEffect(ANI_LEO_Leo_Kick_04_b, true);
	ACT_ANI_LEO_Leo_Kick_04.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	ACT_ANI_LEO_Leo_Kick_04.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_LEO_Leo_Kick_04);

	// ~~~~~~~~~~~~~~~~~~~~~~ ACT_KICK ~~~~~~~~~~~~~~~~~~~~~~

	// ~~~~~~~~~~~~~~~~~~~~~~ ACT_COUNTERATTACK ~~~~~~~~~~~~~~~~~~~~~~

	// ----- 앞의 적 카운터 공격.. -----
	{
		CAction ACT_ANI_LEO_Leo_Counter_F2F( "ANI_LEO_Leo_Counter_F2F", 80 );
		ACT_ANI_LEO_Leo_Counter_F2F.setPrecondition( ACT_INTERACTION, true );
		ACT_ANI_LEO_Leo_Counter_F2F.setPrecondition( SIGNAL_CHANGE_ANIMATION, true );
		ACT_ANI_LEO_Leo_Counter_F2F.setPrecondition( SIGNAL_INTERACTION_COUNTER_FRONT1, true );

		ACT_ANI_LEO_Leo_Counter_F2F.setEffect( SIGNAL_INTERACTION_COUNTER_FRONT1, false );
		ACT_ANI_LEO_Leo_Counter_F2F.setEffect( ANI_LEO_Leo_Counter_F2F, true );
		ACT_ANI_LEO_Leo_Counter_F2F.setEffect( SIGNAL_CHANGE_ANIMATION, false );
		ACT_ANI_LEO_Leo_Counter_F2F.setEffect( SIGNAL_CHANGE_ANGLE_ONCE, true );
		ACT_ANI_LEO_Leo_Counter_F2F.setEffect( SIGNAL_SET_ANGLE_TARGET, true );
		ACT_ANI_LEO_Leo_Counter_F2F.setEffect( CH_STATE_BATTLE, true );
		ACT_ANI_LEO_Leo_Counter_F2F.setEffect( CH_STATE_INTERACTION, true );
		m_tInfo.m_vecAction_Ani.push_back( ACT_ANI_LEO_Leo_Counter_F2F );
	}

	// ----- 뒤의 적 카운터 공격.. -----
	{
		CAction ACT_ANI_LEO_Leo_Counter_B2F_Upg( "ANI_LEO_Leo_Counter_B2F_Upg", 80 );
		ACT_ANI_LEO_Leo_Counter_B2F_Upg.setPrecondition( ACT_INTERACTION, true );
		ACT_ANI_LEO_Leo_Counter_B2F_Upg.setPrecondition( SIGNAL_CHANGE_ANIMATION, true );
		ACT_ANI_LEO_Leo_Counter_B2F_Upg.setPrecondition( SIGNAL_INTERACTION_COUNTER_BACK1, true );

		ACT_ANI_LEO_Leo_Counter_B2F_Upg.setEffect( SIGNAL_INTERACTION_COUNTER_BACK1, false );
		ACT_ANI_LEO_Leo_Counter_B2F_Upg.setEffect( ANI_LEO_Leo_Counter_B2F_Upg, true );
		ACT_ANI_LEO_Leo_Counter_B2F_Upg.setEffect( SIGNAL_CHANGE_ANIMATION, false );
		ACT_ANI_LEO_Leo_Counter_B2F_Upg.setEffect( SIGNAL_CHANGE_ANGLE_ONCE, true );
		ACT_ANI_LEO_Leo_Counter_B2F_Upg.setEffect( SIGNAL_SET_ANGLE_TARGET, true );
		ACT_ANI_LEO_Leo_Counter_B2F_Upg.setEffect( CH_STATE_BATTLE, true );
		ACT_ANI_LEO_Leo_Counter_B2F_Upg.setEffect( CH_STATE_INTERACTION, true );
		m_tInfo.m_vecAction_Ani.push_back( ACT_ANI_LEO_Leo_Counter_B2F_Upg );
	}

	// ~~~~~~~~~~~~~~~~~~~~~~ ACT_COUNTERATTACK ~~~~~~~~~~~~~~~~~~~~~~

	// %%%%%%%%%%%%%%%%%%%%%%%%%% 애니메이션 결정 액션 %%%%%%%%%%%%%%%%%%%%%%%%%%

	return NOERROR;

}

HRESULT CPlayer::Ready_Michelangelo(void)
{
	Engine::CComponent*			pComponent = nullptr;

	// For.Mesh
	pComponent = m_pMeshCom = (Engine::CMesh_Dynamic*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CMesh_Dynamic_Michelangelo");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Mesh", pComponent));
	m_pMeshCom->AddRef();
	m_pMeshCom->Set_RootMotionFrame("Origin");
	lstrcpy(m_tInfo.m_StartAniName, L"ANI_MIKE_TYPE_START");
	m_tInfo.m_StartAniNum = ANI_MIKE_TYPE_START;
	m_tInfo.m_EndAniNum = ANI_MIKE_TYPE_END;


	// -------------------------------- Hit 애니메이션을 차례대로 넣어준다.. -------------------------------- //
	{
		m_iHitAniArr[HitIndex_Front] = ANI_MIKE_Mike_Dmg_F;
		m_iHitAniArr[HitIndex_Right] = ANI_MIKE_Mike_Dmg_R;
		m_iHitAniArr[HitIndex_Back] = ANI_MIKE_Mike_Dmg_B;
		m_iHitAniArr[HitIndex_Left] = ANI_MIKE_Mike_Dmg_L;
	}

	// -------------------------------- 상호 작용 관련 정보 로드.. -------------------------------- //
	if ( FAILED( Ready_LoadInteractionData( L"../Bin/Data/InteractionData/Interaction_Michelangelo.dat" ) ) )
		return E_FAIL;

	// For.Collider - Add By.KMH
	Engine::CCollider* pCollider = (Engine::CCollider*)CComponent_Manager::GetInstance()->Clone_Component( SCENE_STAGE, L"CCollider" );
	if ( nullptr == pCollider )
		return E_FAIL;
	Ready_LoadCollider( L"../Bin/Data/ColliderData/Mike_Collider.Colldat", pCollider );
	Engine::Safe_Release( pCollider );

	/////////////////////////////// 콜리더 그룹 별로 매니저에 등록.. ///////////////////////////////
	{
		// 콜리더를 등록할 때는 공격 관련 콜리더는 플레이어, 몬스터 별로 같은 레이어에 등록해야되요..
		// 충돌 or 기타 등등의 콜리더는 생성되는 객체 당 하나 씩..

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
		m_tInfo.m_iMoveColliderGroupCount = MikeColGroup_Hit;	// Hit 용 콜리더를 캐릭터의 연산에 따라 갱신될 수 있도록 몇 번째 그룹인지 설정..
	}
	/////////////////////////////// 콜리더 그룹 별로 매니저에 등록 끝.. ///////////////////////////////

	//if (FAILED(Equip_Item(CChucks::Create(m_pGraphicDev), m_pMeshCom->Find_FrameMatrixPointer("R_Chuck"))))
	//	return E_FAIL;

	//if (FAILED(Equip_Item(CChucks::Create(m_pGraphicDev), m_pMeshCom->Find_FrameMatrixPointer("L_Chuck"))))
	//	return E_FAIL;

	
	// %%%%%%%%%%%%%%%%%%%%%%%%%% 캐릭터 월드 스테이트 변경 %%%%%%%%%%%%%%%%%%%%%%%%%%
	// ---- ACT ----
	m_tInfo.m_WorldState.setVariable(ACT_TYPE_START, ACT_TYPE_END, false);
	// ---- ANI_MIKE ----
	m_tInfo.m_WorldState.setVariable(ANI_MIKE_TYPE_START, ANI_MIKE_TYPE_END, false);
	// ---- Signal ----
	m_tInfo.m_WorldState.setVariable(SIGNAL_TYPE_START, SIGNAL_TYPE_END, false);
	m_tInfo.m_WorldState.setVariable(CH_STATE_TYPE_START, CH_STATE_TYPE_END, false);
	m_tInfo.m_WorldState.setVariable(TARGET_TYPE_START, TARGET_TYPE_END, false);


	m_tInfo.m_WorldState.setVariable(SIGNAL_CHANGE_ANIMATION, true);
	m_tInfo.m_WorldState.setVariable(ACT_IDLE, true);
	m_tInfo.m_WorldState.setVariable(CH_STATE_PEACE, true);
	m_tInfo.m_WorldState.setVariable(TARGET_PLAYER, true);


	// %%%%%%%%%%%%%%%%%%%%%%%%%% 시그널 처리 액션 %%%%%%%%%%%%%%%%%%%%%%%%%%
	// ----- 애니메이션 강제 변환 -----
	CAction ACT_SIGNAL_CHANGE_ANIMATION_FORCED("SIGNAL_CHANGE_ANIMATION_FORCED", 1);
	ACT_SIGNAL_CHANGE_ANIMATION_FORCED.setPrecondition(SIGNAL_CHANGE_ANIMATION_FORCED, true);

	ACT_SIGNAL_CHANGE_ANIMATION_FORCED.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	ACT_SIGNAL_CHANGE_ANIMATION_FORCED.setEffect(SIGNAL_CHANGE_ANIMATION_FORCED, false);
	m_tInfo.m_vecAction_Ani.push_back(ACT_SIGNAL_CHANGE_ANIMATION_FORCED);
	//// ----- 애니메이션 강제 변환 -----
	//// ----- 무빙 종료 -----
	//CAction ACT_WALK_EXIT
	//// ----- 무빙 종료 -----
	// %%%%%%%%%%%%%%%%%%%%%%%%%% 시그널 처리 액션 %%%%%%%%%%%%%%%%%%%%%%%%%%


	// %%%%%%%%%%%%%%%%%%%%%%%%%% 행동 결정 액션 %%%%%%%%%%%%%%%%%%%%%%%%%%

	CAction AI_ACT_Idle("AI_ACT_Idle", 300);
	//ACT_ANI_DON_Don_Idle_Pose.setPrecondition(ACT_WAIT, true);
	AI_ACT_Idle.setPrecondition(CH_STATE_FOUND_ENEMY, false);
	AI_ACT_Idle.setPrecondition(CH_STATE_NEAR_PLAYER, true);

	AI_ACT_Idle.setEffect(ACT_IDLE, true);
	m_tInfo.m_vecAction_Act.push_back(AI_ACT_Idle);

	CAction AI_ACT_RUN_TO_PLAYER("AI_ACT_RUN_TO_PLAYER", 200);
	//AI_ACT_RUN_TO_PLAYER.setPrecondition(CH_STATE_PEACE, true);
	AI_ACT_RUN_TO_PLAYER.setPrecondition(CH_STATE_FOUND_ENEMY, false);
	AI_ACT_RUN_TO_PLAYER.setPrecondition(CH_STATE_NEAR_PLAYER, false);

	//AI_ACT_RUN_TO_PLAYER.setEffect(CH_STATE_NEAR_PLAYER, true);
	//AI_ACT_RUN_TO_PLAYER.setEffect(SIGNAL_MOVE_TARGET, true);
	AI_ACT_RUN_TO_PLAYER.setEffect(TARGET_PLAYER, true);
	AI_ACT_RUN_TO_PLAYER.setEffect(ACT_RUN, true);
	m_tInfo.m_vecAction_Act.push_back(AI_ACT_RUN_TO_PLAYER);

	// 적에게 카운터 어택을 하겠다..
	{
		CAction AI_ACT_COUNTERATTACK( "AI_ACT_COUNTERATTACK", 1 );
		AI_ACT_COUNTERATTACK.setPrecondition( SIGNAL_ACT_INTERACTION, true );

		AI_ACT_COUNTERATTACK.setEffect( ACT_INTERACTION, true );
		AI_ACT_COUNTERATTACK.setEffect( SIGNAL_ACT_INTERACTION, false );
		m_tInfo.m_vecAction_Act.push_back( AI_ACT_COUNTERATTACK );
	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%% 행동 결정 액션 %%%%%%%%%%%%%%%%%%%%%%%%%%


	// %%%%%%%%%%%%%%%%%%%%%%%%%% 애니메이션 결정 액션 %%%%%%%%%%%%%%%%%%%%%%%%%%
	// ----- 기본 대기 상태 -----
	CAction ACT_ANI_MIKE_Mike_Idle("ANI_MIKE_Mike_Idle", 200);
	ACT_ANI_MIKE_Mike_Idle.setPrecondition(ACT_IDLE, true);
	ACT_ANI_MIKE_Mike_Idle.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_MIKE_Mike_Idle.setEffect(ANI_MIKE_Mike_Idle, true);
	ACT_ANI_MIKE_Mike_Idle.setEffect(SIGNAL_CHANGE_ANIMATION_FORCED, true);
	ACT_ANI_MIKE_Mike_Idle.setEffect(SIGNAL_CHANGE_ANGLE, false);
	ACT_ANI_MIKE_Mike_Idle.setEffect(CH_STATE_IDLE, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_MIKE_Mike_Idle);

	// ----- 달리기 -----
	CAction ACT_ANI_MIKE_Mike_Move_Run_F("ANI_MIKE_Mike_Move_Run_F", 90);
	ACT_ANI_MIKE_Mike_Move_Run_F.setPrecondition(ACT_RUN, true);
	ACT_ANI_MIKE_Mike_Move_Run_F.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_MIKE_Mike_Move_Run_F.setEffect(ANI_MIKE_Mike_Move_Run_F, true);
	ACT_ANI_MIKE_Mike_Move_Run_F.setEffect(SIGNAL_CHANGE_ANIMATION_FORCED, true);
	ACT_ANI_MIKE_Mike_Move_Run_F.setEffect(SIGNAL_CHANGE_ANGLE, true);
	ACT_ANI_MIKE_Mike_Move_Run_F.setEffect(CH_STATE_RUN, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_MIKE_Mike_Move_Run_F);

	// ~~~~~~~~~~~~~~~~~~~~~~ ACT_ATTACK ~~~~~~~~~~~~~~~~~~~~~~
	// ----- 기본 공격1 -----
	CAction ACT_ANI_MIKE_Mike_Melee_01("ANI_MIKE_Mike_Melee_01", 80);
	ACT_ANI_MIKE_Mike_Melee_01.setPrecondition(ACT_ATTACK, true);
	ACT_ANI_MIKE_Mike_Melee_01.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_MIKE_Mike_Melee_01.setEffect(ANI_MIKE_Mike_Melee_01, true);
	ACT_ANI_MIKE_Mike_Melee_01.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	ACT_ANI_MIKE_Mike_Melee_01.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_MIKE_Mike_Melee_01);
	// ----- 기본 공격2 -----
	CAction ACT_ANI_MIKE_Mike_Melee_02("ANI_MIKE_Mike_Melee_02", 79);
	ACT_ANI_MIKE_Mike_Melee_02.setPrecondition(ACT_ATTACK, true);
	ACT_ANI_MIKE_Mike_Melee_02.setPrecondition(ANI_MIKE_Mike_Melee_01, true);
	ACT_ANI_MIKE_Mike_Melee_02.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_MIKE_Mike_Melee_02.setEffect(ANI_MIKE_Mike_Melee_02, true);
	ACT_ANI_MIKE_Mike_Melee_02.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	ACT_ANI_MIKE_Mike_Melee_02.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_MIKE_Mike_Melee_02);
	// ----- 기본 공격3 -----
	CAction ACT_ANI_MIKE_Mike_Melee_03("ANI_MIKE_Mike_Melee_03", 79);
	ACT_ANI_MIKE_Mike_Melee_03.setPrecondition(ACT_ATTACK, true);
	ACT_ANI_MIKE_Mike_Melee_03.setPrecondition(ANI_MIKE_Mike_Melee_02, true);
	ACT_ANI_MIKE_Mike_Melee_03.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_MIKE_Mike_Melee_03.setEffect(ANI_MIKE_Mike_Melee_03, true);
	ACT_ANI_MIKE_Mike_Melee_03.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	ACT_ANI_MIKE_Mike_Melee_03.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_MIKE_Mike_Melee_03);
	// ----- 기본 공격4 -----
	CAction ACT_ANI_MIKE_Mike_Melee_04("ANI_MIKE_Mike_Melee_04", 79);
	ACT_ANI_MIKE_Mike_Melee_04.setPrecondition(ACT_ATTACK, true);
	ACT_ANI_MIKE_Mike_Melee_04.setPrecondition(ANI_MIKE_Mike_Melee_03, true);
	ACT_ANI_MIKE_Mike_Melee_04.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_MIKE_Mike_Melee_04.setEffect(ANI_MIKE_Mike_Melee_04, true);
	ACT_ANI_MIKE_Mike_Melee_04.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	ACT_ANI_MIKE_Mike_Melee_04.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_MIKE_Mike_Melee_04);

	// ----- 기본 공격5 -----
	CAction ACT_ANI_MIKE_Mike_Melee_05("ANI_MIKE_Mike_Melee_05", 79);
	ACT_ANI_MIKE_Mike_Melee_05.setPrecondition(ACT_ATTACK, true);
	ACT_ANI_MIKE_Mike_Melee_05.setPrecondition(ANI_MIKE_Mike_Melee_04, true);
	ACT_ANI_MIKE_Mike_Melee_05.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_MIKE_Mike_Melee_05.setEffect(ANI_MIKE_Mike_Melee_05, true);
	ACT_ANI_MIKE_Mike_Melee_05.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	ACT_ANI_MIKE_Mike_Melee_05.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_MIKE_Mike_Melee_05);
	// ~~~~~~~~~~~~~~~~~~~~~~ ACT_ATTACK ~~~~~~~~~~~~~~~~~~~~~~

	// ~~~~~~~~~~~~~~~~~~~~~~ ACT_KICK ~~~~~~~~~~~~~~~~~~~~~~
	// ----- 발차기 공격1 -----
	CAction ACT_ANI_MIKE_Mike_Kick_01("ANI_MIKE_Mike_Kick_01", 80);
	ACT_ANI_MIKE_Mike_Kick_01.setPrecondition(ACT_KICK, true);
	ACT_ANI_MIKE_Mike_Kick_01.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_MIKE_Mike_Kick_01.setEffect(ANI_MIKE_Mike_Kick_01, true);
	ACT_ANI_MIKE_Mike_Kick_01.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	ACT_ANI_MIKE_Mike_Kick_01.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_MIKE_Mike_Kick_01);
	// ----- 발차기 공격2 -----
	CAction ACT_ANI_MIKE_Mike_Kick_03("ANI_MIKE_Mike_Kick_03", 79);
	ACT_ANI_MIKE_Mike_Kick_03.setPrecondition(ACT_KICK, true);
	ACT_ANI_MIKE_Mike_Kick_03.setPrecondition(ANI_MIKE_Mike_Kick_01, true);
	ACT_ANI_MIKE_Mike_Kick_03.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_MIKE_Mike_Kick_03.setEffect(ANI_MIKE_Mike_Kick_03, true);
	ACT_ANI_MIKE_Mike_Kick_03.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	ACT_ANI_MIKE_Mike_Kick_03.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_MIKE_Mike_Kick_03);
	// ----- 발차기 공격3 -----
	CAction ACT_ANI_MIKE_Mike_Kick_04("ANI_MIKE_Mike_Kick_04", 79);
	ACT_ANI_MIKE_Mike_Kick_04.setPrecondition(ACT_KICK, true);
	ACT_ANI_MIKE_Mike_Kick_04.setPrecondition(ANI_MIKE_Mike_Kick_03, true);
	ACT_ANI_MIKE_Mike_Kick_04.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_MIKE_Mike_Kick_04.setEffect(ANI_MIKE_Mike_Kick_04, true);
	ACT_ANI_MIKE_Mike_Kick_04.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	ACT_ANI_MIKE_Mike_Kick_04.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_MIKE_Mike_Kick_04);
	// ----- 발차기 공격4 -----
	CAction ACT_ANI_MIKE_Mike_Kick_04_c("ANI_MIKE_Mike_Kick_04_c", 79);
	ACT_ANI_MIKE_Mike_Kick_04_c.setPrecondition(ACT_KICK, true);
	ACT_ANI_MIKE_Mike_Kick_04_c.setPrecondition(ANI_MIKE_Mike_Kick_04, true);
	ACT_ANI_MIKE_Mike_Kick_04_c.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_MIKE_Mike_Kick_04_c.setEffect(ANI_MIKE_Mike_Kick_04_c, true);
	ACT_ANI_MIKE_Mike_Kick_04_c.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	ACT_ANI_MIKE_Mike_Kick_04_c.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_MIKE_Mike_Kick_04_c);

	// ~~~~~~~~~~~~~~~~~~~~~~ ACT_KICK ~~~~~~~~~~~~~~~~~~~~~~

	// ~~~~~~~~~~~~~~~~~~~~~~ ACT_COUNTERATTACK ~~~~~~~~~~~~~~~~~~~~~~

	// ----- 앞의 적 카운터 공격.. -----
	{
		CAction ACT_ANI_MIKE_Mike_Counter_F2F( "ANI_MIKE_Mike_Counter_F2F", 80 );
		ACT_ANI_MIKE_Mike_Counter_F2F.setPrecondition( ACT_INTERACTION, true );
		ACT_ANI_MIKE_Mike_Counter_F2F.setPrecondition( SIGNAL_CHANGE_ANIMATION, true );
		ACT_ANI_MIKE_Mike_Counter_F2F.setPrecondition( SIGNAL_INTERACTION_COUNTER_FRONT1, true );

		ACT_ANI_MIKE_Mike_Counter_F2F.setEffect( SIGNAL_INTERACTION_COUNTER_FRONT1, false );
		ACT_ANI_MIKE_Mike_Counter_F2F.setEffect( ANI_MIKE_Mike_Counter_F2F, true );
		ACT_ANI_MIKE_Mike_Counter_F2F.setEffect( SIGNAL_CHANGE_ANIMATION, false );
		ACT_ANI_MIKE_Mike_Counter_F2F.setEffect( SIGNAL_CHANGE_ANGLE_ONCE, true );
		ACT_ANI_MIKE_Mike_Counter_F2F.setEffect( SIGNAL_SET_ANGLE_TARGET, true );
		ACT_ANI_MIKE_Mike_Counter_F2F.setEffect( CH_STATE_BATTLE, true );
		ACT_ANI_MIKE_Mike_Counter_F2F.setEffect( CH_STATE_INTERACTION, true );
		m_tInfo.m_vecAction_Ani.push_back( ACT_ANI_MIKE_Mike_Counter_F2F );
	}

	// ----- 뒤의 적 카운터 공격.. -----
	{
		CAction ACT_ANI_MIKE_Mike_Counter_B( "ANI_MIKE_Mike_Counter_B", 80 );
		ACT_ANI_MIKE_Mike_Counter_B.setPrecondition( ACT_INTERACTION, true );
		ACT_ANI_MIKE_Mike_Counter_B.setPrecondition( SIGNAL_CHANGE_ANIMATION, true );
		ACT_ANI_MIKE_Mike_Counter_B.setPrecondition( SIGNAL_INTERACTION_COUNTER_BACK1, true );

		ACT_ANI_MIKE_Mike_Counter_B.setEffect( SIGNAL_INTERACTION_COUNTER_BACK1, false );
		ACT_ANI_MIKE_Mike_Counter_B.setEffect( ANI_MIKE_Mike_Counter_B, true );
		ACT_ANI_MIKE_Mike_Counter_B.setEffect( SIGNAL_CHANGE_ANIMATION, false );
		ACT_ANI_MIKE_Mike_Counter_B.setEffect( SIGNAL_CHANGE_ANGLE_ONCE, true );
		ACT_ANI_MIKE_Mike_Counter_B.setEffect( SIGNAL_SET_ANGLE_TARGET, true );
		ACT_ANI_MIKE_Mike_Counter_B.setEffect( CH_STATE_BATTLE, true );
		ACT_ANI_MIKE_Mike_Counter_B.setEffect( CH_STATE_INTERACTION, true );
		m_tInfo.m_vecAction_Ani.push_back( ACT_ANI_MIKE_Mike_Counter_B );
	}

	// ~~~~~~~~~~~~~~~~~~~~~~ ACT_COUNTERATTACK ~~~~~~~~~~~~~~~~~~~~~~

	// %%%%%%%%%%%%%%%%%%%%%%%%%% 애니메이션 결정 액션 %%%%%%%%%%%%%%%%%%%%%%%%%%


	return NOERROR;
}

HRESULT CPlayer::Ready_Raphael(void)
{
	Engine::CComponent*			pComponent = nullptr;

	// For.Mesh
	pComponent = m_pMeshCom = (Engine::CMesh_Dynamic*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CMesh_Dynamic_Raphael");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Mesh", pComponent));
	m_pMeshCom->AddRef();
	m_pMeshCom->Set_RootMotionFrame("Origin");
	lstrcpy(m_tInfo.m_StartAniName, L"ANI_RAPH_TYPE_START");
	m_tInfo.m_StartAniNum = ANI_RAPH_TYPE_START;
	m_tInfo.m_EndAniNum = ANI_RAPH_TYPE_END;

	if (FAILED(Equip_Item(CSai::Create(m_pGraphicDev), m_pMeshCom->Find_FrameMatrixPointer("R_Sword1"))))
		return E_FAIL;

	if (FAILED(Equip_Item(CSai::Create(m_pGraphicDev), m_pMeshCom->Find_FrameMatrixPointer("L_Sword1"))))
		return E_FAIL;


	// -------------------------------- Hit 애니메이션을 차례대로 넣어준다.. -------------------------------- //
	{
		m_iHitAniArr[HitIndex_Front] = ANI_RAPH_Raph_Dmg_F;
		m_iHitAniArr[HitIndex_Right] = ANI_RAPH_Raph_Dmg_R;
		m_iHitAniArr[HitIndex_Back] = ANI_RAPH_Raph_Dmg_B;
		m_iHitAniArr[HitIndex_Left] = ANI_RAPH_Raph_Dmg_L;
	}

	// -------------------------------- 상호 작용 관련 정보 로드.. -------------------------------- //
	if ( FAILED( Ready_LoadInteractionData( L"../Bin/Data/InteractionData/Interaction_Raphael.dat" ) ) )
		return E_FAIL;

	// For.Collider - Add By.KMH
	Engine::CCollider* pCollider = (Engine::CCollider*)CComponent_Manager::GetInstance()->Clone_Component( SCENE_STAGE, L"CCollider" );
	if ( nullptr == pCollider )
		return E_FAIL;
	Ready_LoadCollider( L"../Bin/Data/ColliderData/Raph_Collider.Colldat", pCollider );
	Engine::Safe_Release( pCollider );


	/////////////////////////////// 콜리더 그룹 별로 매니저에 등록.. ///////////////////////////////
	{
		// 콜리더를 등록할 때는 공격 관련 콜리더는 플레이어, 몬스터 별로 같은 레이어에 등록해야되요..
		// 충돌 or 기타 등등의 콜리더는 생성되는 객체 당 하나 씩..

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
		m_tInfo.m_iMoveColliderGroupCount = RaphColGroup_Hit;	// Hit 용 콜리더를 캐릭터의 연산에 따라 갱신될 수 있도록 몇 번째 그룹인지 설정..
	}
	/////////////////////////////// 콜리더 그룹 별로 매니저에 등록 끝.. ///////////////////////////////



	// %%%%%%%%%%%%%%%%%%%%%%%%%% 캐릭터 월드 스테이트 변경 %%%%%%%%%%%%%%%%%%%%%%%%%%
	// ---- ACT ----
	m_tInfo.m_WorldState.setVariable(ACT_TYPE_START, ACT_TYPE_END, false);
	// ---- ANI_RAPH ----
	m_tInfo.m_WorldState.setVariable(ANI_RAPH_TYPE_START, ANI_RAPH_TYPE_END, false);
	// ---- Signal ----
	m_tInfo.m_WorldState.setVariable(SIGNAL_TYPE_START, SIGNAL_TYPE_END, false);
	m_tInfo.m_WorldState.setVariable(CH_STATE_TYPE_START, CH_STATE_TYPE_END, false);
	m_tInfo.m_WorldState.setVariable(TARGET_TYPE_START, TARGET_TYPE_END, false);


	m_tInfo.m_WorldState.setVariable(SIGNAL_CHANGE_ANIMATION, true);
	m_tInfo.m_WorldState.setVariable(ACT_IDLE, true);
	m_tInfo.m_WorldState.setVariable(CH_STATE_PEACE, true);
	//m_tInfo.m_WorldState.setVariable(CH_STATE_DIE, false);
	m_tInfo.m_WorldState.setVariable(TARGET_PLAYER, true);


	// %%%%%%%%%%%%%%%%%%%%%%%%%% 시그널 처리 액션 %%%%%%%%%%%%%%%%%%%%%%%%%%
	// ----- 애니메이션 강제 변환 -----
	CAction ACT_SIGNAL_CHANGE_ANIMATION_FORCED("SIGNAL_CHANGE_ANIMATION_FORCED", 1);
	ACT_SIGNAL_CHANGE_ANIMATION_FORCED.setPrecondition(SIGNAL_CHANGE_ANIMATION_FORCED, true);

	ACT_SIGNAL_CHANGE_ANIMATION_FORCED.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	ACT_SIGNAL_CHANGE_ANIMATION_FORCED.setEffect(SIGNAL_CHANGE_ANIMATION_FORCED, false);
	m_tInfo.m_vecAction_Ani.push_back(ACT_SIGNAL_CHANGE_ANIMATION_FORCED);
	//// ----- 애니메이션 강제 변환 -----
	//// ----- 무빙 종료 -----
	//CAction ACT_WALK_EXIT
	//// ----- 무빙 종료 -----
	// %%%%%%%%%%%%%%%%%%%%%%%%%% 시그널 처리 액션 %%%%%%%%%%%%%%%%%%%%%%%%%%


	// %%%%%%%%%%%%%%%%%%%%%%%%%% 행동 결정 액션 %%%%%%%%%%%%%%%%%%%%%%%%%%

	CAction AI_ACT_Idle("AI_ACT_Idle", 300);
	//ACT_ANI_DON_Don_Idle_Pose.setPrecondition(ACT_WAIT, true);
	AI_ACT_Idle.setPrecondition(CH_STATE_FOUND_ENEMY, false);
	AI_ACT_Idle.setPrecondition(CH_STATE_NEAR_PLAYER, true);

	AI_ACT_Idle.setEffect(ACT_IDLE, true);
	m_tInfo.m_vecAction_Act.push_back(AI_ACT_Idle);

	CAction AI_ACT_RUN_TO_PLAYER("AI_ACT_RUN_TO_PLAYER", 200);
	//AI_ACT_RUN_TO_PLAYER.setPrecondition(CH_STATE_PEACE, true);
	AI_ACT_RUN_TO_PLAYER.setPrecondition(CH_STATE_FOUND_ENEMY, false);
	AI_ACT_RUN_TO_PLAYER.setPrecondition(CH_STATE_NEAR_PLAYER, false);

	//AI_ACT_RUN_TO_PLAYER.setEffect(CH_STATE_NEAR_PLAYER, true);
	//AI_ACT_RUN_TO_PLAYER.setEffect(SIGNAL_MOVE_TARGET, true);
	AI_ACT_RUN_TO_PLAYER.setEffect(TARGET_PLAYER, true);
	AI_ACT_RUN_TO_PLAYER.setEffect(ACT_RUN, true);
	m_tInfo.m_vecAction_Act.push_back(AI_ACT_RUN_TO_PLAYER);

	// 적에게 카운터 어택을 하겠다..
	{
		CAction AI_ACT_COUNTERATTACK( "AI_ACT_COUNTERATTACK", 1 );
		AI_ACT_COUNTERATTACK.setPrecondition( SIGNAL_ACT_INTERACTION, true );

		AI_ACT_COUNTERATTACK.setEffect( ACT_INTERACTION, true );
		AI_ACT_COUNTERATTACK.setEffect( SIGNAL_ACT_INTERACTION, false );
		m_tInfo.m_vecAction_Act.push_back( AI_ACT_COUNTERATTACK );
	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%% 행동 결정 액션 %%%%%%%%%%%%%%%%%%%%%%%%%%


	// %%%%%%%%%%%%%%%%%%%%%%%%%% 애니메이션 결정 액션 %%%%%%%%%%%%%%%%%%%%%%%%%%
	// ----- 기본 대기 상태 -----
	CAction ACT_ANI_RAPH_Raph_Idle("ANI_RAPH_Raph_Idle", 200);
	ACT_ANI_RAPH_Raph_Idle.setPrecondition(ACT_IDLE, true);
	ACT_ANI_RAPH_Raph_Idle.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_RAPH_Raph_Idle.setEffect(ANI_RAPH_Raph_Idle, true);
	ACT_ANI_RAPH_Raph_Idle.setEffect(SIGNAL_CHANGE_ANIMATION_FORCED, true);
	ACT_ANI_RAPH_Raph_Idle.setEffect(SIGNAL_CHANGE_ANGLE, false);
	ACT_ANI_RAPH_Raph_Idle.setEffect(CH_STATE_IDLE, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_RAPH_Raph_Idle);

	// ----- 달리기 -----
	CAction ACT_ANI_RAPH_Raph_Move_Run_F("ANI_RAPH_Raph_Move_Run_F", 90);
	ACT_ANI_RAPH_Raph_Move_Run_F.setPrecondition(ACT_RUN, true);
	ACT_ANI_RAPH_Raph_Move_Run_F.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_RAPH_Raph_Move_Run_F.setEffect(ANI_RAPH_Raph_Move_Run_F, true);
	ACT_ANI_RAPH_Raph_Move_Run_F.setEffect(SIGNAL_CHANGE_ANIMATION_FORCED, true);
	ACT_ANI_RAPH_Raph_Move_Run_F.setEffect(SIGNAL_CHANGE_ANGLE, true);
	ACT_ANI_RAPH_Raph_Move_Run_F.setEffect(CH_STATE_RUN, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_RAPH_Raph_Move_Run_F);

	// ~~~~~~~~~~~~~~~~~~~~~~ ACT_ATTACK ~~~~~~~~~~~~~~~~~~~~~~
	// ----- 기본 공격1 -----
	CAction ACT_ANI_RAPH_Raph_Melee_01("ANI_RAPH_Raph_Melee_01", 80);
	ACT_ANI_RAPH_Raph_Melee_01.setPrecondition(ACT_ATTACK, true);
	ACT_ANI_RAPH_Raph_Melee_01.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_RAPH_Raph_Melee_01.setEffect(ANI_RAPH_Raph_Melee_01, true);
	ACT_ANI_RAPH_Raph_Melee_01.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	ACT_ANI_RAPH_Raph_Melee_01.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_RAPH_Raph_Melee_01);
	// ----- 기본 공격2 -----
	CAction ACT_ANI_RAPH_Raph_Melee_02("ANI_RAPH_Raph_Melee_02", 79);
	ACT_ANI_RAPH_Raph_Melee_02.setPrecondition(ACT_ATTACK, true);
	ACT_ANI_RAPH_Raph_Melee_02.setPrecondition(ANI_RAPH_Raph_Melee_01, true);
	ACT_ANI_RAPH_Raph_Melee_02.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_RAPH_Raph_Melee_02.setEffect(ANI_RAPH_Raph_Melee_02, true);
	ACT_ANI_RAPH_Raph_Melee_02.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	ACT_ANI_RAPH_Raph_Melee_02.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_RAPH_Raph_Melee_02);
	// ----- 기본 공격3 -----
	CAction ACT_ANI_RAPH_Raph_Melee_03("ANI_RAPH_Raph_Melee_03", 79);
	ACT_ANI_RAPH_Raph_Melee_03.setPrecondition(ACT_ATTACK, true);
	ACT_ANI_RAPH_Raph_Melee_03.setPrecondition(ANI_RAPH_Raph_Melee_02, true);
	ACT_ANI_RAPH_Raph_Melee_03.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_RAPH_Raph_Melee_03.setEffect(ANI_RAPH_Raph_Melee_03, true);
	ACT_ANI_RAPH_Raph_Melee_03.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	ACT_ANI_RAPH_Raph_Melee_03.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_RAPH_Raph_Melee_03);
	// ----- 기본 공격4 -----
	CAction ACT_ANI_RAPH_Raph_Melee_04("ANI_RAPH_Raph_Melee_04", 79);
	ACT_ANI_RAPH_Raph_Melee_04.setPrecondition(ACT_ATTACK, true);
	ACT_ANI_RAPH_Raph_Melee_04.setPrecondition(ANI_RAPH_Raph_Melee_03, true);
	ACT_ANI_RAPH_Raph_Melee_04.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_RAPH_Raph_Melee_04.setEffect(ANI_RAPH_Raph_Melee_04, true);
	ACT_ANI_RAPH_Raph_Melee_04.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	ACT_ANI_RAPH_Raph_Melee_04.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_RAPH_Raph_Melee_04);

	// ~~~~~~~~~~~~~~~~~~~~~~ ACT_ATTACK ~~~~~~~~~~~~~~~~~~~~~~

	// ~~~~~~~~~~~~~~~~~~~~~~ ACT_KICK ~~~~~~~~~~~~~~~~~~~~~~
	// ----- 발차기 공격1 -----
	CAction ACT_ANI_RAPH_Raph_Kick_01("ANI_RAPH_Raph_Kick_01", 80);
	ACT_ANI_RAPH_Raph_Kick_01.setPrecondition(ACT_KICK, true);
	ACT_ANI_RAPH_Raph_Kick_01.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_RAPH_Raph_Kick_01.setEffect(ANI_RAPH_Raph_Kick_01, true);
	ACT_ANI_RAPH_Raph_Kick_01.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	ACT_ANI_RAPH_Raph_Kick_01.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_RAPH_Raph_Kick_01);
	// ----- 발차기 공격2 -----
	CAction ACT_ANI_RAPH_Raph_Kick_02("ANI_RAPH_Raph_Kick_02", 79);
	ACT_ANI_RAPH_Raph_Kick_02.setPrecondition(ACT_KICK, true);
	ACT_ANI_RAPH_Raph_Kick_02.setPrecondition(ANI_RAPH_Raph_Kick_01, true);
	ACT_ANI_RAPH_Raph_Kick_02.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_RAPH_Raph_Kick_02.setEffect(ANI_RAPH_Raph_Kick_02, true);
	ACT_ANI_RAPH_Raph_Kick_02.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	ACT_ANI_RAPH_Raph_Kick_02.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_RAPH_Raph_Kick_02);
	// ----- 발차기 공격3 -----
	CAction ACT_ANI_RAPH_Raph_Kick_03("ANI_RAPH_Raph_Kick_03", 79);
	ACT_ANI_RAPH_Raph_Kick_03.setPrecondition(ACT_KICK, true);
	ACT_ANI_RAPH_Raph_Kick_03.setPrecondition(ANI_RAPH_Raph_Kick_02, true);
	ACT_ANI_RAPH_Raph_Kick_03.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_RAPH_Raph_Kick_03.setEffect(ANI_RAPH_Raph_Kick_03, true);
	ACT_ANI_RAPH_Raph_Kick_03.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	ACT_ANI_RAPH_Raph_Kick_03.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_RAPH_Raph_Kick_03);
	// ----- 발차기 공격4 -----
	CAction ACT_ANI_RAPH_Raph_Kick_04("ANI_RAPH_Raph_Kick_04", 79);
	ACT_ANI_RAPH_Raph_Kick_04.setPrecondition(ACT_KICK, true);
	ACT_ANI_RAPH_Raph_Kick_04.setPrecondition(ANI_RAPH_Raph_Kick_03, true);
	ACT_ANI_RAPH_Raph_Kick_04.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_RAPH_Raph_Kick_04.setEffect(ANI_RAPH_Raph_Kick_04, true);
	ACT_ANI_RAPH_Raph_Kick_04.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	ACT_ANI_RAPH_Raph_Kick_04.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_RAPH_Raph_Kick_04);

	// ~~~~~~~~~~~~~~~~~~~~~~ ACT_KICK ~~~~~~~~~~~~~~~~~~~~~~

	// ~~~~~~~~~~~~~~~~~~~~~~ ACT_COUNTERATTACK ~~~~~~~~~~~~~~~~~~~~~~

	// ----- 앞의 적 카운터 공격.. -----
	{
		CAction ACT_ANI_RAPH_Raph_Counter_Push_F( "ANI_RAPH_Raph_Counter_Push_F", 80 );
		ACT_ANI_RAPH_Raph_Counter_Push_F.setPrecondition( ACT_INTERACTION, true );
		ACT_ANI_RAPH_Raph_Counter_Push_F.setPrecondition( SIGNAL_CHANGE_ANIMATION, true );
		ACT_ANI_RAPH_Raph_Counter_Push_F.setPrecondition( SIGNAL_INTERACTION_COUNTER_FRONT1, true );

		ACT_ANI_RAPH_Raph_Counter_Push_F.setEffect( SIGNAL_INTERACTION_COUNTER_FRONT1, false );
		ACT_ANI_RAPH_Raph_Counter_Push_F.setEffect( ANI_RAPH_Raph_Counter_Push_F, true );
		ACT_ANI_RAPH_Raph_Counter_Push_F.setEffect( SIGNAL_CHANGE_ANIMATION, false );
		ACT_ANI_RAPH_Raph_Counter_Push_F.setEffect( SIGNAL_CHANGE_ANGLE_ONCE, true );
		ACT_ANI_RAPH_Raph_Counter_Push_F.setEffect( SIGNAL_SET_ANGLE_TARGET, true );
		ACT_ANI_RAPH_Raph_Counter_Push_F.setEffect( CH_STATE_BATTLE, true );
		ACT_ANI_RAPH_Raph_Counter_Push_F.setEffect( CH_STATE_INTERACTION, true );
		m_tInfo.m_vecAction_Ani.push_back( ACT_ANI_RAPH_Raph_Counter_Push_F );
	}

	// ----- 뒤의 적 카운터 공격.. -----
	{
		CAction ACT_ANI_RAPH_Raph_Counter_B2F( "ANI_RAPH_Raph_Counter_B2F", 80 );
		ACT_ANI_RAPH_Raph_Counter_B2F.setPrecondition( ACT_INTERACTION, true );
		ACT_ANI_RAPH_Raph_Counter_B2F.setPrecondition( SIGNAL_CHANGE_ANIMATION, true );
		ACT_ANI_RAPH_Raph_Counter_B2F.setPrecondition( SIGNAL_INTERACTION_COUNTER_BACK1, true );

		ACT_ANI_RAPH_Raph_Counter_B2F.setEffect( SIGNAL_INTERACTION_COUNTER_BACK1, false );
		ACT_ANI_RAPH_Raph_Counter_B2F.setEffect( ANI_RAPH_Raph_Counter_B2F, true );
		ACT_ANI_RAPH_Raph_Counter_B2F.setEffect( SIGNAL_CHANGE_ANIMATION, false );
		ACT_ANI_RAPH_Raph_Counter_B2F.setEffect( SIGNAL_CHANGE_ANGLE_ONCE, true );
		ACT_ANI_RAPH_Raph_Counter_B2F.setEffect( SIGNAL_SET_ANGLE_TARGET, true );
		ACT_ANI_RAPH_Raph_Counter_B2F.setEffect( CH_STATE_BATTLE, true );
		ACT_ANI_RAPH_Raph_Counter_B2F.setEffect( CH_STATE_INTERACTION, true );
		m_tInfo.m_vecAction_Ani.push_back( ACT_ANI_RAPH_Raph_Counter_B2F );
	}

	// ~~~~~~~~~~~~~~~~~~~~~~ ACT_COUNTERATTACK ~~~~~~~~~~~~~~~~~~~~~~

	// %%%%%%%%%%%%%%%%%%%%%%%%%% 애니메이션 결정 액션 %%%%%%%%%%%%%%%%%%%%%%%%%%

	return NOERROR;
}

HRESULT CPlayer::Ready_Camera(void)
{
	Engine::CCamera*	pCamera = nullptr;

	_vec3 vEye = { 0.f,5.f,-5.f };
	_vec3 vAt = { 0.f,0.f,0.f };
	_vec3 vUp = { 0.f,1.f,0.f };

	// For.Camera
	pCamera = CCamera_Player::Create(m_pGraphicDev, &vEye, &vAt, &vUp);
	if (nullptr == pCamera)
		return E_FAIL;

	if (FAILED(Set_Camera(dynamic_cast<CCamera_Player*>(pCamera))))
		return E_FAIL;

	if (FAILED(dynamic_cast<CCamera_Player*>(pCamera)->Set_Player(m_pTransformCom)))
		return E_FAIL;

	dynamic_cast<CCamera_Player*>(pCamera)->Set_PlayerType(m_eType);

	if (FAILED(CCamera_Manager::GetInstance()->Add_Camera(SCENE_STATIC, L"Layer_PlayerCamera", pCamera)))
		return E_FAIL;

	if (CCamera_Manager::GetInstance()->Get_IsChangeCamera())
	{
		CCamera_Manager::GetInstance()->Set_CurrentCamera(pCamera);
	}
	//if (FAILED(m_pCamera_Manager->Set_CurrentCamera(pCamera)))
	//	return E_FAIL;

	return NOERROR;
}

HRESULT CPlayer::Ready_Controller(void)
{
	Engine::CComponent*			pComponent = nullptr;

	//// For.Controller
	//pComponent = CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CPlayerController");
	//if (nullptr == pComponent)
	//	return E_FAIL;
	//m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Controller", pComponent));
	//dynamic_cast<CPlayerController*>(pComponent)->Possess(this);

	// For.Controller
	pComponent = m_pController = (CAIController*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CAIController");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Controller", pComponent));
	m_pController->AddRef();

	dynamic_cast<CAIController*>(pComponent)->Possess(this);

	return NOERROR;
}

HRESULT CPlayer::Ready_LoadInteractionData( const _tchar* _pFilePath )
{
	HANDLE hFile = CreateFile( _pFilePath, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

	if ( INVALID_HANDLE_VALUE == hFile )
		return E_FAIL;

	_ulong dwByte = 0;

	if ( FAILED( m_InteractionInfo.Load( hFile, dwByte ) ) )
		return E_FAIL;

	_ulong dwFrontCounterIndex = 0;
	_ulong dwBackCounterIndex = 0;

	switch ( m_eType )
	{
		case PLAYER_Donatello:
			dwFrontCounterIndex = SIGNAL_INTERACTION_COUNTER_DONFRONT1;
			dwBackCounterIndex = SIGNAL_INTERACTION_COUNTER_DONBACK1;
			break;
		case PLAYER_Leonardo:
			dwFrontCounterIndex = SIGNAL_INTERACTION_COUNTER_LEOFRONT1;
			dwBackCounterIndex = SIGNAL_INTERACTION_COUNTER_LEOBACK1;
			break;
		case PLAYER_Michelangelo:
			dwFrontCounterIndex = SIGNAL_INTERACTION_COUNTER_MIKEFRONT1;
			dwBackCounterIndex = SIGNAL_INTERACTION_COUNTER_MIKEBACK1;
			break;
		case PLAYER_Raphael:
			dwFrontCounterIndex = SIGNAL_INTERACTION_COUNTER_RAPHFRONT1;
			dwBackCounterIndex = SIGNAL_INTERACTION_COUNTER_RAPHBACK1;
			break;
		default:
			MSG_BOX( "Strange Player Type" );
			return E_FAIL;
	}

	if ( m_InteractionInfo.iInteractionCount > 0 )
	{
		if ( nullptr == m_InteractionInfo.pInteractionData[0].pOptionData )
		{
			m_InteractionInfo.pInteractionData[0].pOptionData = new INTERACTIONOPTIONDATAINFO[1];
			m_InteractionInfo.pInteractionData[0].pOptionData[0].OptionData.dwOption = SIGNAL_ACT_INTERACTION;
			m_InteractionInfo.pInteractionData[0].pOptionData[0].OptionData.DetailDataList.push_back( dwFrontCounterIndex );
		}
	}
	if ( m_InteractionInfo.iInteractionCount > 1 )
	{
		if ( nullptr == m_InteractionInfo.pInteractionData[1].pOptionData )
		{
			m_InteractionInfo.pInteractionData[1].pOptionData = new INTERACTIONOPTIONDATAINFO[1];
			m_InteractionInfo.pInteractionData[1].pOptionData[0].OptionData.dwOption = SIGNAL_ACT_INTERACTION;
			m_InteractionInfo.pInteractionData[1].pOptionData[0].OptionData.DetailDataList.push_back( dwBackCounterIndex );
		}
	}

	return NOERROR;
}

HRESULT CPlayer::SetUp_OnConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorld");

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);	

	Safe_Release(pEffect);

	return NOERROR;
}

_uint CPlayer::Compute_HitAniIndex( const Engine::Collision & _Collision )
{
	// ---------- 충돌된 상대와의 방향과 나의 Look 방향을 통해 각도를 구한다.. ---------- //
	_float fAngle = ComputeAngle_HitTargetTransform( _Collision );

	_uint iIndex = (_uint)((fAngle + 0.1f) / 45.f);

	iIndex = ( ( iIndex + 1 ) % 8 ) / 2;

	return iIndex;
}

_float CPlayer::ComputeAngle_HitTargetTransform( const Engine::Collision & _Collision )
{
	// ---------- 충돌된 상대와의 방향과 나의 Look 방향을 통해 각도를 구한다.. ---------- //
	_vec3 vDir( 0.f, 0.f, 0.f );
	_vec3 vMyPosition = *m_pTransformCom->Get_Infomation( Engine::CTransform::INFO_POSITION );

	if ( nullptr != _Collision.pOtherCollider->Get_ObjectTransform() )
	{
		_vec3 vTargetPosition = *_Collision.pOtherCollider->Get_ObjectTransform()->Get_Infomation( Engine::CTransform::INFO_POSITION );
		vDir = vTargetPosition - vMyPosition; vDir.y = 0.f;
	}
	else
	{
		vDir = _Collision.CollisionData.vCollisionPos - vMyPosition; vDir.y = 0.f;
	}

	_vec3 vLook = *m_pTransformCom->Get_WorldLookVec(); vLook.y = 0.f;
	_vec3 vRight = *m_pTransformCom->Get_WorldRightVec(); vRight.y = 0.f;
	_vec3 vOut( 0.f, 0.f, 0.f );

	D3DXVec3Normalize( &vLook, &vLook );
	D3DXVec3Normalize( &vDir, &vDir );
	D3DXVec3Normalize( &vRight, &vRight );

	_float fAngle = D3DXToDegree( acosf( D3DXVec3Dot( &vDir, &vLook ) ) );
	D3DXVec3Cross( &vOut, &vDir, &vRight );

	if ( vOut.y <= 0.f )
		fAngle = 360.f - fAngle;

	return fAngle;
}

_float CPlayer::ComputeAngle_HitTargetCollider( const Engine::Collision & _Collision )
{
	// ---------- 충돌된 상대와의 방향과 나의 Look 방향을 통해 각도를 구한다.. ---------- //
	_vec3 vDir( 0.f, 0.f, 0.f );
	_vec3 vMyPosition = *m_pTransformCom->Get_Infomation( Engine::CTransform::INFO_POSITION );

	vDir = _Collision.CollisionData.vCollisionPos - vMyPosition; vDir.y = 0.f;

	_vec3 vLook = *m_pTransformCom->Get_WorldLookVec(); vLook.y = 0.f;
	_vec3 vRight = *m_pTransformCom->Get_WorldRightVec(); vRight.y = 0.f;
	_vec3 vOut( 0.f, 0.f, 0.f );

	D3DXVec3Normalize( &vLook, &vLook );
	D3DXVec3Normalize( &vDir, &vDir );
	D3DXVec3Normalize( &vRight, &vRight );

	_float fAngle = D3DXToDegree( acosf( D3DXVec3Dot( &vDir, &vLook ) ) );
	D3DXVec3Cross( &vOut, &vDir, &vRight );

	if ( vOut.y <= 0.f )
		fAngle = 360.f - fAngle;

	return fAngle;
}

CPlayer * CPlayer::Create(LPDIRECT3DDEVICE9 pGraphicDev, PLAYER_TYPE eType)
{
	CPlayer *	pInstance = new CPlayer(pGraphicDev);
	pInstance->m_eType = eType;

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MSG_BOX("CPlayer Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}

//Engine::EStatus CPlayer::Move_Forward(float fScale /*= 1.f*/)
//{
//	//현재 카메라가 플레이어 카메라면
//	if (CCamera_Manager::GetInstance()->Get_CurrentCamera() == m_pCamera)
//	{
//		// 카메라
//		m_pTransformCom->Go_Direction(
//			CCamera_Manager::GetInstance()->Get_Direction_NotY(),
//			m_tInfo.m_fSpeed * fScale,
//			m_pTimer->Get_TimeDelta());
//		return EStatus::Success;
//
//	}
//	return EStatus::Failure;
//}
//
//Engine::EStatus CPlayer::Move_Right(float fScale /*= 1.f*/)
//{
//	//현재 카메라가 플레이어 카메라면
//	if (CCamera_Manager::GetInstance()->Get_CurrentCamera() == m_pCamera)
//	{
//		// 카메라
//		m_pTransformCom->Go_Direction(
//			CCamera_Manager::GetInstance()->Get_WorldMatrix().m[0],
//			m_tInfo.m_fSpeed * fScale,
//			m_pTimer->Get_TimeDelta());
//		return EStatus::Success;
//
//	}
//	return EStatus::Failure;
//}


#ifndef _FINAL
void CPlayer::Debug_Render(void)
{
	_matrix matW = m_pTransformCom->Get_WorldMatrix();

	CDebug::GetInstance()->Add_DebugText("Player");
	CDebug::GetInstance()->Add_DebugText("R: %f %f %f", matW.m[0][0], matW.m[0][1], matW.m[0][2]);
	CDebug::GetInstance()->Add_DebugText("U: %f %f %f", matW.m[1][0], matW.m[1][1], matW.m[1][2]);
	CDebug::GetInstance()->Add_DebugText("L: %f %f %f", matW.m[2][0], matW.m[2][1], matW.m[2][2]);
	CDebug::GetInstance()->Add_DebugText("P: %f %f %f", matW.m[3][0], matW.m[3][1], matW.m[3][2]);
	CDebug::GetInstance()->Add_DebugText("MoveDelta: %f %f %f", m_pMeshCom->Get_MoveDelta().x, m_pMeshCom->Get_MoveDelta().y, m_pMeshCom->Get_MoveDelta().z);
	CDebug::GetInstance()->Add_DebugText("AngleDelta: %f %f %f", m_pMeshCom->Get_AngleDelta().x, m_pMeshCom->Get_AngleDelta().y, m_pMeshCom->Get_AngleDelta().z);
	CDebug::GetInstance()->Add_DebugText("AniPos: %f / %f", m_pMeshCom->Get_Period(), m_pMeshCom->Get_TrackInfo()->Position);
	CDebug::GetInstance()->Add_DebugText("INFO_ANGLE: %f / %f / %f", m_pTransformCom->Get_Infomation(CTransform::INFO_ANGLE)->x, m_pTransformCom->Get_Infomation(CTransform::INFO_ANGLE)->y, m_pTransformCom->Get_Infomation(CTransform::INFO_ANGLE)->z);
	CDebug::GetInstance()->Add_DebugText("Camera-Player Angle: %f", m_tInfo.m_fAngleGap_CameraToPlayer);
	
}
#endif
_ulong CPlayer::Free(void)
{
	_ulong		dwRefCnt = 0;
	
	CSubject_Manager::GetInstance()->RemoveData(MESSAGE_PLAYER_DATA, CSubject_Manager::SUBJECTTYPE::TYPE_STATIC, this);
	CSubject_Manager::GetInstance()->RemoveData(MESSAGE_CURRENT_PLAYER_DATA, CSubject_Manager::SUBJECTTYPE::TYPE_STATIC, this);
	CSubject_Manager::GetInstance()->RemoveData(MESSAGE_STATE_DATA, CSubject_Manager::SUBJECTTYPE::TYPE_STATIC, &m_tInfo.m_WorldState);

	m_InteractionInfo.Release();
	
	dwRefCnt = CCharacter::Free();

	return dwRefCnt;
}
