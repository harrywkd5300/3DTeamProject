#include "..\Default\stdafx.h"
#include "..\Headers\Soldier_Bomb.h"
#include "Clinet_Component.h"
#include "Object_Manager.h"
#include "Component_Manager.h"

#include "Soldier_Sword.h"
#include "Bomb_Mask.h"

#include "Navigation.h"
#include "Graphic_Device.h"
#include "Collider_Manager.h"
#include "Camera_Player.h"

CSoldier_Bomb::CSoldier_Bomb(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMonster(pGraphicDev)
{
#ifndef _FINAL
	Tag(L"CSoldier_Bomb", std::bind(&CSoldier_Bomb::Debug_Render, this));
#endif

}


HRESULT CSoldier_Bomb::Ready_GameObject(void)
{
	Engine::CComponent*			pComponent = nullptr;

	pComponent = m_pMeshCom = (Engine::CMesh_Dynamic*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"CMesh_Dynamic_Foot_Soldier_Bomb");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Mesh", pComponent));
	m_pMeshCom->AddRef();
	m_pMeshCom->Set_RootMotionFrame("Origin");
	lstrcpy(m_tInfo.m_StartAniName, L"ANI_SOLDIER_BOMB_TYPE_START");
	m_tInfo.m_StartAniNum = ANI_SOLDIER_BOMB_TYPE_START;
	m_tInfo.m_EndAniNum = ANI_SOLDIER_BOMB_TYPE_END;

	if (FAILED(CMonster::Ready_Monster_Component()))
		return E_FAIL;


#ifdef CHARACTER_COLLIDERMODIFY
	Engine::CCollider* pCollider = (Engine::CCollider*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"CCollider");
	if (nullptr == pCollider)
		return E_FAIL;
	Ready_LoadCollider(L"../Bin/Data/ColliderData/Soldier_Bomb_Collider.Colldat", pCollider);
	Engine::Safe_Release(pCollider);

	/////////////////////////////// 콜리더 그룹 별로 매니저에 등록.. ///////////////////////////////
	// 콜리더를 등록할 때는 공격 관련 콜리더는 플레이어, 몬스터 별로 같은 레이어에 등록해야되요..
	// 충돌 or 기타 등등의 콜리더는 생성되는 객체 당 하나 씩..

	if ( FAILED( Ready_CollidersToColliderManager( CCollider_Manager::GROUP_STATIC, this ) ) )
		return E_FAIL;

	_tchar szLayerName[256] = L"";
	swprintf_s(szLayerName, L"Layer_Soldier_Bomb_Hit%d", GetKey());
	const _tchar* pLayerName = CString_Manager::GetInstance()->GetStr(szLayerName);

	if ( false == Coiiders_Add_ColliderLayer( CCollider_Manager::GROUP_STATIC, pLayerName, Soldier_BombColGroup_Hit) )
		return E_FAIL;
	if ( false == Coiiders_Add_ColliderLayer( CCollider_Manager::GROUP_STATIC, L"Layer_Monster_Attack", Soldier_BombColGroup_Attack) )
		return E_FAIL;

	// -------------- Hit 용 콜리더와 Attack 용 콜리더는 연산할 필요 없기 때문에 연산하지 못하게.. -------------- //
	m_pCollider_Manager->Ignore_ColliderLayer( L"Layer_Monster_Attack", pLayerName );
	m_tInfo.m_iMoveColliderGroupCount = Soldier_BombColGroup_Hit;	// Hit 용 콜리더를 캐릭터의 연산에 따라 갱신될 수 있도록 몇 번째 그룹인지 설정..

	/////////////////////////////// 콜리더 그룹 별로 매니저에 등록 끝.. ///////////////////////////////


#else
	// For.Collider
	pComponent = m_pColliderCom = (Engine::CCollider*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"CCollider");
	if (nullptr == pComponent)
		return E_FAIL;

	CCollider::RESIZEDATA Data;

	//Data.fOriginalRadius = 70.f;
	Data.pTransformCom = m_pTransformCom;
	Data.pParent = m_pMeshCom->Find_FrameMatrixPointer("spine01");
	Data.vOriginalMin = _vec3(-70.f, -70.f, -70.f);
	Data.vOriginalMax = _vec3(70.f, 70.f, 70.f);


	m_pColliderCom->ReSizing(CCollider::TYPE_SPHERE, Data);
	m_mapComponent[Engine::CComponent::TYPE_DYNAMIC].insert(MAPCOMPONENT::value_type(L"Com_Collider", pComponent));
	m_pColliderCom->AddRef();


	// Add By.KMH
	m_pColliderCom->Modify_StaticOption(CCollider::STATICOPTIONDATA(PushOption_BePushedOut), StaticOption_Push);
	CCollider_Manager::GetInstance()->Add_Collider(Engine::CCollider_Manager::GROUP_STATIC, L"Group_KnifeThug", m_pColliderCom, this);
	CCollider_Manager::GetInstance()->Add_ColliderGroup(Engine::CCollider_Manager::GROUP_STATIC, L"Layer_KnifeThug", L"Group_KnifeThug");
#endif

	if (FAILED(Equip_Item(CSoldier_Sword::Create(m_pGraphicDev), m_pMeshCom->Find_FrameMatrixPointer("SaberStaff_Root"))))
		return E_FAIL;

	if (FAILED(Equip_Item(CBomb_Mask::Create(m_pGraphicDev), m_pMeshCom->Find_FrameMatrixPointer("head"))))
		return E_FAIL;

	if (FAILED(Ready_AI()))
		return E_FAIL;

	

	m_pBombHand_mat = m_pMeshCom->Find_FrameMatrixPointer("Lpalm");

	m_pTransformCom->Set_Infomation(Engine::CTransform::INFO_POSITION, &_vec3(130.2f, 0.f, 120.0f));
	m_pTransformCom->Set_Infomation(Engine::CTransform::INFO_SCALE, &_vec3(0.01f, 0.01f, 0.01f));

	m_pMeshCom->Set_AnimationSet(0);
	m_pMeshCom->Play_Animation(0.f);





	// Info 데이터 초기값 세팅..
	m_tInfo.m_fSpeed = 2.f;

	// 캐릭터의 Range 를 세팅..
	m_tInfo.m_fRange_Close = 2.f;
	m_tInfo.m_fRange_Near = 4.f;
	m_tInfo.m_fRange_Mid = 4.5f;
	m_tInfo.m_fRange_Far = 20.f;
	m_tInfo.m_fRange_TooFar = 40.f;
	m_tInfo.m_fRange_FoundEnemy = 20.f;

	m_tInfo.m_iMaxHp = m_tInfo.m_iCurHp = 100;


	// 쉴드 게이지 세팅..
	m_tInfo.m_iShield_Point = 40;
	m_tInfo.m_bShield = true;

	m_tInfo.m_fEvade_Point = 1.f;
	m_tInfo.m_bEvade = false;


	//쉴드 맥스량
	m_iShieldMax = 40;

	//초당 게이지 찰 수치
	m_iAdd_Shield_Point = 5;	//8초당 재생
	m_fAdd_Evade_Point = 1.f;			// 나중에 바꾸자 테스트용     1초마다 회피하게 해놓음



	m_pTrack = m_pMeshCom->Get_TrackInfo();


	_uint iCount = CRandom::GetInstance()->GetValue(0, 10);
	m_iSelect_Atk_Count = iCount;


	return NOERROR;
}

_int CSoldier_Bomb::Update_GameObject(const _float & fTimeDelta)
{
	int returnValue = CMonster::Update_GameObject(fTimeDelta);

	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_NONALPHA, this);
	}

	
	return returnValue;
}

_int CSoldier_Bomb::LastUpdate_GameObject(const _float & fTimeDelta)
{
	CMonster::LastUpdate_GameObject(fTimeDelta);

	if (true == m_bIsDead)
		return 1;


	if (true == m_tInfo.m_WorldState.getVariable(SIGNAL_SOLDIER_BOMB_THROWING_KNOW))
	{
		m_tInfo.m_WorldState.setVariable(SIGNAL_SOLDIER_BOMB_THROWING_KNOW, false);

		m_bThrow_On = true;
	}


	if (true == m_bThrow_On)
	{
		if (0.4 <= m_pTrack->Position)
		{
			if (true == m_bCreate_Bomb) 
			{
				//마스크 생성
				if (nullptr == m_pBomb)
					Create_Bomb();
				else if (nullptr != m_pBomb)
				{
					Safe_Release(m_pBomb);
					Create_Bomb();
				}

				m_bCreate_Bomb = false;
			}
				
		}


		if (1.2 <= m_pTrack->Position)
		{
			m_bThrow_On = false;

			if (nullptr != m_pBomb)
				m_pBomb->Set_bThrowing();
		}
	}




	for (_uint i = 0; i < m_ColliderGroups.pGroup[Soldier_BombColGroup_Attack].iColliderCount; ++i)
	{
		OPTIONDATA OptionData;
		OptionData.dwOption = SIGNAL_HIT_ENEMY;
		OptionData.DetailDataList.push_back(0);
		m_ColliderGroups.pGroup[Soldier_BombColGroup_Attack].ppCollider[i]->Add_Option(OptionData);
	}

	int iRelease_Count = 0;

	if(nullptr != m_pBomb)
		iRelease_Count = m_pBomb->Update_GameObject(fTimeDelta);

	if (1 == iRelease_Count)
	{
		m_bCreate_Bomb = true;
		Safe_Release(m_pBomb);
	}

	return 0;
}

void CSoldier_Bomb::Render_GameObject(void)
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


	SetUp_OnConstantTable(pEffect);

	pEffect->Begin(nullptr, 0);

	pEffect->BeginPass(0);

	m_pMeshCom->Render_Mesh(pEffect, "g_BaseTexture", "g_NormalTexture", "g_SpecularTexture");

	pEffect->EndPass();

	pEffect->End();

	Safe_Release(pEffect);

	m_pNaviCom->Render_Navigation();
	



	if (nullptr != m_pBomb)
		m_pBomb->Render_GameObject();
}

HRESULT CSoldier_Bomb::Ready_AI(void)
{
	m_tInfo.m_StartAniNum = ANI_SOLDIER_BOMB_TYPE_START;
	m_tInfo.m_EndAniNum = ANI_SOLDIER_BOMB_TYPE_END;
	// %%%%%%%%%%%%%%%%%%%%%%%%%% 캐릭터 월드 스테이트 변경 %%%%%%%%%%%%%%%%%%%%%%%%%%
	// ---- ACT ----
	m_tInfo.m_WorldState.setVariable(ACT_TYPE_START, ACT_TYPE_END, false);
	// ---- ANI_BIGTHUG ----
	m_tInfo.m_WorldState.setVariable(ANI_SOLDIER_BOMB_TYPE_START, ANI_SOLDIER_BOMB_TYPE_END, false);
	// ---- Signal ----
	m_tInfo.m_WorldState.setVariable(SIGNAL_TYPE_START, SIGNAL_TYPE_END, false);
	m_tInfo.m_WorldState.setVariable(CH_STATE_TYPE_START, CH_STATE_TYPE_END, false);
	m_tInfo.m_WorldState.setVariable(TARGET_TYPE_START, TARGET_TYPE_END, false);

	m_tInfo.m_WorldState.setVariable(SIGNAL_CHANGE_ANIMATION, true);
	m_tInfo.m_WorldState.setVariable(ACT_IDLE, true);
	m_tInfo.m_WorldState.setVariable(CH_STATE_PEACE, true);
	m_tInfo.m_WorldState.setVariable(CH_STATE_FOUND_ENEMY, false);
	m_tInfo.m_WorldState.setVariable(SIGNAL_CHANGE_TARGET_NEAR_ENEMY, true);


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

	//기본 Idle
	CAction AI_ACT_Idle("AI_ACT_Idle", 300);
	AI_ACT_Idle.setPrecondition(CH_STATE_FOUND_ENEMY, false);

	AI_ACT_Idle.setEffect(CH_STATE_PEACE, true);
	AI_ACT_Idle.setEffect(ACT_IDLE, true);
	m_tInfo.m_vecAction_Act.push_back(AI_ACT_Idle);

	// 적이 멀리 있을 때..
	{
		CAction AI_ACT_GoTo_Target("AI_ACT_GoTo_Enemy", 2000);
		AI_ACT_GoTo_Target.setPrecondition(CH_STATE_FOUND_ENEMY, true);
		AI_ACT_GoTo_Target.setPrecondition(TARGET_NONE, false);
		AI_ACT_GoTo_Target.setPrecondition(CH_STATE_MID_TARGET, false);
		AI_ACT_GoTo_Target.setPrecondition(CH_STATE_NEAR_TARGET, false);
		AI_ACT_GoTo_Target.setPrecondition(CH_STATE_CLOSE_TARGET, false);
		AI_ACT_GoTo_Target.setPrecondition(CH_STATE_HIT, false);
		AI_ACT_GoTo_Target.setPrecondition(CH_STATE_DIE, false);

		AI_ACT_GoTo_Target.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
		AI_ACT_GoTo_Target.setEffect(SIGNAL_CHANGE_ANGLE, true);
		AI_ACT_GoTo_Target.setEffect(SIGNAL_CHANGE_DIR_FRONT, true);
		AI_ACT_GoTo_Target.setEffect(ACT_RUN, true);
		m_tInfo.m_vecAction_Act.push_back(AI_ACT_GoTo_Target);
	}

	// 적이 내 맴도는 범위 안이라면..
	{
		CAction AI_ACT_MoveAround_Target("AI_ACT_MoveAround_Target", 2000);
		AI_ACT_MoveAround_Target.setPrecondition(TARGET_NONE, false);
		AI_ACT_MoveAround_Target.setPrecondition(CH_STATE_MID_TARGET, true);
		AI_ACT_MoveAround_Target.setPrecondition(CH_STATE_ATTACK, false);
		AI_ACT_MoveAround_Target.setPrecondition(CH_STATE_HIT, false);
		AI_ACT_MoveAround_Target.setPrecondition(CH_STATE_DIE, false);

		AI_ACT_MoveAround_Target.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
		AI_ACT_MoveAround_Target.setEffect(SIGNAL_CHANGE_ANGLE, true);
		AI_ACT_MoveAround_Target.setEffect(ACT_WALK, true);
		m_tInfo.m_vecAction_Act.push_back(AI_ACT_MoveAround_Target);
	}

	// 적을 공격하라는 신호를 받았을 때 ( 하지만 공격 범위 밖에 있을 때, 때리러 감 )..
	{
		CAction AI_ACT_MoveTo_Target("AI_ACT_MoveTo_Target", 1999);
		AI_ACT_MoveTo_Target.setPrecondition(TARGET_NONE, false);
		AI_ACT_MoveTo_Target.setPrecondition(CH_STATE_CLOSE_TARGET, false);
		AI_ACT_MoveTo_Target.setPrecondition(SIGNAL_ATTACK_TARGET, true);
		AI_ACT_MoveTo_Target.setPrecondition(CH_STATE_ATTACK, false);
		AI_ACT_MoveTo_Target.setPrecondition(CH_STATE_HIT, false);
		AI_ACT_MoveTo_Target.setPrecondition(CH_STATE_DIE, false);

		AI_ACT_MoveTo_Target.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
		AI_ACT_MoveTo_Target.setEffect(SIGNAL_CHANGE_ANGLE, true);
		AI_ACT_MoveTo_Target.setEffect(SIGNAL_CHANGE_DIR_FRONT, true);
		AI_ACT_MoveTo_Target.setEffect(ACT_WALK, true);
		m_tInfo.m_vecAction_Act.push_back(AI_ACT_MoveTo_Target);
	}

	// 적을 공격하라는 신호를 받았을 때 ( 공격 범위 안에 있을 때, 공격 한다 )..
	{
		CAction AI_ACT_Attack_Target("AI_ACT_Attack_Target", 2000);
		AI_ACT_Attack_Target.setPrecondition(TARGET_NONE, false);
		AI_ACT_Attack_Target.setPrecondition(CH_STATE_CLOSE_TARGET, true);
		AI_ACT_Attack_Target.setPrecondition(SIGNAL_ATTACK_TARGET, true);
		AI_ACT_Attack_Target.setPrecondition(CH_STATE_ATTACK, false);
		AI_ACT_Attack_Target.setPrecondition(CH_STATE_HIT, false);
		AI_ACT_Attack_Target.setPrecondition(CH_STATE_DIE, false);

		AI_ACT_Attack_Target.setEffect(SIGNAL_SET_ANGLE_TARGET, false);
		AI_ACT_Attack_Target.setEffect(SIGNAL_CHANGE_ANGLE, false);
		AI_ACT_Attack_Target.setEffect(ACT_ATTACK, true);
		AI_ACT_Attack_Target.setEffect(SIGNAL_ATTACK_TARGET, false);
		m_tInfo.m_vecAction_Act.push_back(AI_ACT_Attack_Target);
	}




	//맞음
	CAction AI_ACT_HIT("AI_ACT_HIT", 199);
	AI_ACT_HIT.setPrecondition(SIGNAL_HIT_ENEMY, true);
	
	AI_ACT_HIT.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
	AI_ACT_HIT.setEffect(SIGNAL_CHANGE_ANGLE, true);
	AI_ACT_HIT.setEffect(SIGNAL_CHANGE_ANIMATION_FORCED, true);
	AI_ACT_HIT.setEffect(ACT_HIT, true);
	m_tInfo.m_vecAction_Act.push_back(AI_ACT_HIT);


	// 죽었다..
	{
		CAction AI_ACT_Die("AI_ACT_Die", 100);
		AI_ACT_Die.setPrecondition(SIGNAL_DIE, true);
		AI_ACT_Die.setPrecondition(CH_STATE_DIE, false);

		AI_ACT_Die.setEffect(SIGNAL_DIE, false);
		AI_ACT_Die.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
		AI_ACT_Die.setEffect(SIGNAL_CHANGE_ANGLE, true);
		AI_ACT_Die.setEffect(SIGNAL_CHANGE_ANIMATION_FORCED, true);
		AI_ACT_Die.setEffect(ACT_DEATH, true);
		m_tInfo.m_vecAction_Act.push_back(AI_ACT_Die);
	}

	


	// %%%%%%%%%%%%%%%%%%%%%%%%%% 행동 결정 액션 %%%%%%%%%%%%%%%%%%%%%%%%%%


	// %%%%%%%%%%%%%%%%%%%%%%%%%% 애니메이션 결정 액션 %%%%%%%%%%%%%%%%%%%%%%%%%%
	// ----- 기본 대기 상태 -----
	CAction ACT_ANI_SOLDIER_BOMB_Idle_Ready_R01("ANI_SOLDIER_BOMB_Idle_Ready_R01", 200);
	ACT_ANI_SOLDIER_BOMB_Idle_Ready_R01.setPrecondition(ACT_IDLE, true);
	ACT_ANI_SOLDIER_BOMB_Idle_Ready_R01.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_SOLDIER_BOMB_Idle_Ready_R01.setEffect(CH_STATE_IDLE, true);
	ACT_ANI_SOLDIER_BOMB_Idle_Ready_R01.setEffect(ANI_SOLDIER_BOMB_Idle_Ready_R01, true);
	ACT_ANI_SOLDIER_BOMB_Idle_Ready_R01.setEffect(SIGNAL_CHANGE_ANIMATION_FORCED, true);
	ACT_ANI_SOLDIER_BOMB_Idle_Ready_R01.setEffect(SIGNAL_CHANGE_ANGLE, false);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_SOLDIER_BOMB_Idle_Ready_R01);



	// ----- 걷기( Front ).. -----
	{
		CAction ACT_ANI_SOLDIER_BOMB_Walk_R01_F("ANI_SOLDIER_BOMB_Walk_R01_F", 90);
		ACT_ANI_SOLDIER_BOMB_Walk_R01_F.setPrecondition(ACT_WALK, true);
		ACT_ANI_SOLDIER_BOMB_Walk_R01_F.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);
		ACT_ANI_SOLDIER_BOMB_Walk_R01_F.setPrecondition(MOVE_DIR_FRONT, true);

		ACT_ANI_SOLDIER_BOMB_Walk_R01_F.setEffect(ANI_SOLDIER_BOMB_Walk_R01_F, true);
		ACT_ANI_SOLDIER_BOMB_Walk_R01_F.setEffect(SIGNAL_CHANGE_ANIMATION_FORCED, true);
		ACT_ANI_SOLDIER_BOMB_Walk_R01_F.setEffect(SIGNAL_CHANGE_ANGLE, true);
		ACT_ANI_SOLDIER_BOMB_Walk_R01_F.setEffect(CH_STATE_WALK, true);
		m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_SOLDIER_BOMB_Walk_R01_F);
	}

	// ----- 걷기( Back ).. -----
	{
		CAction ACT_ANI_SOLDIER_BOMB_Walk_R01_B("ANI_SOLDIER_BOMB_Walk_R01_B", 90);
		ACT_ANI_SOLDIER_BOMB_Walk_R01_B.setPrecondition(ACT_WALK, true);
		ACT_ANI_SOLDIER_BOMB_Walk_R01_B.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);
		ACT_ANI_SOLDIER_BOMB_Walk_R01_B.setPrecondition(MOVE_DIR_BACK, true);

		ACT_ANI_SOLDIER_BOMB_Walk_R01_B.setEffect(ANI_SOLDIER_BOMB_Walk_R01_B, true);
		ACT_ANI_SOLDIER_BOMB_Walk_R01_B.setEffect(SIGNAL_CHANGE_ANIMATION_FORCED, true);
		ACT_ANI_SOLDIER_BOMB_Walk_R01_B.setEffect(SIGNAL_CHANGE_ANGLE, true);
		ACT_ANI_SOLDIER_BOMB_Walk_R01_B.setEffect(CH_STATE_WALK, true);
		m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_SOLDIER_BOMB_Walk_R01_B);
	}

	// ----- 걷기( Left ).. -----
	{
		CAction ACT_ANI_SOLDIER_BOMB_Walk_R01_L("ANI_SOLDIER_BOMB_Walk_R01_L", 90);
		ACT_ANI_SOLDIER_BOMB_Walk_R01_L.setPrecondition(ACT_WALK, true);
		ACT_ANI_SOLDIER_BOMB_Walk_R01_L.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);
		ACT_ANI_SOLDIER_BOMB_Walk_R01_L.setPrecondition(MOVE_DIR_LEFT, true);

		ACT_ANI_SOLDIER_BOMB_Walk_R01_L.setEffect(ANI_SOLDIER_BOMB_Walk_R01_L, true);
		ACT_ANI_SOLDIER_BOMB_Walk_R01_L.setEffect(SIGNAL_CHANGE_ANIMATION_FORCED, true);
		ACT_ANI_SOLDIER_BOMB_Walk_R01_L.setEffect(SIGNAL_CHANGE_ANGLE, true);
		ACT_ANI_SOLDIER_BOMB_Walk_R01_L.setEffect(CH_STATE_WALK, true);
		m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_SOLDIER_BOMB_Walk_R01_L);
	}

	// ----- 걷기( Right ).. -----
	{
		CAction ACT_ANI_SOLDIER_BOMB_Walk_R01_R("ANI_SOLDIER_BOMB_Walk_R01_R", 90);
		ACT_ANI_SOLDIER_BOMB_Walk_R01_R.setPrecondition(ACT_WALK, true);
		ACT_ANI_SOLDIER_BOMB_Walk_R01_R.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);
		ACT_ANI_SOLDIER_BOMB_Walk_R01_R.setPrecondition(MOVE_DIR_RIGHT, true);

		ACT_ANI_SOLDIER_BOMB_Walk_R01_R.setEffect(ANI_SOLDIER_BOMB_Walk_R01_R, true);
		ACT_ANI_SOLDIER_BOMB_Walk_R01_R.setEffect(SIGNAL_CHANGE_ANIMATION_FORCED, true);
		ACT_ANI_SOLDIER_BOMB_Walk_R01_R.setEffect(SIGNAL_CHANGE_ANGLE, true);
		ACT_ANI_SOLDIER_BOMB_Walk_R01_R.setEffect(CH_STATE_WALK, true);
		m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_SOLDIER_BOMB_Walk_R01_R);
	}






	// ----- 달리기 -----
	CAction ACT_ANI_SOLDIER_BOMB_Run_F_OV("ANI_SOLDIER_BOMB_Run_F_OV", 90);
	ACT_ANI_SOLDIER_BOMB_Run_F_OV.setPrecondition(ACT_RUN, true);
	ACT_ANI_SOLDIER_BOMB_Run_F_OV.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_SOLDIER_BOMB_Run_F_OV.setEffect(ANI_SOLDIER_BOMB_Run_F_OV, true);
	ACT_ANI_SOLDIER_BOMB_Run_F_OV.setEffect(SIGNAL_CHANGE_ANIMATION_FORCED, true);
	ACT_ANI_SOLDIER_BOMB_Run_F_OV.setEffect(SIGNAL_CHANGE_ANGLE, true);
	ACT_ANI_SOLDIER_BOMB_Run_F_OV.setEffect(CH_STATE_RUN, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_SOLDIER_BOMB_Run_F_OV);



	// ~~~~~~~~~~~~~~~~~~~~~~ ACT_ATTACK ~~~~~~~~~~~~~~~~~~~~~~
	//  기본 공격
	CAction ACT_ANI_SOLDIER_BOMB_Melee_R_01("ANI_SOLDIER_BOMB_Melee_R_01", 80);
	ACT_ANI_SOLDIER_BOMB_Melee_R_01.setPrecondition(ACT_ATTACK, true);
	ACT_ANI_SOLDIER_BOMB_Melee_R_01.setPrecondition(SIGNAL_SOLDIER_BOMB_DEFAULT_ATK, true);
	ACT_ANI_SOLDIER_BOMB_Melee_R_01.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);
	
	ACT_ANI_SOLDIER_BOMB_Melee_R_01.setEffect(CH_STATE_ATTACK, true);
	ACT_ANI_SOLDIER_BOMB_Melee_R_01.setEffect(SIGNAL_SOLDIER_BOMB_DEFAULT_ATK, false);
	ACT_ANI_SOLDIER_BOMB_Melee_R_01.setEffect(ANI_SOLDIER_BOMB_Melee_R_01, true);
	ACT_ANI_SOLDIER_BOMB_Melee_R_01.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_SOLDIER_BOMB_Melee_R_01);
	


	// ----- 콤보 공격1 -----
	CAction ACT_ANI_SOLDIER_BOMB_Melee_01("ANI_SOLDIER_BOMB_Melee_01", 80);
	ACT_ANI_SOLDIER_BOMB_Melee_01.setPrecondition(ACT_ATTACK, true);
	ACT_ANI_SOLDIER_BOMB_Melee_01.setPrecondition(SIGNAL_SOLDIER_BOMB_COMBO_ATK, true);
	ACT_ANI_SOLDIER_BOMB_Melee_01.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_SOLDIER_BOMB_Melee_01.setEffect(CH_STATE_ATTACK, true);
	ACT_ANI_SOLDIER_BOMB_Melee_01.setEffect(SIGNAL_SOLDIER_BOMB_COMBO_ATK, false);
	ACT_ANI_SOLDIER_BOMB_Melee_01.setEffect(ANI_SOLDIER_BOMB_Melee_01, true);
	ACT_ANI_SOLDIER_BOMB_Melee_01.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_SOLDIER_BOMB_Melee_01);

	// ----- 콤보 공격2 -----
	CAction ACT_ANI_SOLDIER_BOMB_Melee_02("ANI_SOLDIER_BOMB_Melee_02", 79);
	ACT_ANI_SOLDIER_BOMB_Melee_02.setPrecondition(ANI_SOLDIER_BOMB_Melee_01, true);
	ACT_ANI_SOLDIER_BOMB_Melee_02.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_SOLDIER_BOMB_Melee_02.setEffect(CH_STATE_ATTACK, true);
	ACT_ANI_SOLDIER_BOMB_Melee_02.setEffect(SIGNAL_SOLDIER_BOMB_COMBO_ATK, false);
	ACT_ANI_SOLDIER_BOMB_Melee_02.setEffect(ANI_SOLDIER_BOMB_Melee_02, true);
	ACT_ANI_SOLDIER_BOMB_Melee_02.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_SOLDIER_BOMB_Melee_02);

	// ----- 콤보 공격3 -----
	CAction ACT_ANI_SOLDIER_BOMB_Melee_03("ANI_SOLDIER_BOMB_Melee_03", 78);
	ACT_ANI_SOLDIER_BOMB_Melee_03.setPrecondition(ANI_SOLDIER_BOMB_Melee_02, true);
	ACT_ANI_SOLDIER_BOMB_Melee_03.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_SOLDIER_BOMB_Melee_03.setEffect(CH_STATE_ATTACK, true);
	ACT_ANI_SOLDIER_BOMB_Melee_03.setEffect(SIGNAL_SOLDIER_BOMB_COMBO_ATK, false);
	ACT_ANI_SOLDIER_BOMB_Melee_03.setEffect(ANI_SOLDIER_BOMB_Melee_03, true);
	ACT_ANI_SOLDIER_BOMB_Melee_03.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_SOLDIER_BOMB_Melee_03);



	

	//  스폐셜 공격   점프해서 내려찍기
	CAction ACT_ANI_SOLDIER_BOMB_Melee_Special_01("ANI_SOLDIER_BOMB_Melee_Special_01", 80);
	ACT_ANI_SOLDIER_BOMB_Melee_Special_01.setPrecondition(CH_STATE_NEAR_TARGET, false);
	ACT_ANI_SOLDIER_BOMB_Melee_Special_01.setPrecondition(SIGNAL_SOLDIER_BOMB_JUMP_ATK, true);
	ACT_ANI_SOLDIER_BOMB_Melee_Special_01.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);
	
	ACT_ANI_SOLDIER_BOMB_Melee_Special_01.setEffect(CH_STATE_ATTACK, true);
	ACT_ANI_SOLDIER_BOMB_Melee_Special_01.setEffect(SIGNAL_SOLDIER_BOMB_JUMP_ATK, false);
	ACT_ANI_SOLDIER_BOMB_Melee_Special_01.setEffect(ANI_SOLDIER_BOMB_Melee_Special_01, true);
	ACT_ANI_SOLDIER_BOMB_Melee_Special_01.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_SOLDIER_BOMB_Melee_Special_01);


	//  폭탄 던지기 ★
	CAction ACT_ANI_SOLDIER_BOMB_Throw_L01("ANI_SOLDIER_BOMB_Throw_L01", 80);
	ACT_ANI_SOLDIER_BOMB_Throw_L01.setPrecondition(CH_STATE_MID_TARGET, false);
	ACT_ANI_SOLDIER_BOMB_Throw_L01.setPrecondition(SIGNAL_SOLDIER_BOMB_THROW_BOMB, true);
	ACT_ANI_SOLDIER_BOMB_Throw_L01.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_SOLDIER_BOMB_Throw_L01.setEffect(CH_STATE_ATTACK, true);
	ACT_ANI_SOLDIER_BOMB_Throw_L01.setEffect(SIGNAL_SOLDIER_BOMB_THROW_BOMB, false);
	ACT_ANI_SOLDIER_BOMB_Throw_L01.setEffect(SIGNAL_SOLDIER_BOMB_THROWING_KNOW, true);
	ACT_ANI_SOLDIER_BOMB_Throw_L01.setEffect(ANI_SOLDIER_BOMB_Throw_L01, true);
	ACT_ANI_SOLDIER_BOMB_Throw_L01.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_SOLDIER_BOMB_Throw_L01);

	

	// ~~~~~~~~~~~~~~~~~~~~~~ ACT_ATTACK ~~~~~~~~~~~~~~~~~~~~~~

	// ~~~~~~~~~~~~~~~~~~~~~~ ACT_DEFENSE ~~~~~~~~~~~~~~~~~~~~~~

	// ----- 기본 방어1 -----
	CAction ACT_ANI_SOLDIER_BOMB_Block_F_R2R("ANI_SOLDIER_BOMB_Block_F_R2R", 80);
	ACT_ANI_SOLDIER_BOMB_Block_F_R2R.setPrecondition(SIGNAL_SHIELD_ON, true);
	ACT_ANI_SOLDIER_BOMB_Block_F_R2R.setPrecondition(ACT_HIT, true);
	ACT_ANI_SOLDIER_BOMB_Block_F_R2R.setPrecondition(SIGNAL_HIT_ENEMY, true);
	ACT_ANI_SOLDIER_BOMB_Block_F_R2R.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_SOLDIER_BOMB_Block_F_R2R.setEffect(SIGNAL_HIT_ENEMY, false);
	ACT_ANI_SOLDIER_BOMB_Block_F_R2R.setEffect(ANI_SOLDIER_BOMB_Block_F_R2R, true);
	ACT_ANI_SOLDIER_BOMB_Block_F_R2R.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	ACT_ANI_SOLDIER_BOMB_Block_F_R2R.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
	ACT_ANI_SOLDIER_BOMB_Block_F_R2R.setEffect(SIGNAL_CHANGE_ANGLE, true);
	ACT_ANI_SOLDIER_BOMB_Block_F_R2R.setEffect(SIGNAL_CHANGE_TARGET_NEAR_ENEMY, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_SOLDIER_BOMB_Block_F_R2R);

	// ----- 기본 방어2 -----
	CAction ACT_ANI_SOLDIER_BOMB_Block_L_R2R("ANI_SOLDIER_BOMB_Block_L_R2R", 79);
	ACT_ANI_SOLDIER_BOMB_Block_L_R2R.setPrecondition(SIGNAL_SHIELD_ON, true);
	ACT_ANI_SOLDIER_BOMB_Block_L_R2R.setPrecondition(ACT_HIT, true);
	ACT_ANI_SOLDIER_BOMB_Block_L_R2R.setPrecondition(ANI_SOLDIER_BOMB_Block_F_R2R, true);
	ACT_ANI_SOLDIER_BOMB_Block_L_R2R.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_SOLDIER_BOMB_Block_L_R2R.setEffect(SIGNAL_HIT_ENEMY, false);
	ACT_ANI_SOLDIER_BOMB_Block_L_R2R.setEffect(ANI_SOLDIER_BOMB_Block_L_R2R, true);
	ACT_ANI_SOLDIER_BOMB_Block_L_R2R.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	ACT_ANI_SOLDIER_BOMB_Block_L_R2R.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
	ACT_ANI_SOLDIER_BOMB_Block_L_R2R.setEffect(SIGNAL_CHANGE_ANGLE, true);
	ACT_ANI_SOLDIER_BOMB_Block_L_R2R.setEffect(SIGNAL_CHANGE_TARGET_NEAR_ENEMY, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_SOLDIER_BOMB_Block_L_R2R);

	// ----- 기본 방어3 -----
	CAction ACT_ANI_SOLDIER_BOMB_Block_R_R2R("ANI_SOLDIER_BOMB_Block_R_R2R", 78);
	ACT_ANI_SOLDIER_BOMB_Block_R_R2R.setPrecondition(SIGNAL_SHIELD_ON, true);
	ACT_ANI_SOLDIER_BOMB_Block_R_R2R.setPrecondition(ACT_HIT, true);
	ACT_ANI_SOLDIER_BOMB_Block_R_R2R.setPrecondition(ANI_SOLDIER_BOMB_Block_L_R2R, true);
	ACT_ANI_SOLDIER_BOMB_Block_R_R2R.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_SOLDIER_BOMB_Block_R_R2R.setEffect(SIGNAL_HIT_ENEMY, false);
	ACT_ANI_SOLDIER_BOMB_Block_R_R2R.setEffect(ANI_SOLDIER_BOMB_Block_R_R2R, true);
	ACT_ANI_SOLDIER_BOMB_Block_R_R2R.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	ACT_ANI_SOLDIER_BOMB_Block_R_R2R.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
	ACT_ANI_SOLDIER_BOMB_Block_R_R2R.setEffect(SIGNAL_CHANGE_ANGLE, true);
	ACT_ANI_SOLDIER_BOMB_Block_R_R2R.setEffect(SIGNAL_CHANGE_TARGET_NEAR_ENEMY, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_SOLDIER_BOMB_Block_R_R2R);

	// ----- 기본 방어 깨짐 -----
	CAction ACT_ANI_SOLDIER_BOMB_Block_F_R2R_01("ANI_SOLDIER_BOMB_Block_F_R2R_01", 77);
	ACT_ANI_SOLDIER_BOMB_Block_F_R2R_01.setPrecondition(SIGNAL_SHIELD_ON, true);
	ACT_ANI_SOLDIER_BOMB_Block_F_R2R_01.setPrecondition(SIGNAL_SHIELD_BREAK_ON, true);
	ACT_ANI_SOLDIER_BOMB_Block_F_R2R_01.setPrecondition(ACT_HIT, true);
	ACT_ANI_SOLDIER_BOMB_Block_F_R2R_01.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_SOLDIER_BOMB_Block_F_R2R_01.setEffect(SIGNAL_SHIELD_BREAK_ON, false);
	ACT_ANI_SOLDIER_BOMB_Block_F_R2R_01.setEffect(SIGNAL_HIT_ENEMY, false);
	ACT_ANI_SOLDIER_BOMB_Block_F_R2R_01.setEffect(ANI_SOLDIER_BOMB_Block_F_R2R_01, true);
	ACT_ANI_SOLDIER_BOMB_Block_F_R2R_01.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	ACT_ANI_SOLDIER_BOMB_Block_F_R2R_01.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
	ACT_ANI_SOLDIER_BOMB_Block_F_R2R_01.setEffect(SIGNAL_CHANGE_ANGLE, true);
	ACT_ANI_SOLDIER_BOMB_Block_F_R2R_01.setEffect(SIGNAL_CHANGE_TARGET_NEAR_ENEMY, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_SOLDIER_BOMB_Block_F_R2R_01);


	// ----- 회피 B -----        
	CAction ACT_ANI_SOLDIER_BOMB_Evade_B_L01("ANI_SOLDIER_BOMB_Evade_B_L01", 76);
	ACT_ANI_SOLDIER_BOMB_Evade_B_L01.setPrecondition(SIGNAL_EVADE_ON, true);
	ACT_ANI_SOLDIER_BOMB_Evade_B_L01.setPrecondition(SIGNAL_EVADE_BACK, true);
	ACT_ANI_SOLDIER_BOMB_Evade_B_L01.setPrecondition(ACT_HIT, true);
	ACT_ANI_SOLDIER_BOMB_Evade_B_L01.setPrecondition(SIGNAL_HIT_ENEMY, true);
	ACT_ANI_SOLDIER_BOMB_Evade_B_L01.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_SOLDIER_BOMB_Evade_B_L01.setEffect(SIGNAL_EVADE_BACK, false);
	ACT_ANI_SOLDIER_BOMB_Evade_B_L01.setEffect(SIGNAL_HIT_ENEMY, false);
	ACT_ANI_SOLDIER_BOMB_Evade_B_L01.setEffect(ANI_SOLDIER_BOMB_Evade_B_L01, true);
	ACT_ANI_SOLDIER_BOMB_Evade_B_L01.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	ACT_ANI_SOLDIER_BOMB_Evade_B_L01.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
	ACT_ANI_SOLDIER_BOMB_Evade_B_L01.setEffect(SIGNAL_CHANGE_ANGLE, true);
	ACT_ANI_SOLDIER_BOMB_Evade_B_L01.setEffect(SIGNAL_CHANGE_TARGET_NEAR_ENEMY, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_SOLDIER_BOMB_Evade_B_L01);


	// ----- 회피 R-----        
	CAction ACT_ANI_SOLDIER_BOMB_Evade_JumpKick_R01_x("ANI_SOLDIER_BOMB_Evade_JumpKick_R01_x", 76);
	ACT_ANI_SOLDIER_BOMB_Evade_JumpKick_R01_x.setPrecondition(SIGNAL_EVADE_ON, true);
	ACT_ANI_SOLDIER_BOMB_Evade_JumpKick_R01_x.setPrecondition(SIGNAL_EVADE_RIGHT, true);
	ACT_ANI_SOLDIER_BOMB_Evade_JumpKick_R01_x.setPrecondition(ACT_HIT, true);
	ACT_ANI_SOLDIER_BOMB_Evade_JumpKick_R01_x.setPrecondition(SIGNAL_HIT_ENEMY, true);
	ACT_ANI_SOLDIER_BOMB_Evade_JumpKick_R01_x.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_SOLDIER_BOMB_Evade_JumpKick_R01_x.setEffect(SIGNAL_EVADE_RIGHT, false);
	ACT_ANI_SOLDIER_BOMB_Evade_JumpKick_R01_x.setEffect(SIGNAL_HIT_ENEMY, false);
	ACT_ANI_SOLDIER_BOMB_Evade_JumpKick_R01_x.setEffect(ANI_SOLDIER_BOMB_Evade_JumpKick_R01_x, true);
	ACT_ANI_SOLDIER_BOMB_Evade_JumpKick_R01_x.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	ACT_ANI_SOLDIER_BOMB_Evade_JumpKick_R01_x.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
	ACT_ANI_SOLDIER_BOMB_Evade_JumpKick_R01_x.setEffect(SIGNAL_CHANGE_ANGLE, true);
	ACT_ANI_SOLDIER_BOMB_Evade_JumpKick_R01_x.setEffect(SIGNAL_CHANGE_TARGET_NEAR_ENEMY, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_SOLDIER_BOMB_Evade_JumpKick_R01_x);


	// ----- 회피 L-----        
	CAction ACT_ANI_SOLDIER_BOMB_Evade_JumpKick_L01_x("ANI_SOLDIER_BOMB_Evade_JumpKick_L01_x", 76);
	ACT_ANI_SOLDIER_BOMB_Evade_JumpKick_L01_x.setPrecondition(SIGNAL_EVADE_ON, true);
	ACT_ANI_SOLDIER_BOMB_Evade_JumpKick_L01_x.setPrecondition(SIGNAL_EVADE_LEFT, true);
	ACT_ANI_SOLDIER_BOMB_Evade_JumpKick_L01_x.setPrecondition(ACT_HIT, true);
	ACT_ANI_SOLDIER_BOMB_Evade_JumpKick_L01_x.setPrecondition(SIGNAL_HIT_ENEMY, true);
	ACT_ANI_SOLDIER_BOMB_Evade_JumpKick_L01_x.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_SOLDIER_BOMB_Evade_JumpKick_L01_x.setEffect(SIGNAL_EVADE_LEFT, false);
	ACT_ANI_SOLDIER_BOMB_Evade_JumpKick_L01_x.setEffect(SIGNAL_HIT_ENEMY, false);
	ACT_ANI_SOLDIER_BOMB_Evade_JumpKick_L01_x.setEffect(ANI_SOLDIER_BOMB_Evade_JumpKick_L01_x, true);
	ACT_ANI_SOLDIER_BOMB_Evade_JumpKick_L01_x.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	ACT_ANI_SOLDIER_BOMB_Evade_JumpKick_L01_x.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
	ACT_ANI_SOLDIER_BOMB_Evade_JumpKick_L01_x.setEffect(SIGNAL_CHANGE_ANGLE, true);
	ACT_ANI_SOLDIER_BOMB_Evade_JumpKick_L01_x.setEffect(SIGNAL_CHANGE_TARGET_NEAR_ENEMY, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_SOLDIER_BOMB_Evade_JumpKick_L01_x);



	// ~~~~~~~~~~~~~~~~~~~~~~ ACT_DEFENSE ~~~~~~~~~~~~~~~~~~~~~~

	// ~~~~~~~~~~~~~~~~~~~~~~ ACT_HIT ~~~~~~~~~~~~~~~~~~~~~~

	{
		// ----- 기본 히트1 -----
		CAction ACT_ANI_SOLDIER_BOMB_Dmg_F("ANI_SOLDIER_BOMB_Dmg_F", 81);
		ACT_ANI_SOLDIER_BOMB_Dmg_F.setPrecondition(SIGNAL_EVADE_ON, false);
		ACT_ANI_SOLDIER_BOMB_Dmg_F.setPrecondition(SIGNAL_SHIELD_ON, false);
		ACT_ANI_SOLDIER_BOMB_Dmg_F.setPrecondition(ACT_HIT, true);
		ACT_ANI_SOLDIER_BOMB_Dmg_F.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

		ACT_ANI_SOLDIER_BOMB_Dmg_F.setEffect(SIGNAL_HIT_ENEMY, false);
		ACT_ANI_SOLDIER_BOMB_Dmg_F.setEffect(ANI_SOLDIER_BOMB_Dmg_F, true);
		ACT_ANI_SOLDIER_BOMB_Dmg_F.setEffect(SIGNAL_CHANGE_ANIMATION, false);
		ACT_ANI_SOLDIER_BOMB_Dmg_F.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
		ACT_ANI_SOLDIER_BOMB_Dmg_F.setEffect(SIGNAL_CHANGE_ANGLE, true);
		ACT_ANI_SOLDIER_BOMB_Dmg_F.setEffect(SIGNAL_CHANGE_TARGET_NEAR_ENEMY, true);
		m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_SOLDIER_BOMB_Dmg_F);
	}

	// ~~~~~~~~~~~~~~~~~~~~~~ ACT_HIT ~~~~~~~~~~~~~~~~~~~~~~

	//------------------------Die--------------------------

	// ----- 뒤로 날아감( 맞아서 ).. -----
	{
		CAction ACT_ANI_SOLDIER_BOMB_Kndown_Gut_x("ANI_SOLDIER_BOMB_Kndown_Gut_x", 80);
		ACT_ANI_SOLDIER_BOMB_Kndown_Gut_x.setPrecondition(ACT_DEATH, true);
		ACT_ANI_SOLDIER_BOMB_Kndown_Gut_x.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

		ACT_ANI_SOLDIER_BOMB_Kndown_Gut_x.setEffect(ANI_SOLDIER_BOMB_Kndown_Gut_x, true);
		ACT_ANI_SOLDIER_BOMB_Kndown_Gut_x.setEffect(SIGNAL_CHANGE_ANIMATION, false);
		ACT_ANI_SOLDIER_BOMB_Kndown_Gut_x.setEffect(CH_STATE_DIE, true);
		m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_SOLDIER_BOMB_Kndown_Gut_x);
	}

	// ----- 뒤로 죽음1.. -----
	{
		CAction ACT_ANI_SOLDIER_BOMB_Kndown_Loop_b("ANI_SOLDIER_BOMB_Kndown_Loop_b", 79);
		ACT_ANI_SOLDIER_BOMB_Kndown_Loop_b.setPrecondition(ACT_DEATH, true);
		ACT_ANI_SOLDIER_BOMB_Kndown_Loop_b.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);
		ACT_ANI_SOLDIER_BOMB_Kndown_Loop_b.setPrecondition(ANI_SOLDIER_BOMB_Kndown_Gut_x, true);

		ACT_ANI_SOLDIER_BOMB_Kndown_Loop_b.setEffect(ANI_SOLDIER_BOMB_Kndown_Loop_b, true);
		ACT_ANI_SOLDIER_BOMB_Kndown_Loop_b.setEffect(SIGNAL_CHANGE_ANIMATION, false);
		ACT_ANI_SOLDIER_BOMB_Kndown_Loop_b.setEffect(CH_STATE_DIE, true);
		m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_SOLDIER_BOMB_Kndown_Loop_b);
	}

	//------------------------Die--------------------------




	// %%%%%%%%%%%%%%%%%%%%%%%%%% 애니메이션 결정 액션 %%%%%%%%%%%%%%%%%%%%%%%%%%

	return NOERROR;

}

void CSoldier_Bomb::SignalUpdate(CCharacterGroup::CHARDATA* ptChar)
{
	++m_iSelect_Atk_Count;

	if (12 <= m_iSelect_Atk_Count)
		m_iSelect_Atk_Count = 1;


	if (2 >= m_iSelect_Atk_Count)
		m_iAtk_Count = Soldier_Bomb_Defalut_Atk;
	else if (3 == m_iSelect_Atk_Count)
		m_iAtk_Count = Soldier_Bomb_Combo_Atk;
	else if (4 == m_iSelect_Atk_Count || 5 == m_iSelect_Atk_Count)
		m_iAtk_Count = Soldier_Bomb_Defalut_Atk;
	else if (6 == m_iSelect_Atk_Count)
		m_iAtk_Count = Soldier_Bomb_Jump_Atk;
	else if (7 == m_iSelect_Atk_Count || 8 == m_iSelect_Atk_Count)
		m_iAtk_Count = Soldier_Bomb_Defalut_Atk;
	else if (9 == m_iSelect_Atk_Count)
		m_iAtk_Count = Soldier_Bomb_Combo_Atk;
	else if (10 == m_iSelect_Atk_Count)
		m_iAtk_Count = Soldier_Bomb_Defalut_Atk;
	else if (11 == m_iSelect_Atk_Count)
		m_iAtk_Count = Soldier_Bomb_Throw_Bomb;


	m_tInfo.m_WorldState.setVariable(SIGNAL_SOLDIER_BOMB_DEFAULT_ATK, false);
	m_tInfo.m_WorldState.setVariable(SIGNAL_SOLDIER_BOMB_COMBO_ATK, false);
	m_tInfo.m_WorldState.setVariable(SIGNAL_SOLDIER_BOMB_JUMP_ATK, false);
	m_tInfo.m_WorldState.setVariable(SIGNAL_SOLDIER_BOMB_THROW_BOMB, false);
	

	if (Soldier_Bomb_Defalut_Atk == m_iAtk_Count)
		m_tInfo.m_WorldState.setVariable(SIGNAL_SOLDIER_BOMB_DEFAULT_ATK, true);
	else if (Soldier_Bomb_Combo_Atk == m_iAtk_Count)
		m_tInfo.m_WorldState.setVariable(SIGNAL_SOLDIER_BOMB_COMBO_ATK, true);
	else if (Soldier_Bomb_Jump_Atk == m_iAtk_Count)
		m_tInfo.m_WorldState.setVariable(SIGNAL_SOLDIER_BOMB_JUMP_ATK, true);
	else if (Soldier_Bomb_Throw_Bomb == m_iAtk_Count)
		m_tInfo.m_WorldState.setVariable(SIGNAL_SOLDIER_BOMB_THROW_BOMB, true);

	_uint iCount = CRandom::GetInstance()->GetValue(0, 6);

	ptChar->fTime_Atk = 8.f + (float)iCount;
}

void CSoldier_Bomb::Move_SignalUpdate(CCharacterGroup::CHARDATA* ptChar)
{
	if (true == m_tInfo.m_WorldState.getVariable(SIGNAL_SOLDIER_BOMB_THROW_BOMB))
	{
		if (true == m_tInfo.m_WorldState.getVariable(SIGNAL_SOLDIER_BOMB_DEFAULT_ATK))
		{
			m_tInfo.m_WorldState.setVariable(SIGNAL_SOLDIER_BOMB_THROW_BOMB, false);
		}


		m_tInfo.m_WorldState.setVariable(SIGNAL_CHANGE_DIR_BACK, true);

		ptChar->fTime = 0.5f;
	}
	else if (true == m_tInfo.m_WorldState.getVariable(SIGNAL_SOLDIER_BOMB_JUMP_ATK))
	{
		if (true == m_tInfo.m_WorldState.getVariable(SIGNAL_SOLDIER_BOMB_DEFAULT_ATK))
		{
			m_tInfo.m_WorldState.setVariable(SIGNAL_SOLDIER_BOMB_JUMP_ATK, false);
		}


		m_tInfo.m_WorldState.setVariable(SIGNAL_CHANGE_DIR_BACK, true);

		ptChar->fTime = 0.5f;
	}
	else
	{
		if (true == m_tInfo.m_WorldState.getVariable(CH_STATE_CLOSE_TARGET))
		{
			_uint iCount = 0;

			while (true)
			{
				iCount = CRandom::GetInstance()->GetValue(0, 4);

				if (0 != iCount)
					break;
			}

			m_tInfo.m_WorldState.setVariable(SIGNAL_CHANGE_DIR_FRONT + iCount, true);
		}
		else if (false == m_tInfo.m_WorldState.getVariable(CH_STATE_CLOSE_TARGET))
		{
			if (false == m_tInfo.m_WorldState.getVariable(CH_STATE_NEAR_TARGET))
			{
				_uint iCount = 0;

				while (true)
				{
					iCount = CRandom::GetInstance()->GetValue(0, 4);

					if (1 != iCount)
						break;
				}

				m_tInfo.m_WorldState.setVariable(SIGNAL_CHANGE_DIR_FRONT + iCount, true);
			}
			else
			{
				_uint iCount = CRandom::GetInstance()->GetValue(0, 4);

				m_tInfo.m_WorldState.setVariable(SIGNAL_CHANGE_DIR_FRONT + iCount, true);
			}

		}


		ptChar->fTime = 1.f;
	}

}


HRESULT CSoldier_Bomb::SetUp_OnConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorld");
	//m_pTransformCom->SetUp_OnShader_FORCED(pEffect, "g_matWorld");

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);	

	Safe_Release(pEffect);

	return NOERROR;
}

void CSoldier_Bomb::Create_Bomb(void)
{
	m_pBomb = CBomb::Create(m_pGraphicDev, m_pBombHand_mat, m_pTransformCom);

	if (nullptr == m_pBomb)
	{
		MSG_BOX("CBomb Created Failed");
		return;
	}
}


CSoldier_Bomb * CSoldier_Bomb::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CSoldier_Bomb *	pInstance = new CSoldier_Bomb(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MSG_BOX("CSoldier_Bomb Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}

Engine::EStatus CSoldier_Bomb::Move_Forward(float fScale /*= 1.f*/)
{
	if (nullptr == m_pTransformCom)
		return EStatus::Failure;

	m_pTransformCom->Go_Straight(
		m_tInfo.m_fSpeed * fScale,
		m_pTimer->Get_TimeDelta());

	return EStatus::Success;
}

Engine::EStatus CSoldier_Bomb::Move_Right(float fScale /*= 1.f*/)
{
	if (nullptr == m_pTransformCom)
		return EStatus::Failure;

	m_pTransformCom->Go_Right(
		m_tInfo.m_fSpeed * fScale,
		m_pTimer->Get_TimeDelta());

	return EStatus::Success;
}


#ifndef _FINAL
void CSoldier_Bomb::Debug_Render(void)
{
	CDebug::GetInstance()->Add_DebugText("CSoldier_Bomb");

}
#endif
_ulong CSoldier_Bomb::Free(void)
{
	_ulong		dwRefCnt = 0;


	if (nullptr != m_pBomb)
		Safe_Release(m_pBomb);
	

	dwRefCnt = CMonster::Free();

	return dwRefCnt;
}
