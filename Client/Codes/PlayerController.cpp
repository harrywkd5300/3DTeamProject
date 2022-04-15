#include "stdafx.h"
#include "PlayerController.h"
#include "Character.h"
#include "Component_Manager.h"
#include "Object_Manager.h"
#include "Camera_Manager.h"
#include "Subject_Manager.h"
#include "Management.h"
#include "Scene.h"

CPlayerController::CPlayerController(LPDIRECT3DDEVICE9 pGraphicDev)
	: CController(pGraphicDev)
	,m_pInput(CInput_Device::GetInstance())
{
	m_pInput->AddRef();
#ifndef _FINAL
	Tag(L"CPlayerController", std::bind(&CPlayerController::Debug_Render, this));
#endif

}

CPlayerController::CPlayerController(const CPlayerController & rhs)
	: CController(rhs)
	,m_pInput(CInput_Device::GetInstance())
{
	m_pInput->AddRef();
#ifndef _FINAL
	Tag(L"CPlayerController", std::bind(&CPlayerController::Debug_Render, this));
#endif

}

HRESULT CPlayerController::Ready_Controller(void)
{
	if (FAILED(CController::Ready_GoalState()))
	{
		MSG_BOX("CPlayerController::Ready_GoalState() Failed");
		return E_FAIL;
	}
	return NOERROR;
}


void CPlayerController::Update_Component(const _float & fTimeDelta)
{
	if (m_pCharacter == nullptr)
		return;

	// 현재 조종중인 플레이어가 사망했다면
	if (m_pCharacter->Get_IsDead()) 
	{
		CGameObject* pPlayer = nullptr;
		// 스테이지의 플레이어 검색
		for (int i = 0; i < 4; ++i)
		{
			pPlayer = CObject_Manager::GetInstance()->Get_FindObject(SCENE_STATIC, L"Player", i);
			if (pPlayer != nullptr)
			{
				// 살아있는 플레이어 발견
				if (pPlayer->Get_IsDead() == false) 
				{
					Possess(dynamic_cast<CCharacter*>(pPlayer));
					return;
				}
			}
		}
		UnPossess();
		return;
	}

	// 현재 카메라를 변경 할 수 있는 상황이면 플레이어 카메라를 셋팅.
	if (CCamera_Manager::GetInstance()->Get_IsChangeCamera())
	{
		// 현재 카메라가 조중중인 플레이어의 카메라가 아니라면 셋팅
		if (CCamera_Manager::GetInstance()->Get_CurrentCamera() != m_pCharacter->Get_Camera())
		{
			CCamera_Manager::GetInstance()->Set_CurrentCamera(m_pCharacter->Get_Camera());
		}
	}

	// 카메라 룩방향 - 플레이어 룩방향 사이 각도 계산
	_vec3 vPlayerLook = *m_pCharacter->Get_Transform()->Get_WorldLookVec();
	vPlayerLook.y = 0.f;
	D3DXVec3Normalize(&vPlayerLook, &vPlayerLook);
	_vec3 vCameraLook = CCamera_Manager::GetInstance()->Get_Direction_NotY();
	vCameraLook.y = 0.f;
	D3DXVec3Normalize(&vCameraLook, &vCameraLook);
	m_pCharacter_Info->m_fAngleGap_CameraToPlayer = D3DXToDegree(acosf(D3DXVec3Dot(&vCameraLook, &vPlayerLook)));
	if (isnan(m_pCharacter_Info->m_fAngleGap_CameraToPlayer))
		m_pCharacter_Info->m_fAngleGap_CameraToPlayer = 0.f;



	// ----------- 초기화 -----------	
	Get_TargetInfo(m_pCharacter);
	// 액션 시그널 초기화
	m_pCharacter_Info->m_WorldState.setVariable(ACT_TYPE_START, ACT_TYPE_END, false);
	Set_Bool_BitTable( InputKey_Action, false );
	// 방향 시그널 초기화
	m_pCharacter_Info->m_WorldState.setVariable(DEST_DIR_0, DEST_DIR_315, false);
	// 플레이어 타겟 -> 적 또는 아군
	m_pCharacter_Info->m_WorldState.setVariable(TARGET_NEAR_ENEMY, true);

	// 현재 타겟중인 캐릭터가 사망했다면
	if (m_pTarget != nullptr)
	{
		if (m_pTarget->Get_IsDead())
		{
			m_pTarget = nullptr;
			m_pCharacter_Info->m_WorldState.setVariable(TARGET_TYPE_START, TARGET_TYPE_END, false);
			m_pCharacter_Info->m_WorldState.setVariable(TARGET_NONE, true);
		}
	}
	// ----------- 초기화 -----------	

	// ------------------- 월드 스테이트 외부 변화 시작 -------------------


	const _tchar* pEnemyTargetTag = nullptr;	// 조종 캐릭터의 적 레이어 태그
	const _tchar* pMyTag = nullptr;	// 조종 캐릭터의 레이어 태그
	_uint iSceneNumber = 0;	// 씬넘버
	_ulong dwChangeActType = 0;	// 바꿀 Act Type..

	// 조종하는 캐릭터 타입 설정
	if (m_pCharacter_Info->m_eCharType == CHARINFO::CHAR_PLAYER)
	{
		iSceneNumber = CManagement::GetInstance()->Get_Current_Scene()->Get_SceneIndex();
		pEnemyTargetTag = L"Monster";
		pMyTag = L"Player";
	}
	else if (m_pCharacter_Info->m_eCharType == CHARINFO::CHAR_MONSTER)
	{
		iSceneNumber = SCENETYPE::SCENE_STATIC;
		pEnemyTargetTag = L"Player";
		pMyTag = L"Monster";
	}

	//// ---- 가까운 적 위치 갱신 시작 ----
	CCharacter* pNearEnemy = nullptr;
	//CCharacter* pNearEnemy = dynamic_cast<CCharacter*>(CObject_Manager::GetInstance()->Get_FindObject_Near_Position(
	//	iSceneNumber, pEnemyTargetTag, m_pCharacter));
	//float fNearEnemyToMeDist = 99999.f;
	//
	//if (pNearEnemy != nullptr)
	//{
	//	fNearEnemyToMeDist = D3DXVec3Length(&(*pNearEnemy->Get_Transform()->Get_Infomation(CTransform::INFO_POSITION)
	//										   - *m_pCharacter->Get_Transform()->Get_Infomation(CTransform::INFO_POSITION)));
	//
	//	// 계산된 거리를 월드 스테이트에 반영
	//	if (fNearEnemyToMeDist < m_pCharacter_Info->m_fRange_Close)
	//		m_pCharacter_Info->m_WorldState.setVariable(CH_STATE_CLOSE_NEAR_ENEMY, CH_STATE_TOOFAR_NEAR_ENEMY, true);
	//	else if (fNearEnemyToMeDist < m_pCharacter_Info->m_fRange_Near)
	//		m_pCharacter_Info->m_WorldState.setVariable(CH_STATE_NEAR_NEAR_ENEMY, CH_STATE_TOOFAR_NEAR_ENEMY, true);
	//	else if (fNearEnemyToMeDist < m_pCharacter_Info->m_fRange_Mid)
	//		m_pCharacter_Info->m_WorldState.setVariable(CH_STATE_MID_NEAR_ENEMY, CH_STATE_TOOFAR_NEAR_ENEMY, true);
	//	else if (fNearEnemyToMeDist < m_pCharacter_Info->m_fRange_Far)
	//		m_pCharacter_Info->m_WorldState.setVariable(CH_STATE_FAR_NEAR_ENEMY, CH_STATE_TOOFAR_NEAR_ENEMY, true);
	//	else if (fNearEnemyToMeDist < m_pCharacter_Info->m_fRange_TooFar)
	//		m_pCharacter_Info->m_WorldState.setVariable(CH_STATE_TOOFAR_NEAR_ENEMY, true);
	//
	//	if (fNearEnemyToMeDist < m_pCharacter_Info->m_fRange_FoundEnemy)
	//		m_pCharacter_Info->m_WorldState.setVariable(CH_STATE_FOUND_ENEMY, true);
	//	//else
	//	//	m_pCharacter_Info->m_WorldState.setVariable(CH_STATE_FOUND_ENEMY, false);
	//}
	//// ---- 가까운 적 위치 갱신 끝 ----

	// ---- 가까운 팀 위치 갱신 시작 ----
	CCharacter* pNearTeam = dynamic_cast<CCharacter*>(CObject_Manager::GetInstance()->Get_FindObject_Near_Position(
		iSceneNumber, pMyTag, m_pCharacter));
	float fNearTeamToMeDist = 99999.f;


	// ---- 가까운 팀 위치 갱신 끝 ----

	// ---- 타겟 위치 갱신 시작 ----
	// 타겟과의 거리 초기화
	float fTargetToMeDist = 0.f;
	m_pCharacter_Info->m_WorldState.setVariable(CH_STATE_CLOSE_TARGET, CH_STATE_TOOFAR_TARGET, false);

	// 타겟과 의 거리 계산
	if (m_pTarget != nullptr)
	{
		fTargetToMeDist = D3DXVec3Length(&(*m_pTarget->Get_Transform()->Get_Infomation(CTransform::INFO_POSITION)
											- *m_pCharacter->Get_Transform()->Get_Infomation(CTransform::INFO_POSITION)));

		// 계산된 거리를 월드 스테이트에 반영
		if (fTargetToMeDist < m_pCharacter_Info->m_fRange_TooFar)
		{
			m_pCharacter_Info->m_WorldState.setVariable(CH_STATE_TOOFAR_TARGET, true);
		}
		if (fTargetToMeDist < m_pCharacter_Info->m_fRange_Far)
		{
			m_pCharacter_Info->m_WorldState.setVariable(CH_STATE_FAR_TARGET, true);
		}
		if (fTargetToMeDist < m_pCharacter_Info->m_fRange_Mid)
		{
			m_pCharacter_Info->m_WorldState.setVariable(CH_STATE_MID_TARGET, true);
		}
		if (fTargetToMeDist < m_pCharacter_Info->m_fRange_Near)
		{
			m_pCharacter_Info->m_WorldState.setVariable(CH_STATE_NEAR_TARGET, true);
		}
		if (fTargetToMeDist < m_pCharacter_Info->m_fRange_Close)
		{
			m_pCharacter_Info->m_WorldState.setVariable(CH_STATE_CLOSE_TARGET, true);
		}
	}
	// ---- 타겟 위치 갱신 끝 ----

	// ------------------- 월드 스테이트 외부 변화 끝 -------------------


	// ----------------- 행동 선택 키입력 시작 -------------------
	// 대쉬 키 입력 체크
	if (m_pInput->Get_DIKeyState(DIK_LSHIFT))
	{
		m_pCharacter_Info->m_WorldState.setVariable(ACT_SPRINT, true);
	}

	if (false)
	{

	}
	// 상호작용 체크..
	else if ( nullptr != CAI_Manager::GetInstance()->plan( m_pCharacter_Info->m_WorldState, m_GoalState_Interaction, m_pCharacter_Info->m_vecAction_Act, &m_pCharacter_Info->m_CurAction_Act ) )
	{
		++m_iAniPlanCount;
		m_pCharacter_Info->m_CurAction_Act.Set_ActEffect(m_pCharacter_Info->m_WorldState);
	}
	// 가드(카운터 준비 동작) 체크..
	else if( m_pInput->Get_DIMouseState(CInput_Device::DIM_LBUTTON) && m_pInput->Get_DIMouseState(CInput_Device::DIM_RBUTTON) )
	{
		Set_Bool_BitTable( InputKey_Guard, true );
		Set_Bool_BitTable( IsGuardState, true );
	}
	// 히트 체크
	else if (nullptr != CAI_Manager::GetInstance()->plan(m_pCharacter_Info->m_WorldState, m_GoalState_Hit, m_pCharacter_Info->m_vecAction_Act, &m_pCharacter_Info->m_CurAction_Act))
	{
		++m_iAniPlanCount;
		m_pCharacter_Info->m_CurAction_Act.Set_ActEffect(m_pCharacter_Info->m_WorldState);
	}
	// 상호작용 / 회피 / 점프 키 입력
	else if (m_pInput->Get_DIKeyState(DIK_SPACE))
	{
		// 상호작용 액션
		if (nullptr != CAI_Manager::GetInstance()->plan(m_pCharacter_Info->m_WorldState, m_GoalState_Interaction, m_pCharacter_Info->m_vecAction_Act, &m_pCharacter_Info->m_CurAction_Act))
		{
			++m_iAniPlanCount;
			m_pCharacter_Info->m_CurAction_Act.Set_ActEffect(m_pCharacter_Info->m_WorldState);
		}
		// 상호작용이 없다면 회피모드
		else
		{
			// 상태를 바로 바꾸지 않고 받아놓은 후 최종적으로 바꾸기를 한다..
			dwChangeActType = (ACT_SIDESTEP << 1) + true;
			//Set_Bool_BitTable( InputKey_SideStep, true );
		}
		//m_pCharacter_Info->m_WorldState.setVariable(ACT_SIDESTEP, true);
		//회피
		//if (nullptr != CAI_Manager::GetInstance()->plan(m_pCharacter_Info->m_WorldState, m_GoalState_SideStep, m_pCharacter_Info->m_vecAction_Act, &m_pCharacter_Info->m_CurAction_Act))
		//{
		//	m_pCharacter_Info->m_CurAction_Act.Set_ActEffect(m_pCharacter_Info->m_WorldState);
		//}
		//else if (nullptr != CAI_Manager::GetInstance()->plan(m_pCharacter_Info->m_WorldState, m_GoalState_Interaction, m_pCharacter_Info->m_vecAction_Act, &m_pCharacter_Info->m_CurAction_Act))
		//{
		//	m_pCharacter_Info->m_CurAction_Act.Set_ActEffect(m_pCharacter_Info->m_WorldState);
		//}
		Set_Bool_BitTable( InputKey_Action, true );
	}
	// 공격 키 입력
	else if (m_pInput->Get_DownMouseState(Engine::CInput_Device::DIM_LBUTTON))
	{
		////타겟이 딱히 없으면?
		//if ( m_pCharacter_Info->m_WorldState.getVariable( TARGET_NONE ) )
		//{
		//	m_pCharacter_Info->m_WorldState.setVariable( SIGNAL_CHANGE_TARGET_NEAR_ENEMY, true );
		//}

		Set_Bool_BitTable( InputKey_Attack, true );
		Set_Bool_BitTable( InputKey_Action, true );
	}
	// 발차기 키 입력
	else if ( m_pInput->Get_DownMouseState( Engine::CInput_Device::DIM_RBUTTON ) )
	{
		Set_Bool_BitTable( InputKey_Kick, true );
		Set_Bool_BitTable( InputKey_Action, true );
	}
	else if ( m_pInput->Get_DownKeyState( DIK_F ) )
	{
		// 표창을 던지는 코드를 추가할 예정..
	}
	else if ( m_pInput->Get_DownKeyState( DIK_G ) )
	{
		// 아이템을 먹을 코드를 추가할 예정..
	}

	// 움직임 키입력
	int iInputAngle = 0;
	if (m_pInput->Get_DIKeyState(DIK_W))
	{
		if ( !Get_Bool_BitTable( InputKey_Action ) )
		{
			// 상태를 바로 바꾸지 않고 받아놓은 후 최종적으로 바꾸기를 한다..
			dwChangeActType = (ACT_RUN << 1) + true;

			Set_Bool_BitTable( InputKey_Action, true );
		}
		iInputAngle += 1;
	}
	if (m_pInput->Get_DIKeyState(DIK_S))
	{
		if ( !Get_Bool_BitTable( InputKey_Action ) )
		{
			// 상태를 바로 바꾸지 않고 받아놓은 후 최종적으로 바꾸기를 한다..
			dwChangeActType = (ACT_RUN << 1) + true;

			Set_Bool_BitTable( InputKey_Action, true );
		}
		iInputAngle += -1;
	}
	if (m_pInput->Get_DIKeyState(DIK_D))
	{
		if ( !Get_Bool_BitTable( InputKey_Action ) )
		{
			// 상태를 바로 바꾸지 않고 받아놓은 후 최종적으로 바꾸기를 한다..
			dwChangeActType = (ACT_RUN << 1) + true;

			Set_Bool_BitTable( InputKey_Action, true );
		}
		iInputAngle += 3;
	}
	if (m_pInput->Get_DIKeyState(DIK_A))
	{
		if ( !Get_Bool_BitTable( InputKey_Action ) )
		{
			// 상태를 바로 바꾸지 않고 받아놓은 후 최종적으로 바꾸기를 한다..
			dwChangeActType = (ACT_RUN << 1) + true;

			Set_Bool_BitTable( InputKey_Action, true );
		}
		iInputAngle += -3;
	}

	// ----------------- 행동 선택 키입력 끝 -------------------
	// ------ 행동이 선택 된 후, 월드스테이트 변화에 따른 파라미터 변경 시작 ------

	// 타겟 업데이트
	Update_CharacterTarget(pNearEnemy, pNearTeam,m_pCharacter,m_pCharacter_Info->m_pEnemy_Attacked);

	//// 캐릭터 앵글 변경
	//Update_CharacterAngle();

	// ------ 행동이 선택 된 후, 월드스테이트 변화에 따른 파라미터 변경 끝 ------


	// ----------------- 애니메이션 선택 AI 시작 -------------------
	if ( m_pCharacter->Get_Mesh()->is_Finish_Animation() >= 1
		 || m_pCharacter_Info->m_WorldState.getVariable( SIGNAL_CHANGE_ANIMATION_FORCED )
		 )
	{
		m_pCharacter_Info->m_WorldState.setVariable( SIGNAL_CHANGE_ANIMATION_FORCED, false );
		m_pCharacter_Info->m_WorldState.setVariable( SIGNAL_CHANGE_ANIMATION, true );

		if ( m_pCharacter->Get_Mesh()->is_Finish_Animation() >= 1 )	// 애니메이션이 끝났을 때에만 실행할 것들..
		{
			Set_Bool_BitTable( IsGuardState, false );
		}

		// 공격 상태의 관련된 값들 초기화..
		Set_Bool_BitTable( IsAttackState, false );
		Set_Bool_BitTable( IsKickState, false );
		Set_Bool_BitTable( IsSideStepState, false );

		// 만약 공격 키를 눌렀다면 공격을 하게끔..
		if ( Get_Bool_BitTable( InputKey_Attack ) == true )
		{
			Set_Bool_BitTable( InputKey_Attack, false );
			Set_Bool_BitTable( IsAttackState, true );

			// 공격할 방향에 타겟이 있는 지 검사하고 있다면 타겟 데이터 갱신..
			Update_AttackTargetData( &pNearEnemy, iInputAngle, iSceneNumber, pEnemyTargetTag );
		}
		else if ( Get_Bool_BitTable( InputKey_Kick ) == true )
		{
			Set_Bool_BitTable( InputKey_Kick, false );
			Set_Bool_BitTable( IsKickState, true );

			// 공격할 방향에 타겟이 있는 지 검사하고 있다면 타겟 데이터 갱신..
			Update_AttackTargetData( &pNearEnemy, iInputAngle, iSceneNumber, pEnemyTargetTag );
		}
	}

	if ( Get_Bool_BitTable( InputKey_Guard ) == true )
	{
		// 상태를 바로 바꾸지 않고 받아놓은 후 최종적으로 바꾸기를 한다..
		dwChangeActType = (ACT_GUARD << 1) + true;
		Set_Bool_BitTable( InputKey_Guard, false );
	}
	else if ( true == Get_Bool_BitTable( IsGuardState ) )
	{
		if ( false == Get_Bool_BitTable( InputKey_Action ) )
		{
			// 상태를 바로 바꾸지 않고 받아놓은 후 최종적으로 바꾸기를 한다..
			dwChangeActType = (ACT_GUARD << 1) + true;
		}
		else
			Set_Bool_BitTable( IsGuardState, false );
	}
	else if ( Get_Bool_BitTable( IsSideStepState ) == true )	// 만약 회피(구르기) 중인 경우..
	{
		// 상태를 바로 바꾸지 않고 받아놓은 후 최종적으로 바꾸기를 한다..
		dwChangeActType = (ACT_SIDESTEP << 1) + true;
	}
	else if ( Get_Bool_BitTable( IsAttackState ) == true ) // 만약 공격 중인 경우..
	{
		// 상태를 바로 바꾸지 않고 받아놓은 후 최종적으로 바꾸기를 한다..
		dwChangeActType = ( ACT_ATTACK << 1 ) + true;
	}
	else if ( Get_Bool_BitTable( IsKickState ) == true ) // 만약 발차기 공격 중인 경우..
	{
		// 상태를 바로 바꾸지 않고 받아놓은 후 최종적으로 바꾸기를 한다..
		dwChangeActType = ( ACT_KICK << 1 ) + true;
	}

	// 모든 케이스가 아닐 경우 WAIT 액션 상태로..
	if ( !Get_Bool_BitTable( InputKey_Action ) )
	{
		if ( 0 == dwChangeActType )
		{
			m_pCharacter_Info->m_WorldState.setVariable( SIGNAL_CHANGE_TARGET_NONE, true );

			// 상태를 바로 바꾸지 않고 받아놓은 후 최종적으로 바꾸기를 한다..
			dwChangeActType = ( ACT_IDLE << 1 ) + true;
		}
	}
	else
	{
		if ( false == m_pCharacter_Info->m_WorldState.getVariable( CH_STATE_ATTACK ) &&
			 false == Get_Bool_BitTable( IsSideStepState ) )
		{
			m_pCharacter_Info->m_fAngleSpeed = 5.f;

			//키 입력에 따른 앵글 계산
			//m_fDestinationAngle = Compute_CamAngleToCharAngle( 360.f + Compute_DirToInputKey( iInputAngle ) );
			m_fDestinationAngle = Compute_DirToInputKey( iInputAngle );

			Set_Angle( m_pCharacter->Get_Camera()->Get_WorldMatrix(), m_fDestinationAngle );

			Compute_DirWorldState();
		}
		else
		{
			m_pCharacter_Info->m_fAngleSpeed = 5.f;
		}
	}

	if ( dwChangeActType != 0 )	// 상태를 바꿔야 할 때..
	{
		_bool bIsVariable = dwChangeActType & 1;
		dwChangeActType = dwChangeActType >> 1;

		switch ( dwChangeActType )
		{
			case ACT_IDLE:
				break;
			case ACT_RUN:
				break;
			case ACT_ATTACK:
				break;
			case ACT_KICK:
				break;
			case ACT_SIDESTEP:
				m_fDestinationAngle = Compute_DirToInputKey( iInputAngle );

				Set_Angle( m_pCharacter->Get_Camera()->Get_WorldMatrix(), m_fDestinationAngle );

				Compute_DirWorldState();

				Set_Bool_BitTable( IsSideStepState, true );
				break;
		}

		m_pCharacter_Info->m_WorldState.setVariable( dwChangeActType, bIsVariable );
	}

	// 이동키를 입력한 경우 자유로운 이동이 가능해야함
	if ( m_pCharacter_Info->m_WorldState.getVariable( ACT_RUN ) )
		m_pCharacter_Info->m_WorldState.setVariable( SIGNAL_CHANGE_TARGET_NONE, true );

	// 애니메이션 변경되어야 하면?
	if (m_pCharacter_Info->m_WorldState.getVariable(SIGNAL_CHANGE_ANIMATION) == true)
	{
		// ----------------- 애니메이션 따라 변화가 있는 캐릭터 스테이트 초기화 -------------------
		m_pCharacter_Info->m_WorldState.setVariable( CH_STATE_TYPE_BY_ANI_START, CH_STATE_TYPE_BY_ANI_END, false );

#ifndef _FINAL
		float fTime = 0.f;
		bool bErr = false;
		CHECK_TIME_START
#endif
		// 애니메이션 선택
		if (nullptr != CAI_Manager::GetInstance()->plan(m_pCharacter_Info->m_WorldState, m_GoalState_Animation, m_pCharacter_Info->m_vecAction_Ani, &m_pCharacter_Info->m_CurAction_Ani))
			{
				++m_iAniPlanCount;
				m_pCharacter_Info->m_WorldState.setVariable(m_pCharacter_Info->m_StartAniNum, m_pCharacter_Info->m_EndAniNum, false);
				m_pCharacter_Info->m_CurAction_Ani.Set_ActEffect(m_pCharacter_Info->m_WorldState);
			}
#ifndef _FINAL
		CHECK_TIME_END(fPlanTime, bErr);
#endif

		//애니메이션 세팅
		m_iCurAniNum = m_pCharacter_Info->m_CurAction_Ani.Get_Animation(m_pCharacter_Info->m_StartAniNum, m_pCharacter_Info->m_EndAniNum) - (m_pCharacter_Info->m_StartAniNum + 1);
		m_pCharacter->Get_Mesh()->Set_AnimationSet(m_iCurAniNum);

		//현재 애니메이션 정보 얻어오기
		m_iCurAniEventSize = m_pCharacter->Get_Mesh()->Get_CurAnimationInfo().iAni_Index;
		m_iCurAniEventNum = 0;
		m_curAniEvent = m_pCharacter->Get_Mesh()->Get_CurAnimationInfo().m_AniEvent;

		m_pCharacter_Info->m_WorldState.setVariable(STATE_TYPE::SIGNAL_CHANGE_ANIMATION, false);

	}
	// ----------------- 애니메이션 선택 AI 끝 -------------------


	// --------- 행동/애니메이션으로 변화된 월드 스테이스에 따라 데이터 처리 시작 ----------

	// 타겟 업데이트
	Update_CharacterTarget(pNearEnemy, pNearTeam, m_pCharacter, m_pCharacter_Info->m_pEnemy_Attacked);

	// 캐릭터 앵글 변경
	Update_CharacterAngle();

	// 캐릭터와 타겟 간의 거리 갱신..
	Update_TargetDistance( fTimeDelta );


	// 달리기 상태 처리
	if (m_pCharacter_Info->m_WorldState.getVariable(CH_STATE_RUN))
	{
		if (m_pCharacter_Info->m_WorldState.getVariable(CH_STATE_SPRINT))
			m_pCharacter->Move_Forward(2.0f);
		else
			m_pCharacter->Move_Forward(1.f);
	}

	// ---- 애니메이션 이벤트 처리 시작----
	if ( m_curAniEvent != nullptr )
	{
		if ( m_iCurAniEventNum < m_iCurAniEventSize )
		{
			while ( m_pCharacter->Get_Mesh()->Get_TrackInfo()->Position >= m_curAniEvent[m_iCurAniEventNum].fAniPosition )
			{
				switch ( m_curAniEvent[m_iCurAniEventNum].pEvnetArr )
				{
					case COLLIDER_ON:
						m_pCharacter->Act_AnimationEvent( m_pCharacter->Get_Mesh()->Get_CurAnimationInfo(), m_iCurAniEventNum );
						break;
				}
				++m_iCurAniEventNum;
			}
		}
	}
	// ---- 애니메이션 이벤트 처리 끝----

	// --------- 행동/애니메이션으로 변화된 월드 스테이스에 따라 데이터 처리 끝 ----------

	// UI로 정보 넘긴다.
	CSubject_Manager::GetInstance()->Notify(MESSAGE_CURRENT_PLAYER_DATA, CSubject_Manager::SUBJECTTYPE::TYPE_STATIC, m_pCharacter);
	CSubject_Manager::GetInstance()->Notify(MESSAGE_CAMERA_DATA, CSubject_Manager::SUBJECTTYPE::TYPE_STATIC, m_pCharacter->Get_Camera());
	CSubject_Manager::GetInstance()->Notify(MESSAGE_STATE_DATA, CSubject_Manager::SUBJECTTYPE::TYPE_STATIC, &m_pCharacter_Info->m_WorldState);


	if (m_pInput->Get_DIKeyState(DIK_LCONTROL))
	{
		if (m_pInput->Get_DownMouseState(CInput_Device::DIM_LBUTTON))
		{
			CEffect_Manager::GetInstance()->Use_Effect(SCENE_STAGE, L"Effect_Test_9999",m_pCharacter,m_pCharacter->Get_Mesh()->Find_FrameMatrixPointer("Origin"));
		}
		if (m_pInput->Get_DownKeyState(DIK_1))
		{
			CEffect_Manager::GetInstance()->Use_Effect(SCENE_STAGE, L"Effect_Test", *m_pCharacter->Get_Transform()->Get_Infomation(CTransform::INFO_POSITION));
		}
		if (m_pInput->Get_DownKeyState(DIK_2))
		{
			CEffect_Manager::GetInstance()->Use_Effect(SCENE_STAGE, L"Effect_Test2", *m_pCharacter->Get_Transform()->Get_Infomation(CTransform::INFO_POSITION));
		}
		if (m_pInput->Get_DownKeyState(DIK_3))
		{
			CEffect_Manager::GetInstance()->Use_Effect(SCENE_STAGE, L"Effect_Test3", *m_pCharacter->Get_Transform()->Get_Infomation(CTransform::INFO_POSITION));
		}
		if (m_pInput->Get_DownKeyState(DIK_4))
		{
			CEffect_Manager::GetInstance()->Use_Effect(SCENE_STAGE, L"Effect_Test_Mask", *m_pCharacter->Get_Transform()->Get_Infomation(CTransform::INFO_POSITION));
		}
		
	}
}

void CPlayerController::LastUpdate_Component(const _float & fTimeDelta)
{

	// 플레이 캐릭터 선택
	if (Engine::CInput_Device::GetInstance()->Get_DownKeyState(DIK_UP))
	{
		CGameObject* pPlayer = nullptr;
		pPlayer = CObject_Manager::GetInstance()->Get_FindObject(SCENE_STATIC, L"Player", 0);
		if (pPlayer != nullptr)
		{
			// 살아있는 플레이어
			if (pPlayer->Get_IsDead() == false)
			{
				Possess(dynamic_cast<CCharacter*>(pPlayer));
				return;
			}
		}
	}
	else if (Engine::CInput_Device::GetInstance()->Get_DownKeyState(DIK_LEFT))
	{
		CGameObject* pPlayer = nullptr;
		pPlayer = CObject_Manager::GetInstance()->Get_FindObject(SCENE_STATIC, L"Player", 1);
		if (pPlayer != nullptr)
		{
			// 살아있는 플레이어
			if (pPlayer->Get_IsDead() == false)
			{
				Possess(dynamic_cast<CCharacter*>(pPlayer));
				return;
			}
		}
	}
	else if (Engine::CInput_Device::GetInstance()->Get_DownKeyState(DIK_DOWN))
	{
		CGameObject* pPlayer = nullptr;
		pPlayer = CObject_Manager::GetInstance()->Get_FindObject(SCENE_STATIC, L"Player", 2);
		if (pPlayer != nullptr)
		{
			// 살아있는 플레이어
			if (pPlayer->Get_IsDead() == false)
			{
				Possess(dynamic_cast<CCharacter*>(pPlayer));
				return;
			}
		}
	}
	else if (Engine::CInput_Device::GetInstance()->Get_DownKeyState(DIK_RIGHT))
	{
		CGameObject* pPlayer = nullptr;
		pPlayer = CObject_Manager::GetInstance()->Get_FindObject(SCENE_STATIC, L"Player", 3);
		if (pPlayer != nullptr)
		{
			// 살아있는 플레이어
			if (pPlayer->Get_IsDead() == false)
			{
				Possess(dynamic_cast<CCharacter*>(pPlayer));
				return;
			}
		}
	}
}


CPlayerController * CPlayerController::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CPlayerController *		pInstance = new CPlayerController(pGraphicDev);

	if (FAILED(pInstance->Ready_Controller()))
	{
		MSG_BOX("CPlayerController Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CPlayerController::Clone(void)
{
	this->AddRef();
	return this;
}

//CComponent * CPlayerController::Clone(void)
//{
//	//this->AddRef();
//	//return this;
//	return new CPlayerController(*this);
//}

_ulong CPlayerController::Free(void)
{
	Safe_Release(m_pTargetExController);
	Safe_Release(m_pInput);
	return CController::Free();
}

void CPlayerController::Possess(CCharacter * pTarget)
{
	if (nullptr == pTarget)
		return;

	// 이전 타겟에게 이전 컨트롤러 반환
	if (nullptr != m_pCharacter)
		UnPossess();

	// 새로 설정할 타겟의 컨트롤러를 보관
	m_pTargetExController = pTarget->Get_Controller();


	//타겟 재설정
	m_pCharacter = pTarget;
	Get_TargetInfo(m_pCharacter);

	// 타겟이 가지고 있는 컨트롤러가 this가 아니라면 세팅
	//if (m_pTarget->Get_Controller() != this)
	m_pCharacter->Set_Controller(this);

	m_iTargetAniSetCount = m_pCharacter->Get_Mesh()->Get_MaxAniSet();

}

bool CPlayerController::UnPossess()
{
	if (m_pCharacter != nullptr)
	{	// 이전 타겟에게 이전 컨트롤러를 세팅
		m_pTargetExController->Possess(m_pCharacter);
		//m_pTarget->Set_Controller(m_pTargetExController);
	}
	// 타겟 초기화
	m_pCharacter = nullptr;
	m_pCharacter_Info = nullptr;

	return true;
}

void CPlayerController::Set_Angle_ForPlayer(const _matrix & matTargetWorld, float offsetAngle)
{
	_vec3 vTargetLook = *(_vec3*)matTargetWorld.m[2];
	vTargetLook.y = 0.f;

	D3DXVec3Normalize(&vTargetLook, &vTargetLook);


	m_pCharacter_Info->m_fDestAngle = D3DXToDegree(acosf(D3DXVec3Dot(&vTargetLook, &_vec3(0.f, 0.f, 1.f))));
	if (vTargetLook.x <= 0.f)
	{
		m_pCharacter_Info->m_fDestAngle = 360.f - m_pCharacter_Info->m_fDestAngle;
	}
	m_pCharacter_Info->m_fDestAngle += offsetAngle;
	//m_pTargetInfo->m_fDestAngle = offsetAngle;

	if (m_pCharacter_Info->m_fDestAngle > 360.f)
	{
		m_pCharacter_Info->m_fDestAngle -= 360.f;
	}
	if (m_pCharacter_Info->m_fDestAngle < 0.f)
	{
		m_pCharacter_Info->m_fDestAngle += 360.f;
	}

	_float fAngleGap = m_pCharacter_Info->m_fDestAngle - m_pCharacter->Get_Transform()->Get_Infomation(CTransform::INFO_ANGLE)->y;


	if (fAngleGap <= 0.f)
	{
		fAngleGap += 360.f;
	}

	if (fAngleGap >= 180.f)
	{
		if ((360.f - fAngleGap) * m_pTimer->Get_TimeDelta() * m_pCharacter_Info->m_fAngleSpeed <=  SMALL_NUMBER )//* fabs(m_pTargetInfo->m_fAngleGap_CameraToPlayer))
		{
			m_pCharacter_Info->m_WorldState.setVariable(SIGNAL_CHANGE_ANGLE, false);
			return;
		}
		m_pCharacter->Get_Transform()->Add_Infomation(CTransform::INFO_ANGLE,
													   &_vec3(0.f,
															   -(360.f - fAngleGap) * m_pTimer->Get_TimeDelta() * m_pCharacter_Info->m_fAngleSpeed,
															   0.f));
	}
	else
	{
		if ((fAngleGap)* m_pTimer->Get_TimeDelta() * m_pCharacter_Info->m_fAngleSpeed <= SMALL_NUMBER)//* fabs(m_pTargetInfo->m_fAngleGap_CameraToPlayer))
		{
			m_pCharacter_Info->m_WorldState.setVariable(SIGNAL_CHANGE_ANGLE, false);
			return;
		}
		m_pCharacter->Get_Transform()->Add_Infomation(CTransform::INFO_ANGLE,
													   &_vec3(0.f,
													   (fAngleGap)* m_pTimer->Get_TimeDelta() * m_pCharacter_Info->m_fAngleSpeed,
															   0.f));
	}

	m_pCharacter_Info->m_WorldState.setVariable(SIGNAL_CHANGE_ANGLE, true);
}

//
//EStatus CPlayerController::Attack_Forward_Dash()
//{
//	if (nullptr == m_pTarget)
//		return EStatus::Failure;
//
//	m_pTarget->Get_Transform();
//
//	return EStatus::Success;
//}
//
//EStatus CPlayerController::Move_Forward()
//{
//	CTransform* pTargetTrnasform = m_pTarget->Get_Transform();
//
//	//현재 카메라가 플레이어 카메라면
//	if (CCamera_Manager::GetInstance()->Get_CurrentCamera() == m_pTarget->Get_Camera())
//	{
//		// 카메라
//		pTargetTrnasform->Go_Direction(
//			CCamera_Manager::GetInstance()->Get_Direction_NotY(),
//			m_pTarget->Get_Info().m_fSpeed,
//			m_pInput->Get_TimeDelta());
//		return EStatus::Success;
//
//	}
//	return EStatus::Failure;
//}
//
//EStatus CPlayerController::Move_Right()
//{
//	
//	return EStatus::Success;
//}

_bool CPlayerController::IsCanChangeAttackTarget( const _uint& _iInputKey )
{
	if ( false == m_pCharacter_Info->m_WorldState.getVariable( TARGET_NONE ) )
	{
		if ( m_iPrevAttInputKey == _iInputKey )	// 이전 공격 했을 때와 같은 공격 키를 눌렀다면..
			return false;

		return true;
	}
	else
		return true;

	return false;
}

void CPlayerController::Update_CharacterAngle( void )
{
	//캐릭터 앵글 변경 시그널
	if (m_pCharacter_Info->m_WorldState.getVariable(SIGNAL_CHANGE_ANGLE)) 
	{
		// 타겟과의 앵글값 계산
		if (m_pCharacter_Info->m_WorldState.getVariable(SIGNAL_SET_ANGLE_TARGET)
			 && m_pCharacter_Info->m_WorldState.getVariable(CH_STATE_TOOFAR_TARGET)
			 )
		{
			if (m_pTarget != nullptr)
			{
				m_fDestinationAngle = Get_Angle( *m_pTarget->Get_Transform()->Get_Infomation( Engine::CTransform::INFO_POSITION ) );

				Set_Angle( m_pCharacter->Get_Transform()->Get_WorldMatrix(), m_fDestinationAngle );
				Compute_DirWorldState();	// 플레이어 방향 앵글값 월드스테이트에 입력

				//CDebug::GetInstance()->Add_ConsoleDebugText("m_pCharacter_Info->m_fDestAngle :%f", m_pCharacter_Info->m_fDestAngle);
			}
			else
			{
				//if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_0))
				//	Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 0.f);
				//else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_45))
				//	Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 45.f);
				//else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_90))
				//	Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 90.f);
				//else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_135))
				//	Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 135.f);
				//else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_180))
				//	Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 180.f);
				//else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_225))
				//	Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 225.f);
				//else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_270))
				//	Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 270.f);
				//else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_315))
				//	Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 315.f);

				//Set_Angle( m_pCharacter->Get_Transform()->Get_WorldMatrix(), m_fDestinationAngle );
				//Compute_DirWorldState();	// 플레이어 방향 앵글값 월드스테이트에 입력
			}
		}
		else
		{
			//if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_0))
			//	Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 0.f);
			//else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_45))
			//	Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 45.f);
			//else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_90))
			//	Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 90.f);
			//else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_135))
			//	Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 135.f);
			//else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_180))
			//	Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 180.f);
			//else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_225))
			//	Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 225.f);
			//else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_270))
			//	Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 270.f);
			//else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_315))
			//	Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 315.f);

			//Set_Angle( m_pCharacter->Get_Transform()->Get_WorldMatrix(), m_fDestinationAngle );
			//Compute_DirWorldState();	// 플레이어 방향 앵글값 월드스테이트에 입력
		}
	}
	else if (m_pCharacter_Info->m_WorldState.getVariable(SIGNAL_CHANGE_ANGLE_FORCED))
	{// 타겟과의 앵글값 계산
		if (m_pCharacter_Info->m_WorldState.getVariable(SIGNAL_SET_ANGLE_TARGET)
			 && m_pCharacter_Info->m_WorldState.getVariable(CH_STATE_TOOFAR_TARGET)
			 )
		{	
			if (m_pTarget != nullptr)
			{
				m_fDestinationAngle = Get_Angle( *m_pTarget->Get_Transform()->Get_Infomation( Engine::CTransform::INFO_POSITION ) );

				Set_Angle( m_pCharacter->Get_Transform()->Get_WorldMatrix(), m_fDestinationAngle );
				Compute_DirWorldState();	// 플레이어 방향 앵글값 월드스테이트에 입력

				Set_Angle_FORCED( m_pCharacter->Get_Transform()->Get_WorldMatrix(), m_fDestinationAngle );
			}
			else
			{
				//if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_0))
				//	Set_Angle_FORCED(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 0.f);
				//else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_45))
				//	Set_Angle_FORCED(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 45.f);
				//else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_90))
				//	Set_Angle_FORCED(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 90.f);
				//else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_135))
				//	Set_Angle_FORCED(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 135.f);
				//else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_180))
				//	Set_Angle_FORCED(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 180.f);
				//else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_225))
				//	Set_Angle_FORCED(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 225.f);
				//else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_270))
				//	Set_Angle_FORCED(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 270.f);
				//else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_315))
				//	Set_Angle_FORCED(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 315.f);
			}

		}
		else
		{
			//if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_0))
			//	Set_Angle_FORCED(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 0.f);
			//else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_45))
			//	Set_Angle_FORCED(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 45.f);
			//else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_90))
			//	Set_Angle_FORCED(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 90.f);
			//else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_135))
			//	Set_Angle_FORCED(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 135.f);
			//else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_180))
			//	Set_Angle_FORCED(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 180.f);
			//else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_225))
			//	Set_Angle_FORCED(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 225.f);
			//else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_270))
			//	Set_Angle_FORCED(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 270.f);
			//else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_315))
			//	Set_Angle_FORCED(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 315.f);
		}
	}
	else if (m_pCharacter_Info->m_WorldState.getVariable(SIGNAL_CHANGE_ANGLE_ONCE))
	{
		// 타겟과의 앵글값 계산
		if (m_pCharacter_Info->m_WorldState.getVariable(SIGNAL_SET_ANGLE_TARGET)
			 && m_pCharacter_Info->m_WorldState.getVariable(CH_STATE_TOOFAR_TARGET)
			 )
		{
			if (m_pTarget != nullptr)
			{
				m_fDestinationAngle = Get_Angle(*m_pTarget->Get_Transform()->Get_Infomation(CTransform::INFO_POSITION));

				// 플레이어 방향 앵글값 월드스테이트에 입력
				Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), m_fDestinationAngle);
				Compute_DirWorldState();

				//CDebug::GetInstance()->Add_ConsoleDebugText("m_pCharacter_Info->m_fDestAngle :%f", m_pCharacter_Info->m_fDestAngle);
			}
			else
			{
				//if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_0))
				//	Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 0.f);
				//else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_45))
				//	Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 45.f);
				//else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_90))
				//	Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 90.f);
				//else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_135))
				//	Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 135.f);
				//else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_180))
				//	Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 180.f);
				//else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_225))
				//	Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 225.f);
				//else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_270))
				//	Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 270.f);
				//else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_315))
				//	Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 315.f);
			}
		}
		else
		{
			//if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_0))
			//	Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 0.f);
			//else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_45))
			//	Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 45.f);
			//else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_90))
			//	Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 90.f);
			//else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_135))
			//	Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 135.f);
			//else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_180))
			//	Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 180.f);
			//else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_225))
			//	Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 225.f);
			//else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_270))
			//	Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 270.f);
			//else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_315))
			//	Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 315.f);
		}
		m_pCharacter_Info->m_WorldState.setVariable(SIGNAL_CHANGE_ANGLE_ONCE, false);
	}
	else
		return;

	//_float fAngleGap = m_pCharacter_Info->m_fDestAngle - m_pCharacter->Get_Transform()->Get_Infomation(CTransform::INFO_ANGLE)->y;
	_float fAngleGap = m_pCharacter_Info->m_fDestAngle - m_pCharacter->Get_Transform()->Get_Infomation(CTransform::INFO_ANGLE)->y;

	if (fAngleGap <= 0.f)
		fAngleGap += 360.f;

	if (fAngleGap >= 180.f)
	{
		//if ((360.f - fAngleGap) * m_pTimer->Get_TimeDelta() * m_pCharacter_Info->m_fAngleSpeed <= SMALL_NUMBER)//* fabs(m_pTargetInfo->m_fAngleGap_CameraToPlayer))
		if (fabsf(360.f - fAngleGap) <= m_pTimer->Get_TimeDelta() * m_pCharacter_Info->m_fAngleSpeed) // Add By.KMH - 앵글 체인지 끝내는 조건을 다르게(Test)..
		{
			m_pCharacter_Info->m_WorldState.setVariable(SIGNAL_CHANGE_ANGLE, false);
			m_pCharacter_Info->m_WorldState.setVariable(SIGNAL_CHANGE_ANGLE_ONCE, false);
			m_pCharacter_Info->m_WorldState.setVariable(SIGNAL_CHANGE_ANGLE_FORCED, false);
			return;
		}
		m_pCharacter->Get_Transform()->Add_Infomation(CTransform::INFO_ANGLE,
													   &_vec3(0.f,
															   -(360.f - fAngleGap) * m_pTimer->Get_TimeDelta() * m_pCharacter_Info->m_fAngleSpeed,
															   0.f));
	}
	else
	{
		//if ((fAngleGap)* m_pTimer->Get_TimeDelta() * m_pCharacter_Info->m_fAngleSpeed <= SMALL_NUMBER)//* fabs(m_pTargetInfo->m_fAngleGap_CameraToPlayer))
		if ((fAngleGap) <= m_pTimer->Get_TimeDelta() * m_pCharacter_Info->m_fAngleSpeed) // Add By.KMH - 앵글 체인지 끝내는 조건을 다르게(Test)..
		{
			m_pCharacter_Info->m_WorldState.setVariable(SIGNAL_CHANGE_ANGLE, false);
			m_pCharacter_Info->m_WorldState.setVariable(SIGNAL_CHANGE_ANGLE_ONCE, false);
			m_pCharacter_Info->m_WorldState.setVariable(SIGNAL_CHANGE_ANGLE_FORCED, false);
			return;
		}
		m_pCharacter->Get_Transform()->Add_Infomation(CTransform::INFO_ANGLE,
													   &_vec3(0.f,
													   (fAngleGap)* m_pTimer->Get_TimeDelta() * m_pCharacter_Info->m_fAngleSpeed,
															   0.f));
	}

	Compute_DirWorldState();
	//m_pCharacter_Info->m_WorldState.setVariable(SIGNAL_CHANGE_ANGLE, true);
}

void CPlayerController::Update_TargetDistance( const _float& _fTimeDelta )
{
	// 타겟이 있는지 없는지 부터 체크..
	if ( true == m_pCharacter_Info->m_WorldState.getVariable( TARGET_NONE ) )
		return;

	if ( true == Get_Bool_BitTable( IsApproachTarget ) )
	{
		CTransform* pTransform = m_pCharacter->Get_Transform();

		_vec3 vTargetPos = *m_pTarget->Get_Transform()->Get_Infomation( Engine::CTransform::INFO_POSITION );
		_vec3 vPos = *pTransform->Get_Infomation( Engine::CTransform::INFO_POSITION );

		_float fMoveTimeDeltaSpeed = _fTimeDelta * m_fMoveTimeSpeed;

		if ( fMoveTimeDeltaSpeed > 1.f )
			fMoveTimeDeltaSpeed = 0.f;

		D3DXVec3Lerp( &vPos, &vPos, &vTargetPos, fMoveTimeDeltaSpeed );
		m_fAccMoveTime += fMoveTimeDeltaSpeed;

		if ( D3DXVec3Length(&(vPos - vTargetPos)) <= m_pCharacter_Info->m_fRange_Close )
		{
			Set_Bool_BitTable( IsApproachTarget, false );
			m_fAccMoveTime = 0.f;
		}

		pTransform->Set_Infomation( Engine::CTransform::INFO_POSITION, &vPos );
	}
}

void CPlayerController::Set_Bool_BitTable( const BOOLTABLETYPE& _InputKeyType, const _bool & _bIsInput )
{
	if ( _InputKeyType >= InputKey_End )
		return;

	if ( _bIsInput == true )
		m_dwBool_BitTable |= (1 << _InputKeyType);
	else
		m_dwBool_BitTable &= ~(1 << _InputKeyType);
}

_bool CPlayerController::Get_Bool_BitTable( const BOOLTABLETYPE& _InputKeyType )
{
	if ( _InputKeyType >= InputKey_End )
		return false;

	return (m_dwBool_BitTable & (1 << _InputKeyType)) != 0;
}

_float CPlayerController::Compute_DirToInputKey( const _uint & _iInputKey )
{
	switch (_iInputKey)
	{
		case 1: // 북
			return 0.f;
		case 4: // 북동
			return -315.f;
		case 3: // 동
			return -270.f;
		case 2: // 동남
			return -225.f;		
		case -1: // 남
			return -180.f;
		case -4: // 남서
			return -135.f;
		case -3: // 서
			return -90.f;
		case -2: // 서북
			return -45.f;
	}

	return 0.f;
}

_float CPlayerController::Compute_CamAngleToCharAngle( _float _fAngle )
{
	_fAngle = Get_Angle( m_pCharacter->Get_Camera()->Get_WorldMatrix(), _fAngle );

	return _fAngle;
}

void CPlayerController::Compute_DirWorldState( void )
{
	m_pCharacter_Info->m_WorldState.setVariable(DEST_DIR_0, DEST_DIR_315, false);

	_float fAngleGap = m_pCharacter_Info->m_fDestAngle - m_pCharacter->Get_Transform()->Get_Infomation( CTransform::INFO_ANGLE )->y;
	if ( fAngleGap < 0.f )
		fAngleGap = 360.f + fAngleGap;
	else if ( fAngleGap > 360.f )
		fAngleGap -= 360.f;

	// 플레이어 방향 앵글값 월드스테이트에 입력
	if (fAngleGap < 22.5f)
	{
		m_pCharacter_Info->m_WorldState.setVariable(DEST_DIR_0, true);
	}
	else if (fAngleGap < 67.5f)
	{
		m_pCharacter_Info->m_WorldState.setVariable(DEST_DIR_45, true);
	}
	else if (fAngleGap < 112.5f)
	{
		m_pCharacter_Info->m_WorldState.setVariable(DEST_DIR_90, true);
	}
	else if (fAngleGap < 157.5f)
	{
		m_pCharacter_Info->m_WorldState.setVariable(DEST_DIR_135, true);
	}
	else if (fAngleGap < 202.5)
	{
		m_pCharacter_Info->m_WorldState.setVariable(DEST_DIR_180, true);
	}
	else if (fAngleGap < 247.5)
	{
		m_pCharacter_Info->m_WorldState.setVariable(DEST_DIR_225, true);
	}
	else if (fAngleGap < 292.5)
	{
		m_pCharacter_Info->m_WorldState.setVariable(DEST_DIR_270, true);
	}
	else if (fAngleGap < 337.5)
	{
		m_pCharacter_Info->m_WorldState.setVariable(DEST_DIR_315, true);
	}
	else if (fAngleGap <= 360.f)
	{
		m_pCharacter_Info->m_WorldState.setVariable(DEST_DIR_0, true);
	}
	//_float fAngle = m_pCharacter_Info->m_fDestAngle + 22.5f;
	//if ( fAngle >= 359.999999f )
	//	fAngle = fabsf( fAngle - 360.f );
	//_uint iCount = (_uint)(fAngle / 45.f);
	//
	//m_pCharacter_Info->m_WorldState.setVariable(DEST_DIR_0, DEST_DIR_315, false);
	//m_pCharacter_Info->m_WorldState.setVariable(DEST_DIR_0 + iCount, true);
}

void CPlayerController::Update_AttackTargetData( CCharacter** _pOut, const _uint& _iInputKey, const _int& _iSceneNumber, const _tchar* _pEnemyTargetTag )
{
	//키 입력에 따른 앵글 계산
	_float fAngle = Compute_CamAngleToCharAngle( 360.f + Compute_DirToInputKey( _iInputKey ) ); // 0 ~ 360 사이의 각도로 만들자..

	// ---- 가까운 적 위치 갱신 시작 ----
	CCharacter* pNearEnemy = dynamic_cast<CCharacter*>(CObject_Manager::GetInstance()->Get_FindObject_Near_Position(
		_iSceneNumber, _pEnemyTargetTag, m_pCharacter, fAngle, 45.f, 10.f ));
	
	if ( pNearEnemy != nullptr )
	{
		if ( IsCanChangeAttackTarget( _iInputKey ) )
		{
			m_iPrevAttInputKey = _iInputKey;

			Modify_NearEnemyRangeData( pNearEnemy );

			_float fDistance = m_pCharacter->Get_Transform()->Compute_Distance( &pNearEnemy->Get_Transform()->Get_WorldMatrix() );

			if ( m_pCharacter_Info->m_fRange_Near < fDistance )
				Set_Bool_BitTable( IsApproachTarget, true );
			else
				Set_Bool_BitTable( IsApproachTarget, false );

			m_pCharacter_Info->m_WorldState.setVariable( SIGNAL_CHANGE_TARGET_NEAR_ENEMY, true );
			m_pCharacter_Info->m_WorldState.setVariable( SIGNAL_CHANGE_ANGLE, true );

#ifndef _FINAL
			CDebug::GetInstance()->Add_ConsoleDebugText( L"Find Enemy" );
#endif
		}
	}
	else
	{
#ifndef _FINAL
		CDebug::GetInstance()->Add_ConsoleDebugText( L"Not Find Enemy" );
#endif

		m_pCharacter_Info->m_WorldState.setVariable( SIGNAL_CHANGE_TARGET_NONE, true );
		m_pCharacter_Info->m_WorldState.setVariable( SIGNAL_CHANGE_ANGLE, true );

		_float fOffsetAngle = Compute_DirToInputKey( _iInputKey );

		Set_Angle( m_pCharacter->Get_Camera()->Get_WorldMatrix(), fOffsetAngle );

		Compute_DirWorldState();
	}

	*_pOut = pNearEnemy;
}

void CPlayerController::Modify_NearEnemyRangeData( CCharacter * _pNearEnemy )
{
	_float fNearEnemyToMeDist = D3DXVec3Length(&(*_pNearEnemy->Get_Transform()->Get_Infomation(CTransform::INFO_POSITION)
										   - *m_pCharacter->Get_Transform()->Get_Infomation(CTransform::INFO_POSITION)));

	// 계산된 거리를 월드 스테이트에 반영
	if (fNearEnemyToMeDist < m_pCharacter_Info->m_fRange_Close)
		m_pCharacter_Info->m_WorldState.setVariable(CH_STATE_CLOSE_NEAR_ENEMY, CH_STATE_TOOFAR_NEAR_ENEMY, true);
	else if (fNearEnemyToMeDist < m_pCharacter_Info->m_fRange_Near)
		m_pCharacter_Info->m_WorldState.setVariable(CH_STATE_NEAR_NEAR_ENEMY, CH_STATE_TOOFAR_NEAR_ENEMY, true);
	else if (fNearEnemyToMeDist < m_pCharacter_Info->m_fRange_Mid)
		m_pCharacter_Info->m_WorldState.setVariable(CH_STATE_MID_NEAR_ENEMY, CH_STATE_TOOFAR_NEAR_ENEMY, true);
	else if (fNearEnemyToMeDist < m_pCharacter_Info->m_fRange_Far)
		m_pCharacter_Info->m_WorldState.setVariable(CH_STATE_FAR_NEAR_ENEMY, CH_STATE_TOOFAR_NEAR_ENEMY, true);
	else if (fNearEnemyToMeDist < m_pCharacter_Info->m_fRange_TooFar)
		m_pCharacter_Info->m_WorldState.setVariable(CH_STATE_TOOFAR_NEAR_ENEMY, true);

	if (fNearEnemyToMeDist < m_pCharacter_Info->m_fRange_FoundEnemy)
		m_pCharacter_Info->m_WorldState.setVariable(CH_STATE_FOUND_ENEMY, true);
	//else
	//	m_pCharacter_Info->m_WorldState.setVariable(CH_STATE_FOUND_ENEMY, false);
}

#ifndef _FINAL
void CPlayerController::Debug_Render()
{
	CDebug::GetInstance()->Add_DebugText("CPlayerController");
	CDebug::GetInstance()->Add_DebugText("Target Key : %d", m_pCharacter->GetKey());
	CDebug::GetInstance()->Add_DebugText("Plan() RunTime : %f (ms)",fPlanTime);
	CDebug::GetInstance()->Add_DebugText(0, m_pCharacter_Info->m_WorldState.getVariable(TARGET_PLAYER), "TARGET_PLAYER");
	CDebug::GetInstance()->Add_DebugText(0, m_pCharacter_Info->m_WorldState.getVariable(TARGET_NEAR_ENEMY), "TARGET_NEAR_ENEMY");
	CDebug::GetInstance()->Add_DebugText(0, (bool)m_pTarget,"m_pTarget");
	CDebug::GetInstance()->Add_DebugText(m_fDestinationAngle,"m_fDestinationAngle" );
	CDebug::GetInstance()->Add_DebugText("ActPlan(%d): %s", m_iActPlanCount,m_pCharacter_Info->m_CurAction_Act.name().c_str());
	CDebug::GetInstance()->Add_DebugText("AniPlan(%d): %s", m_iAniPlanCount,m_pCharacter_Info->m_CurAction_Ani.name().c_str());
	CDebug::GetInstance()->Add_DebugText( 0, Get_Bool_BitTable( IsSideStepState ), "IsSideStepState" );
	CDebug::GetInstance()->Add_DebugText( 0, Get_Bool_BitTable( IsGuardState ), "IsGuardState" );
	CDebug::GetInstance()->Add_DebugText( 0, Get_Bool_BitTable( InputKey_Guard ), "InputKey_Guard" );
	CDebug::GetInstance()->Add_DebugText( 0, Get_Bool_BitTable( InputKey_Action ), "InputKey_Action" );
	//CDebug::GetInstance()->Add_DebugText("Plan() RunTime : %f (ms)", fPlanTime);
	//m_pTargetInfo->m_fAngleGap_CameraToPlayer
}
#endif