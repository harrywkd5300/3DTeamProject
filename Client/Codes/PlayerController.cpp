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

	// ���� �������� �÷��̾ ����ߴٸ�
	if (m_pCharacter->Get_IsDead()) 
	{
		CGameObject* pPlayer = nullptr;
		// ���������� �÷��̾� �˻�
		for (int i = 0; i < 4; ++i)
		{
			pPlayer = CObject_Manager::GetInstance()->Get_FindObject(SCENE_STATIC, L"Player", i);
			if (pPlayer != nullptr)
			{
				// ����ִ� �÷��̾� �߰�
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

	// ���� ī�޶� ���� �� �� �ִ� ��Ȳ�̸� �÷��̾� ī�޶� ����.
	if (CCamera_Manager::GetInstance()->Get_IsChangeCamera())
	{
		// ���� ī�޶� �������� �÷��̾��� ī�޶� �ƴ϶�� ����
		if (CCamera_Manager::GetInstance()->Get_CurrentCamera() != m_pCharacter->Get_Camera())
		{
			CCamera_Manager::GetInstance()->Set_CurrentCamera(m_pCharacter->Get_Camera());
		}
	}

	// ī�޶� ����� - �÷��̾� ����� ���� ���� ���
	_vec3 vPlayerLook = *m_pCharacter->Get_Transform()->Get_WorldLookVec();
	vPlayerLook.y = 0.f;
	D3DXVec3Normalize(&vPlayerLook, &vPlayerLook);
	_vec3 vCameraLook = CCamera_Manager::GetInstance()->Get_Direction_NotY();
	vCameraLook.y = 0.f;
	D3DXVec3Normalize(&vCameraLook, &vCameraLook);
	m_pCharacter_Info->m_fAngleGap_CameraToPlayer = D3DXToDegree(acosf(D3DXVec3Dot(&vCameraLook, &vPlayerLook)));
	if (isnan(m_pCharacter_Info->m_fAngleGap_CameraToPlayer))
		m_pCharacter_Info->m_fAngleGap_CameraToPlayer = 0.f;



	// ----------- �ʱ�ȭ -----------	
	Get_TargetInfo(m_pCharacter);
	// �׼� �ñ׳� �ʱ�ȭ
	m_pCharacter_Info->m_WorldState.setVariable(ACT_TYPE_START, ACT_TYPE_END, false);
	Set_Bool_BitTable( InputKey_Action, false );
	// ���� �ñ׳� �ʱ�ȭ
	m_pCharacter_Info->m_WorldState.setVariable(DEST_DIR_0, DEST_DIR_315, false);
	// �÷��̾� Ÿ�� -> �� �Ǵ� �Ʊ�
	m_pCharacter_Info->m_WorldState.setVariable(TARGET_NEAR_ENEMY, true);

	// ���� Ÿ������ ĳ���Ͱ� ����ߴٸ�
	if (m_pTarget != nullptr)
	{
		if (m_pTarget->Get_IsDead())
		{
			m_pTarget = nullptr;
			m_pCharacter_Info->m_WorldState.setVariable(TARGET_TYPE_START, TARGET_TYPE_END, false);
			m_pCharacter_Info->m_WorldState.setVariable(TARGET_NONE, true);
		}
	}
	// ----------- �ʱ�ȭ -----------	

	// ------------------- ���� ������Ʈ �ܺ� ��ȭ ���� -------------------


	const _tchar* pEnemyTargetTag = nullptr;	// ���� ĳ������ �� ���̾� �±�
	const _tchar* pMyTag = nullptr;	// ���� ĳ������ ���̾� �±�
	_uint iSceneNumber = 0;	// ���ѹ�
	_ulong dwChangeActType = 0;	// �ٲ� Act Type..

	// �����ϴ� ĳ���� Ÿ�� ����
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

	//// ---- ����� �� ��ġ ���� ���� ----
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
	//	// ���� �Ÿ��� ���� ������Ʈ�� �ݿ�
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
	//// ---- ����� �� ��ġ ���� �� ----

	// ---- ����� �� ��ġ ���� ���� ----
	CCharacter* pNearTeam = dynamic_cast<CCharacter*>(CObject_Manager::GetInstance()->Get_FindObject_Near_Position(
		iSceneNumber, pMyTag, m_pCharacter));
	float fNearTeamToMeDist = 99999.f;


	// ---- ����� �� ��ġ ���� �� ----

	// ---- Ÿ�� ��ġ ���� ���� ----
	// Ÿ�ٰ��� �Ÿ� �ʱ�ȭ
	float fTargetToMeDist = 0.f;
	m_pCharacter_Info->m_WorldState.setVariable(CH_STATE_CLOSE_TARGET, CH_STATE_TOOFAR_TARGET, false);

	// Ÿ�ٰ� �� �Ÿ� ���
	if (m_pTarget != nullptr)
	{
		fTargetToMeDist = D3DXVec3Length(&(*m_pTarget->Get_Transform()->Get_Infomation(CTransform::INFO_POSITION)
											- *m_pCharacter->Get_Transform()->Get_Infomation(CTransform::INFO_POSITION)));

		// ���� �Ÿ��� ���� ������Ʈ�� �ݿ�
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
	// ---- Ÿ�� ��ġ ���� �� ----

	// ------------------- ���� ������Ʈ �ܺ� ��ȭ �� -------------------


	// ----------------- �ൿ ���� Ű�Է� ���� -------------------
	// �뽬 Ű �Է� üũ
	if (m_pInput->Get_DIKeyState(DIK_LSHIFT))
	{
		m_pCharacter_Info->m_WorldState.setVariable(ACT_SPRINT, true);
	}

	if (false)
	{

	}
	// ��ȣ�ۿ� üũ..
	else if ( nullptr != CAI_Manager::GetInstance()->plan( m_pCharacter_Info->m_WorldState, m_GoalState_Interaction, m_pCharacter_Info->m_vecAction_Act, &m_pCharacter_Info->m_CurAction_Act ) )
	{
		++m_iAniPlanCount;
		m_pCharacter_Info->m_CurAction_Act.Set_ActEffect(m_pCharacter_Info->m_WorldState);
	}
	// ����(ī���� �غ� ����) üũ..
	else if( m_pInput->Get_DIMouseState(CInput_Device::DIM_LBUTTON) && m_pInput->Get_DIMouseState(CInput_Device::DIM_RBUTTON) )
	{
		Set_Bool_BitTable( InputKey_Guard, true );
		Set_Bool_BitTable( IsGuardState, true );
	}
	// ��Ʈ üũ
	else if (nullptr != CAI_Manager::GetInstance()->plan(m_pCharacter_Info->m_WorldState, m_GoalState_Hit, m_pCharacter_Info->m_vecAction_Act, &m_pCharacter_Info->m_CurAction_Act))
	{
		++m_iAniPlanCount;
		m_pCharacter_Info->m_CurAction_Act.Set_ActEffect(m_pCharacter_Info->m_WorldState);
	}
	// ��ȣ�ۿ� / ȸ�� / ���� Ű �Է�
	else if (m_pInput->Get_DIKeyState(DIK_SPACE))
	{
		// ��ȣ�ۿ� �׼�
		if (nullptr != CAI_Manager::GetInstance()->plan(m_pCharacter_Info->m_WorldState, m_GoalState_Interaction, m_pCharacter_Info->m_vecAction_Act, &m_pCharacter_Info->m_CurAction_Act))
		{
			++m_iAniPlanCount;
			m_pCharacter_Info->m_CurAction_Act.Set_ActEffect(m_pCharacter_Info->m_WorldState);
		}
		// ��ȣ�ۿ��� ���ٸ� ȸ�Ǹ��
		else
		{
			// ���¸� �ٷ� �ٲ��� �ʰ� �޾Ƴ��� �� ���������� �ٲٱ⸦ �Ѵ�..
			dwChangeActType = (ACT_SIDESTEP << 1) + true;
			//Set_Bool_BitTable( InputKey_SideStep, true );
		}
		//m_pCharacter_Info->m_WorldState.setVariable(ACT_SIDESTEP, true);
		//ȸ��
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
	// ���� Ű �Է�
	else if (m_pInput->Get_DownMouseState(Engine::CInput_Device::DIM_LBUTTON))
	{
		////Ÿ���� ���� ������?
		//if ( m_pCharacter_Info->m_WorldState.getVariable( TARGET_NONE ) )
		//{
		//	m_pCharacter_Info->m_WorldState.setVariable( SIGNAL_CHANGE_TARGET_NEAR_ENEMY, true );
		//}

		Set_Bool_BitTable( InputKey_Attack, true );
		Set_Bool_BitTable( InputKey_Action, true );
	}
	// ������ Ű �Է�
	else if ( m_pInput->Get_DownMouseState( Engine::CInput_Device::DIM_RBUTTON ) )
	{
		Set_Bool_BitTable( InputKey_Kick, true );
		Set_Bool_BitTable( InputKey_Action, true );
	}
	else if ( m_pInput->Get_DownKeyState( DIK_F ) )
	{
		// ǥâ�� ������ �ڵ带 �߰��� ����..
	}
	else if ( m_pInput->Get_DownKeyState( DIK_G ) )
	{
		// �������� ���� �ڵ带 �߰��� ����..
	}

	// ������ Ű�Է�
	int iInputAngle = 0;
	if (m_pInput->Get_DIKeyState(DIK_W))
	{
		if ( !Get_Bool_BitTable( InputKey_Action ) )
		{
			// ���¸� �ٷ� �ٲ��� �ʰ� �޾Ƴ��� �� ���������� �ٲٱ⸦ �Ѵ�..
			dwChangeActType = (ACT_RUN << 1) + true;

			Set_Bool_BitTable( InputKey_Action, true );
		}
		iInputAngle += 1;
	}
	if (m_pInput->Get_DIKeyState(DIK_S))
	{
		if ( !Get_Bool_BitTable( InputKey_Action ) )
		{
			// ���¸� �ٷ� �ٲ��� �ʰ� �޾Ƴ��� �� ���������� �ٲٱ⸦ �Ѵ�..
			dwChangeActType = (ACT_RUN << 1) + true;

			Set_Bool_BitTable( InputKey_Action, true );
		}
		iInputAngle += -1;
	}
	if (m_pInput->Get_DIKeyState(DIK_D))
	{
		if ( !Get_Bool_BitTable( InputKey_Action ) )
		{
			// ���¸� �ٷ� �ٲ��� �ʰ� �޾Ƴ��� �� ���������� �ٲٱ⸦ �Ѵ�..
			dwChangeActType = (ACT_RUN << 1) + true;

			Set_Bool_BitTable( InputKey_Action, true );
		}
		iInputAngle += 3;
	}
	if (m_pInput->Get_DIKeyState(DIK_A))
	{
		if ( !Get_Bool_BitTable( InputKey_Action ) )
		{
			// ���¸� �ٷ� �ٲ��� �ʰ� �޾Ƴ��� �� ���������� �ٲٱ⸦ �Ѵ�..
			dwChangeActType = (ACT_RUN << 1) + true;

			Set_Bool_BitTable( InputKey_Action, true );
		}
		iInputAngle += -3;
	}

	// ----------------- �ൿ ���� Ű�Է� �� -------------------
	// ------ �ൿ�� ���� �� ��, ���彺����Ʈ ��ȭ�� ���� �Ķ���� ���� ���� ------

	// Ÿ�� ������Ʈ
	Update_CharacterTarget(pNearEnemy, pNearTeam,m_pCharacter,m_pCharacter_Info->m_pEnemy_Attacked);

	//// ĳ���� �ޱ� ����
	//Update_CharacterAngle();

	// ------ �ൿ�� ���� �� ��, ���彺����Ʈ ��ȭ�� ���� �Ķ���� ���� �� ------


	// ----------------- �ִϸ��̼� ���� AI ���� -------------------
	if ( m_pCharacter->Get_Mesh()->is_Finish_Animation() >= 1
		 || m_pCharacter_Info->m_WorldState.getVariable( SIGNAL_CHANGE_ANIMATION_FORCED )
		 )
	{
		m_pCharacter_Info->m_WorldState.setVariable( SIGNAL_CHANGE_ANIMATION_FORCED, false );
		m_pCharacter_Info->m_WorldState.setVariable( SIGNAL_CHANGE_ANIMATION, true );

		if ( m_pCharacter->Get_Mesh()->is_Finish_Animation() >= 1 )	// �ִϸ��̼��� ������ ������ ������ �͵�..
		{
			Set_Bool_BitTable( IsGuardState, false );
		}

		// ���� ������ ���õ� ���� �ʱ�ȭ..
		Set_Bool_BitTable( IsAttackState, false );
		Set_Bool_BitTable( IsKickState, false );
		Set_Bool_BitTable( IsSideStepState, false );

		// ���� ���� Ű�� �����ٸ� ������ �ϰԲ�..
		if ( Get_Bool_BitTable( InputKey_Attack ) == true )
		{
			Set_Bool_BitTable( InputKey_Attack, false );
			Set_Bool_BitTable( IsAttackState, true );

			// ������ ���⿡ Ÿ���� �ִ� �� �˻��ϰ� �ִٸ� Ÿ�� ������ ����..
			Update_AttackTargetData( &pNearEnemy, iInputAngle, iSceneNumber, pEnemyTargetTag );
		}
		else if ( Get_Bool_BitTable( InputKey_Kick ) == true )
		{
			Set_Bool_BitTable( InputKey_Kick, false );
			Set_Bool_BitTable( IsKickState, true );

			// ������ ���⿡ Ÿ���� �ִ� �� �˻��ϰ� �ִٸ� Ÿ�� ������ ����..
			Update_AttackTargetData( &pNearEnemy, iInputAngle, iSceneNumber, pEnemyTargetTag );
		}
	}

	if ( Get_Bool_BitTable( InputKey_Guard ) == true )
	{
		// ���¸� �ٷ� �ٲ��� �ʰ� �޾Ƴ��� �� ���������� �ٲٱ⸦ �Ѵ�..
		dwChangeActType = (ACT_GUARD << 1) + true;
		Set_Bool_BitTable( InputKey_Guard, false );
	}
	else if ( true == Get_Bool_BitTable( IsGuardState ) )
	{
		if ( false == Get_Bool_BitTable( InputKey_Action ) )
		{
			// ���¸� �ٷ� �ٲ��� �ʰ� �޾Ƴ��� �� ���������� �ٲٱ⸦ �Ѵ�..
			dwChangeActType = (ACT_GUARD << 1) + true;
		}
		else
			Set_Bool_BitTable( IsGuardState, false );
	}
	else if ( Get_Bool_BitTable( IsSideStepState ) == true )	// ���� ȸ��(������) ���� ���..
	{
		// ���¸� �ٷ� �ٲ��� �ʰ� �޾Ƴ��� �� ���������� �ٲٱ⸦ �Ѵ�..
		dwChangeActType = (ACT_SIDESTEP << 1) + true;
	}
	else if ( Get_Bool_BitTable( IsAttackState ) == true ) // ���� ���� ���� ���..
	{
		// ���¸� �ٷ� �ٲ��� �ʰ� �޾Ƴ��� �� ���������� �ٲٱ⸦ �Ѵ�..
		dwChangeActType = ( ACT_ATTACK << 1 ) + true;
	}
	else if ( Get_Bool_BitTable( IsKickState ) == true ) // ���� ������ ���� ���� ���..
	{
		// ���¸� �ٷ� �ٲ��� �ʰ� �޾Ƴ��� �� ���������� �ٲٱ⸦ �Ѵ�..
		dwChangeActType = ( ACT_KICK << 1 ) + true;
	}

	// ��� ���̽��� �ƴ� ��� WAIT �׼� ���·�..
	if ( !Get_Bool_BitTable( InputKey_Action ) )
	{
		if ( 0 == dwChangeActType )
		{
			m_pCharacter_Info->m_WorldState.setVariable( SIGNAL_CHANGE_TARGET_NONE, true );

			// ���¸� �ٷ� �ٲ��� �ʰ� �޾Ƴ��� �� ���������� �ٲٱ⸦ �Ѵ�..
			dwChangeActType = ( ACT_IDLE << 1 ) + true;
		}
	}
	else
	{
		if ( false == m_pCharacter_Info->m_WorldState.getVariable( CH_STATE_ATTACK ) &&
			 false == Get_Bool_BitTable( IsSideStepState ) )
		{
			m_pCharacter_Info->m_fAngleSpeed = 5.f;

			//Ű �Է¿� ���� �ޱ� ���
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

	if ( dwChangeActType != 0 )	// ���¸� �ٲ�� �� ��..
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

	// �̵�Ű�� �Է��� ��� �����ο� �̵��� �����ؾ���
	if ( m_pCharacter_Info->m_WorldState.getVariable( ACT_RUN ) )
		m_pCharacter_Info->m_WorldState.setVariable( SIGNAL_CHANGE_TARGET_NONE, true );

	// �ִϸ��̼� ����Ǿ�� �ϸ�?
	if (m_pCharacter_Info->m_WorldState.getVariable(SIGNAL_CHANGE_ANIMATION) == true)
	{
		// ----------------- �ִϸ��̼� ���� ��ȭ�� �ִ� ĳ���� ������Ʈ �ʱ�ȭ -------------------
		m_pCharacter_Info->m_WorldState.setVariable( CH_STATE_TYPE_BY_ANI_START, CH_STATE_TYPE_BY_ANI_END, false );

#ifndef _FINAL
		float fTime = 0.f;
		bool bErr = false;
		CHECK_TIME_START
#endif
		// �ִϸ��̼� ����
		if (nullptr != CAI_Manager::GetInstance()->plan(m_pCharacter_Info->m_WorldState, m_GoalState_Animation, m_pCharacter_Info->m_vecAction_Ani, &m_pCharacter_Info->m_CurAction_Ani))
			{
				++m_iAniPlanCount;
				m_pCharacter_Info->m_WorldState.setVariable(m_pCharacter_Info->m_StartAniNum, m_pCharacter_Info->m_EndAniNum, false);
				m_pCharacter_Info->m_CurAction_Ani.Set_ActEffect(m_pCharacter_Info->m_WorldState);
			}
#ifndef _FINAL
		CHECK_TIME_END(fPlanTime, bErr);
#endif

		//�ִϸ��̼� ����
		m_iCurAniNum = m_pCharacter_Info->m_CurAction_Ani.Get_Animation(m_pCharacter_Info->m_StartAniNum, m_pCharacter_Info->m_EndAniNum) - (m_pCharacter_Info->m_StartAniNum + 1);
		m_pCharacter->Get_Mesh()->Set_AnimationSet(m_iCurAniNum);

		//���� �ִϸ��̼� ���� ������
		m_iCurAniEventSize = m_pCharacter->Get_Mesh()->Get_CurAnimationInfo().iAni_Index;
		m_iCurAniEventNum = 0;
		m_curAniEvent = m_pCharacter->Get_Mesh()->Get_CurAnimationInfo().m_AniEvent;

		m_pCharacter_Info->m_WorldState.setVariable(STATE_TYPE::SIGNAL_CHANGE_ANIMATION, false);

	}
	// ----------------- �ִϸ��̼� ���� AI �� -------------------


	// --------- �ൿ/�ִϸ��̼����� ��ȭ�� ���� �����̽��� ���� ������ ó�� ���� ----------

	// Ÿ�� ������Ʈ
	Update_CharacterTarget(pNearEnemy, pNearTeam, m_pCharacter, m_pCharacter_Info->m_pEnemy_Attacked);

	// ĳ���� �ޱ� ����
	Update_CharacterAngle();

	// ĳ���Ϳ� Ÿ�� ���� �Ÿ� ����..
	Update_TargetDistance( fTimeDelta );


	// �޸��� ���� ó��
	if (m_pCharacter_Info->m_WorldState.getVariable(CH_STATE_RUN))
	{
		if (m_pCharacter_Info->m_WorldState.getVariable(CH_STATE_SPRINT))
			m_pCharacter->Move_Forward(2.0f);
		else
			m_pCharacter->Move_Forward(1.f);
	}

	// ---- �ִϸ��̼� �̺�Ʈ ó�� ����----
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
	// ---- �ִϸ��̼� �̺�Ʈ ó�� ��----

	// --------- �ൿ/�ִϸ��̼����� ��ȭ�� ���� �����̽��� ���� ������ ó�� �� ----------

	// UI�� ���� �ѱ��.
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

	// �÷��� ĳ���� ����
	if (Engine::CInput_Device::GetInstance()->Get_DownKeyState(DIK_UP))
	{
		CGameObject* pPlayer = nullptr;
		pPlayer = CObject_Manager::GetInstance()->Get_FindObject(SCENE_STATIC, L"Player", 0);
		if (pPlayer != nullptr)
		{
			// ����ִ� �÷��̾�
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
			// ����ִ� �÷��̾�
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
			// ����ִ� �÷��̾�
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
			// ����ִ� �÷��̾�
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

	// ���� Ÿ�ٿ��� ���� ��Ʈ�ѷ� ��ȯ
	if (nullptr != m_pCharacter)
		UnPossess();

	// ���� ������ Ÿ���� ��Ʈ�ѷ��� ����
	m_pTargetExController = pTarget->Get_Controller();


	//Ÿ�� �缳��
	m_pCharacter = pTarget;
	Get_TargetInfo(m_pCharacter);

	// Ÿ���� ������ �ִ� ��Ʈ�ѷ��� this�� �ƴ϶�� ����
	//if (m_pTarget->Get_Controller() != this)
	m_pCharacter->Set_Controller(this);

	m_iTargetAniSetCount = m_pCharacter->Get_Mesh()->Get_MaxAniSet();

}

bool CPlayerController::UnPossess()
{
	if (m_pCharacter != nullptr)
	{	// ���� Ÿ�ٿ��� ���� ��Ʈ�ѷ��� ����
		m_pTargetExController->Possess(m_pCharacter);
		//m_pTarget->Set_Controller(m_pTargetExController);
	}
	// Ÿ�� �ʱ�ȭ
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
//	//���� ī�޶� �÷��̾� ī�޶��
//	if (CCamera_Manager::GetInstance()->Get_CurrentCamera() == m_pTarget->Get_Camera())
//	{
//		// ī�޶�
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
		if ( m_iPrevAttInputKey == _iInputKey )	// ���� ���� ���� ���� ���� ���� Ű�� �����ٸ�..
			return false;

		return true;
	}
	else
		return true;

	return false;
}

void CPlayerController::Update_CharacterAngle( void )
{
	//ĳ���� �ޱ� ���� �ñ׳�
	if (m_pCharacter_Info->m_WorldState.getVariable(SIGNAL_CHANGE_ANGLE)) 
	{
		// Ÿ�ٰ��� �ޱ۰� ���
		if (m_pCharacter_Info->m_WorldState.getVariable(SIGNAL_SET_ANGLE_TARGET)
			 && m_pCharacter_Info->m_WorldState.getVariable(CH_STATE_TOOFAR_TARGET)
			 )
		{
			if (m_pTarget != nullptr)
			{
				m_fDestinationAngle = Get_Angle( *m_pTarget->Get_Transform()->Get_Infomation( Engine::CTransform::INFO_POSITION ) );

				Set_Angle( m_pCharacter->Get_Transform()->Get_WorldMatrix(), m_fDestinationAngle );
				Compute_DirWorldState();	// �÷��̾� ���� �ޱ۰� ���彺����Ʈ�� �Է�

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
				//Compute_DirWorldState();	// �÷��̾� ���� �ޱ۰� ���彺����Ʈ�� �Է�
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
			//Compute_DirWorldState();	// �÷��̾� ���� �ޱ۰� ���彺����Ʈ�� �Է�
		}
	}
	else if (m_pCharacter_Info->m_WorldState.getVariable(SIGNAL_CHANGE_ANGLE_FORCED))
	{// Ÿ�ٰ��� �ޱ۰� ���
		if (m_pCharacter_Info->m_WorldState.getVariable(SIGNAL_SET_ANGLE_TARGET)
			 && m_pCharacter_Info->m_WorldState.getVariable(CH_STATE_TOOFAR_TARGET)
			 )
		{	
			if (m_pTarget != nullptr)
			{
				m_fDestinationAngle = Get_Angle( *m_pTarget->Get_Transform()->Get_Infomation( Engine::CTransform::INFO_POSITION ) );

				Set_Angle( m_pCharacter->Get_Transform()->Get_WorldMatrix(), m_fDestinationAngle );
				Compute_DirWorldState();	// �÷��̾� ���� �ޱ۰� ���彺����Ʈ�� �Է�

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
		// Ÿ�ٰ��� �ޱ۰� ���
		if (m_pCharacter_Info->m_WorldState.getVariable(SIGNAL_SET_ANGLE_TARGET)
			 && m_pCharacter_Info->m_WorldState.getVariable(CH_STATE_TOOFAR_TARGET)
			 )
		{
			if (m_pTarget != nullptr)
			{
				m_fDestinationAngle = Get_Angle(*m_pTarget->Get_Transform()->Get_Infomation(CTransform::INFO_POSITION));

				// �÷��̾� ���� �ޱ۰� ���彺����Ʈ�� �Է�
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
		if (fabsf(360.f - fAngleGap) <= m_pTimer->Get_TimeDelta() * m_pCharacter_Info->m_fAngleSpeed) // Add By.KMH - �ޱ� ü���� ������ ������ �ٸ���(Test)..
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
		if ((fAngleGap) <= m_pTimer->Get_TimeDelta() * m_pCharacter_Info->m_fAngleSpeed) // Add By.KMH - �ޱ� ü���� ������ ������ �ٸ���(Test)..
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
	// Ÿ���� �ִ��� ������ ���� üũ..
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
		case 1: // ��
			return 0.f;
		case 4: // �ϵ�
			return -315.f;
		case 3: // ��
			return -270.f;
		case 2: // ����
			return -225.f;		
		case -1: // ��
			return -180.f;
		case -4: // ����
			return -135.f;
		case -3: // ��
			return -90.f;
		case -2: // ����
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

	// �÷��̾� ���� �ޱ۰� ���彺����Ʈ�� �Է�
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
	//Ű �Է¿� ���� �ޱ� ���
	_float fAngle = Compute_CamAngleToCharAngle( 360.f + Compute_DirToInputKey( _iInputKey ) ); // 0 ~ 360 ������ ������ ������..

	// ---- ����� �� ��ġ ���� ���� ----
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

	// ���� �Ÿ��� ���� ������Ʈ�� �ݿ�
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