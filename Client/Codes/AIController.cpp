#include "stdafx.h"
#include "Monster.h"
#include "AIController.h"
#include "Character.h"
#include "Component_Manager.h"
#include "Object_Manager.h"
#include "Camera_Manager.h"
#include "PlayerController.h"
#include "Management.h"
#include "Scene.h"

CAIController::CAIController(LPDIRECT3DDEVICE9 pGraphicDev)
	: CController(pGraphicDev)
	,m_pInput(CInput_Device::GetInstance())
{
	m_pInput->AddRef();
#ifndef _FINAL
	Tag(L"CAIController", std::bind(&CAIController::Debug_Render, this));
#endif

}

CAIController::CAIController(const CAIController & rhs)
	: CController(rhs)
	,m_pInput(CInput_Device::GetInstance())
	//, m_GoalState_Attack(rhs.m_GoalState_Attack)
	//, m_GoalState_SideStep(rhs.m_GoalState_SideStep)
	//, m_GoalState_Run(rhs.m_GoalState_Run)
{
	m_pInput->AddRef();
#ifndef _FINAL
	Tag(L"CAIController", std::bind(&CAIController::Debug_Render, this));
#endif

}

HRESULT CAIController::Ready_Controller(void)
{
	if (FAILED(CController::Ready_GoalState()))
	{
		MSG_BOX("CAIController::Ready_GoalState() Failed");
		return E_FAIL;
	}
	return NOERROR;
}

void CAIController::Update_Component(const _float & fTimeDelta)
{
	if (m_pCharacter == nullptr)
		return;

#ifndef _FINAL
	float fTime = 0.f;
	bool bErr = false;
	CHECK_TIME_START
#endif

	if (m_iCurAniNum >= m_iTargetAniSetCount)
		m_iCurAniNum = 0;
	// ���� �������� �÷��̾ ����ߴٸ�
	if (m_pCharacter->Get_IsDead())
	{
		//UnPossess();//���� ������...
		m_pCharacter = nullptr;
		return;
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
	// ���� �ñ׳� �ʱ�ȭ
	m_pCharacter_Info->m_WorldState.setVariable(DEST_DIR_0, DEST_DIR_315, false);
	m_pCharacter_Info->m_WorldState.setVariable(TARGET_STATE_TYPE_START, TARGET_STATE_TYPE_START, false);
	
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
	// ----------- �ʱ�ȭ �� -----------

	
	// ------------------- ���� ������Ʈ �ܺ� ��ȭ ���� -------------------
	

	// ------ Ÿ�ٰ��� ��ġ ���� ���� ------
	const _tchar* pEnemyTargetTag = nullptr;	// ���� ĳ������ �� ���̾� �±�
	const _tchar* pMyTag = nullptr;	// ���� ĳ������ ���̾� �±�
	_uint iSceneNumber = 0;	// ���ѹ�

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

	// ---- �÷��̾� ��ġ ���� ���� ----
	// �÷��̾� �Ÿ� �ʱ�ȭ
	m_pCharacter_Info->m_WorldState.setVariable(CH_STATE_CLOSE_PLAYER, CH_STATE_TOOFAR_PLAYER, false);

	// ���� �÷��̾�� �Ÿ� ���
	CCharacter* pPlayer = dynamic_cast<CPlayerController*>(CComponent_Manager::GetInstance()->Find_Component(0, L"CPlayerController"))->Get_Character();
	float fPlayerToMeDist = 99999.f;
	if (pPlayer != nullptr)
	{
		fPlayerToMeDist = D3DXVec3Length(&(*pPlayer->Get_Transform()->Get_Infomation(CTransform::INFO_POSITION)
			- *m_pCharacter->Get_Transform()->Get_Infomation(CTransform::INFO_POSITION)));

		// ���� �Ÿ��� ���� ������Ʈ�� �ݿ�
		if (fPlayerToMeDist < m_pCharacter_Info->m_fRange_Close)
			m_pCharacter_Info->m_WorldState.setVariable(CH_STATE_CLOSE_PLAYER, true);
		if (fPlayerToMeDist < m_pCharacter_Info->m_fRange_Near)
			m_pCharacter_Info->m_WorldState.setVariable(CH_STATE_NEAR_PLAYER, true);
		if (fPlayerToMeDist < m_pCharacter_Info->m_fRange_Mid)
			m_pCharacter_Info->m_WorldState.setVariable(CH_STATE_MID_PLAYER, true);
		if (fPlayerToMeDist < m_pCharacter_Info->m_fRange_Far)
			m_pCharacter_Info->m_WorldState.setVariable(CH_STATE_FAR_PLAYER, true);
		if (fPlayerToMeDist < m_pCharacter_Info->m_fRange_TooFar)
			m_pCharacter_Info->m_WorldState.setVariable(CH_STATE_TOOFAR_PLAYER, true);
	}
	// ---- �÷��̾� ��ġ ���� �� ----

	// ---- ����� �� ��ġ ���� ���� ----
	CCharacter* pNearTeam = dynamic_cast<CCharacter*>(CObject_Manager::GetInstance()->Get_FindObject_Near_Position(
		iSceneNumber, pMyTag, m_pCharacter));
	float fNearTeamToMeDist = 99999.f;


	// ---- ����� �� ��ġ ���� �� ----

	// ---- ����� �� ��ġ ���� ���� ----
	CCharacter* pNearEnemy = dynamic_cast<CCharacter*>(CObject_Manager::GetInstance()->Get_FindObject_Near_Position(
		iSceneNumber, pEnemyTargetTag, m_pCharacter));
	float fNearEnemyToMeDist = 99999.f;

	if (pNearEnemy != nullptr)
	{
		fNearEnemyToMeDist = D3DXVec3Length(&(*pNearEnemy->Get_Transform()->Get_Infomation(CTransform::INFO_POSITION)
			- *m_pCharacter->Get_Transform()->Get_Infomation(CTransform::INFO_POSITION)));

		// ���� �Ÿ��� ���� ������Ʈ�� �ݿ�
		if (fNearEnemyToMeDist < m_pCharacter_Info->m_fRange_Close)
			m_pCharacter_Info->m_WorldState.setVariable(CH_STATE_CLOSE_NEAR_ENEMY, true);
		if (fNearEnemyToMeDist < m_pCharacter_Info->m_fRange_Near)
			m_pCharacter_Info->m_WorldState.setVariable(CH_STATE_NEAR_NEAR_ENEMY, true);
		if (fNearEnemyToMeDist < m_pCharacter_Info->m_fRange_Mid)
			m_pCharacter_Info->m_WorldState.setVariable(CH_STATE_MID_NEAR_ENEMY, true);
		if (fNearEnemyToMeDist < m_pCharacter_Info->m_fRange_Far)
			m_pCharacter_Info->m_WorldState.setVariable(CH_STATE_FAR_NEAR_ENEMY, true);
		if (fNearEnemyToMeDist < m_pCharacter_Info->m_fRange_TooFar)
			m_pCharacter_Info->m_WorldState.setVariable(CH_STATE_TOOFAR_NEAR_ENEMY, true);
		
		if(fNearEnemyToMeDist < m_pCharacter_Info->m_fRange_FoundEnemy)
			m_pCharacter_Info->m_WorldState.setVariable(CH_STATE_FOUND_ENEMY, true);
		//else
		//	m_pCharacter_Info->m_WorldState.setVariable(CH_STATE_FOUND_ENEMY, false);
	}
	// ---- ����� �� ��ġ ���� �� ----
	

	// ---- Ÿ�� ��ġ ���� ���� ----
	// Ÿ�ٰ��� �Ÿ� �ʱ�ȭ
	float fTargetToMeDist = 99999.f;
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
	
	//// ---- ���� �� �� ��ġ ���� ���� ----
	//CCharacter* pEnemy_Attacked = m_pCharacter_Info->m_pEnemy_Attacked; // �׾����� Ȯ���ؾ� ������?

	//if (m_pCharacter_Info->m_WorldState.getVariable(SIGNAL_HIT_ENEMY))
	//{
	//	if (pEnemy_Attacked != nullptr)
	//	{
	//		float fAttackedEnemyToMeDist = 99999.f;

	//		fAttackedEnemyToMeDist = D3DXVec3Length(&(*m_pCharacter_Info->m_pEnemy_Attacked->Get_Transform()->Get_Infomation(CTransform::INFO_POSITION)
	//			- *m_pCharacter->Get_Transform()->Get_Infomation(CTransform::INFO_POSITION)));
	//	}
	//}
	//// ---- ���� �� �� ��ġ ���� �� ----

	// ---- Ÿ�� ���� ���� ���� ----
	if (m_pTarget != nullptr)
	{
		if (m_pTarget->Get_Info().m_WorldState.getVariable(CH_STATE_ATTACK))
			m_pCharacter_Info->m_WorldState.setVariable(TARGET_STATE_ATTACK, true);
		else
			m_pCharacter_Info->m_WorldState.setVariable(TARGET_STATE_ATTACK, false);
	}
	// ---- Ÿ�� ���� ���� �� ----

	// ------------------- ���� ������Ʈ �ܺ� ��ȭ �� -------------------


	int iSignalCount = 0;
	do 
	{
		if (nullptr != CAI_Manager::GetInstance()->plan(m_pCharacter_Info->m_WorldState, m_GoalState_Signal, m_pCharacter_Info->m_vecAction_Act, &m_pCharacter_Info->m_CurAction_Act))
			m_pCharacter_Info->m_CurAction_Act.Set_ActEffect(m_pCharacter_Info->m_WorldState);
		else
			break;
	} while (++iSignalCount <= SIGNAL_SUCCESS_MAX_COUNT);
	
	// ----------------- �ൿ ���� AI ���� -------------------
	
	// �ൿ ���� (������ ���� �ֻ��� ��ǥ)
	// ���� �ൿ
	if (nullptr != CAI_Manager::GetInstance()->plan(m_pCharacter_Info->m_WorldState, m_GoalState_Death, m_pCharacter_Info->m_vecAction_Act, &m_pCharacter_Info->m_CurAction_Act))
	{
		++m_iActPlanCount;
		m_pCharacter_Info->m_CurAction_Act.Set_ActEffect(m_pCharacter_Info->m_WorldState);
	}
	// ��Ʈ �ൿ
	if (nullptr != CAI_Manager::GetInstance()->plan(m_pCharacter_Info->m_WorldState, m_GoalState_Hit, m_pCharacter_Info->m_vecAction_Act, &m_pCharacter_Info->m_CurAction_Act))
	{
		++m_iActPlanCount;
		m_pCharacter_Info->m_CurAction_Act.Set_ActEffect(m_pCharacter_Info->m_WorldState);
	}
	// ȸ�� �ൿ
	else if (nullptr != CAI_Manager::GetInstance()->plan(m_pCharacter_Info->m_WorldState, m_GoalState_SideStep, m_pCharacter_Info->m_vecAction_Act, &m_pCharacter_Info->m_CurAction_Act))
	{
		++m_iActPlanCount;
		m_pCharacter_Info->m_CurAction_Act.Set_ActEffect(m_pCharacter_Info->m_WorldState);
	}
	// ���� �ൿ
	else if (nullptr != CAI_Manager::GetInstance()->plan(m_pCharacter_Info->m_WorldState, m_GoalState_Attack, m_pCharacter_Info->m_vecAction_Act, &m_pCharacter_Info->m_CurAction_Act))
	{
		++m_iActPlanCount;
		m_pCharacter_Info->m_CurAction_Act.Set_ActEffect(m_pCharacter_Info->m_WorldState);
	}
	// �̵� �ൿ
	else if (nullptr != CAI_Manager::GetInstance()->plan(m_pCharacter_Info->m_WorldState, m_GoalState_Run, m_pCharacter_Info->m_vecAction_Act, &m_pCharacter_Info->m_CurAction_Act))
	{
		++m_iActPlanCount;
		m_pCharacter_Info->m_CurAction_Act.Set_ActEffect(m_pCharacter_Info->m_WorldState);
	}
	// �̵� �ൿ
	else if (nullptr != CAI_Manager::GetInstance()->plan(m_pCharacter_Info->m_WorldState, m_GoalState_Walk, m_pCharacter_Info->m_vecAction_Act, &m_pCharacter_Info->m_CurAction_Act))
	{
		++m_iActPlanCount;
		m_pCharacter_Info->m_CurAction_Act.Set_ActEffect(m_pCharacter_Info->m_WorldState);
	}
	// ��� �ൿ
	else if(nullptr != CAI_Manager::GetInstance()->plan(m_pCharacter_Info->m_WorldState, m_GoalState_Idle, m_pCharacter_Info->m_vecAction_Act, &m_pCharacter_Info->m_CurAction_Act))
	{
		++m_iActPlanCount;
		m_pCharacter_Info->m_CurAction_Act.Set_ActEffect(m_pCharacter_Info->m_WorldState);
	}

	// ----------------- �ൿ ���� AI �� -------------------

	// ------ �ൿ�� ���� �� ��, ���彺����Ʈ ��ȭ�� ���� �Ķ���� ���� ���� ------

	// Ÿ�� ������Ʈ
	Update_CharacterTarget(pNearEnemy, pNearTeam, pPlayer, m_pCharacter_Info->m_pEnemy_Attacked);

	// ĳ���� �ޱ� ����
	Update_CharacterAngle();

	if ( m_pCharacter_Info->m_WorldState.getVariable( SIGNAL_CHANGE_DIR_FRONT ) )
	{
		m_pCharacter_Info->m_WorldState.setVariable( MOVE_DIR_RIGHT, MOVE_DIR_LEFT, false );
		m_pCharacter_Info->m_WorldState.setVariable( MOVE_DIR_FRONT, true );
		m_pCharacter_Info->m_WorldState.setVariable( SIGNAL_CHANGE_DIR_FRONT, false );
	}
	else if ( m_pCharacter_Info->m_WorldState.getVariable( SIGNAL_CHANGE_DIR_RIGHT ) )
	{
		m_pCharacter_Info->m_WorldState.setVariable( MOVE_DIR_RIGHT, MOVE_DIR_LEFT, false );
		m_pCharacter_Info->m_WorldState.setVariable( MOVE_DIR_RIGHT, true );
		m_pCharacter_Info->m_WorldState.setVariable( SIGNAL_CHANGE_DIR_RIGHT, false );
	}
	else if ( m_pCharacter_Info->m_WorldState.getVariable( SIGNAL_CHANGE_DIR_LEFT ) )
	{
		m_pCharacter_Info->m_WorldState.setVariable( MOVE_DIR_RIGHT, MOVE_DIR_LEFT, false );
		m_pCharacter_Info->m_WorldState.setVariable( MOVE_DIR_LEFT, true );
		m_pCharacter_Info->m_WorldState.setVariable( SIGNAL_CHANGE_DIR_LEFT, false );
	}
	else if ( m_pCharacter_Info->m_WorldState.getVariable( SIGNAL_CHANGE_DIR_BACK ) )
	{
		m_pCharacter_Info->m_WorldState.setVariable( MOVE_DIR_RIGHT, MOVE_DIR_LEFT, false );
		m_pCharacter_Info->m_WorldState.setVariable( MOVE_DIR_BACK, true );
		m_pCharacter_Info->m_WorldState.setVariable( SIGNAL_CHANGE_DIR_BACK, false );
	}

	// ------ �ൿ�� ���� �� ��, ���彺����Ʈ ��ȭ�� ���� �Ķ���� ���� �� ------

	// ----------------- �ִϸ��̼� ���� AI ���� -------------------
	if (m_pCharacter->Get_Mesh()->is_Finish_Animation() >= 1
		|| m_pCharacter_Info->m_WorldState.getVariable(SIGNAL_CHANGE_ANIMATION_FORCED)
		)
	{
		m_pCharacter_Info->m_WorldState.setVariable(SIGNAL_CHANGE_ANIMATION_FORCED, false);
		m_pCharacter_Info->m_WorldState.setVariable(SIGNAL_CHANGE_ANIMATION, true);
	}

	// �ִϸ��̼� ����Ǿ�� �ϸ�?
	if (m_pCharacter_Info->m_WorldState.getVariable(SIGNAL_CHANGE_ANIMATION))
	{
		// ----------------- �ִϸ��̼� ���� ��ȭ�� �ִ� ĳ���� ������Ʈ �ʱ�ȭ ���� -------------------
		m_pCharacter_Info->m_WorldState.setVariable(CH_STATE_TYPE_BY_ANI_START, CH_STATE_TYPE_BY_ANI_END, false);
		//if (!m_pCharacter_Info->m_WorldState.getVariable(ACT_RUN))
		//{
		//	m_pCharacter_Info->m_WorldState.setVariable(CH_STATE_RUN, false);
		//}


		// ----------------- �ִϸ��̼� ���� ��ȭ�� �ִ� ĳ���� ������Ʈ �ʱ�ȭ �� -------------------
//#ifnde#ifndef _FINAL
//CHECK_TIME_END(fPlanTime, bErr);
//CDebug::GetInstance()->Add_AIRunTime(fPlanTime);
//#endif
			// �ִϸ��̼� ����
			if (nullptr != CAI_Manager::GetInstance()->plan(m_pCharacter_Info->m_WorldState, m_GoalState_Animation, m_pCharacter_Info->m_vecAction_Ani, &m_pCharacter_Info->m_CurAction_Ani))
			{
				++m_iAniPlanCount;
				m_pCharacter_Info->m_WorldState.setVariable(m_pCharacter_Info->m_StartAniNum, m_pCharacter_Info->m_EndAniNum, false);
				m_pCharacter_Info->m_CurAction_Ani.Set_ActEffect(m_pCharacter_Info->m_WorldState);
			}
//#ifndef _FINAL
//		CHECK_TIME_END(fPlanTime, bErr);
//		CDebug::GetInstance()->Add_AIRunTime(fPlanTime);
//#endif
		// ���õ� �ִϸ��̼� ����
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
	Update_CharacterTarget(pNearEnemy, pNearTeam, pPlayer, m_pCharacter_Info->m_pEnemy_Attacked);

	// ĳ���� �ޱ� ����
	Update_CharacterAngle();

	
	// �޸��� ���� ó��
	float fScale = 0.f;
	if ( m_pCharacter_Info->m_WorldState.getVariable( CH_STATE_SPRINT ) )
		fScale = 2.f;
	else if ( m_pCharacter_Info->m_WorldState.getVariable( CH_STATE_RUN ) )
		fScale = 1.5f;
	else if ( m_pCharacter_Info->m_WorldState.getVariable( CH_STATE_WALK ) )
		fScale = 0.5f;

	if (fScale > 0.00001f)
	{
		if (m_pCharacter_Info->m_WorldState.getVariable(MOVE_DIR_RIGHT))
		{
			m_pCharacter->Move_Right(fScale);
		}
		else if (m_pCharacter_Info->m_WorldState.getVariable(MOVE_DIR_LEFT))
		{
			m_pCharacter->Move_Right(-fScale);
		}
		else if (m_pCharacter_Info->m_WorldState.getVariable(MOVE_DIR_BACK))
		{
			m_pCharacter->Move_Forward(-fScale);
		}
		else
		{
			m_pCharacter->Move_Forward(fScale);
		}
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
	//// ---- �ִϸ��̼� �̺�Ʈ ó�� ��----

	// --------- �ൿ/�ִϸ��̼����� ��ȭ�� ���� �����̽��� ���� ������ ó�� �� ----------

	//// ī�޶� ����� - �÷��̾� ����� ���� ����
	//_vec3 vPlayerLook = *m_pCharacter->Get_Transform()->Get_WorldLookVec();
	//vPlayerLook.y = 0.f;
	//D3DXVec3Normalize(&vPlayerLook, &vPlayerLook);
	//_vec3 vCameraLook = CCamera_Manager::GetInstance()->Get_Direction_NotY();
	//vCameraLook.y = 0.f;
	//D3DXVec3Normalize(&vCameraLook, &vCameraLook);

	m_pCharacter_Info->m_fAngleGap_CameraToPlayer = D3DXToDegree(acosf(D3DXVec3Dot(&vCameraLook, &vPlayerLook)));
	if (isnan(m_pCharacter_Info->m_fAngleGap_CameraToPlayer))
		m_pCharacter_Info->m_fAngleGap_CameraToPlayer = 0.f;


#ifndef _FINAL
	CHECK_TIME_END(fPlanTime, bErr);
	CDebug::GetInstance()->Add_AIRunTime(fPlanTime);
#endif

}

void CAIController::LastUpdate_Component(const _float & fTimeDelta)
{

}


CAIController * CAIController::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CAIController *		pInstance = new CAIController(pGraphicDev);

	if (FAILED(pInstance->Ready_Controller()))
	{
		MSG_BOX("CAIController Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CAIController::Clone(void)
{
	return new CAIController(*this);
}

_ulong CAIController::Free(void)
{
	Safe_Release(m_pInput);

	return CController::Free();
}

void CAIController::Possess(CCharacter * pTarget)
{
	if (nullptr == pTarget)
		return;

	//// ���� Ÿ�ٿ��� ���� ��Ʈ�ѷ� ��ȯ
	//if (nullptr != m_pTarget)
	//	UnPossess();

	//// ���� ������ Ÿ���� ��Ʈ�ѷ��� ����
	//m_pTargetExController = pTarget->Get_Controller();


	// ���� Ÿ�� ��Ʈ�ѷ� �ʱ�ȭ
	if (nullptr != m_pCharacter)
		UnPossess();

	//Ÿ�� �缳��
	m_pCharacter = pTarget;
	Get_TargetInfo(m_pCharacter);

	m_pCharacter->Set_Controller(this);

	m_iTargetAniSetCount = m_pCharacter->Get_Mesh()->Get_MaxAniSet();

}

bool CAIController::UnPossess()
{
	if (m_pCharacter != nullptr)
	{	// Ÿ�� ��Ʈ�ѷ� �ʱ�ȭ
		m_pCharacter->Set_Controller(nullptr);
	}
	// Ÿ�� �ʱ�ȭ
	m_pCharacter = nullptr;
	m_pCharacter_Info = nullptr;

	return true;
}
//
//EStatus CAIController::Attack_Forward_Dash()
//{
//	if (nullptr == m_pTarget)
//		return EStatus::Failure;
//
//	m_pTarget->Get_Transform();
//
//	return EStatus::Success;
//}
//
//EStatus CAIController::Move_Forward()
//{
//	CTransform* pTargetTrnasform = m_pTarget->Get_Transform();
//	
//	if(nullptr == pTargetTrnasform)
//		return EStatus::Failure;
//
//	pTargetTrnasform->Go_Straight(
//		m_pTarget->Get_Info().m_fSpeed,
//		m_pInput->Get_TimeDelta());
//	
//	return EStatus::Success;
//
//	//return EStatus::Failure;
//}
//
//EStatus CAIController::Move_Right()
//{
//	
//	return EStatus::Success;
//}

#ifndef _FINAL
void CAIController::Debug_Render()
{
	CDebug::GetInstance()->Add_DebugText("CAIController");
	//CDebug::GetInstance()->Add_DebugText("CAIController");
	CDebug::GetInstance()->Add_DebugText("ActPlan(%d): %s", m_iActPlanCount,m_pCharacter_Info->m_CurAction_Act.name().c_str());
	CDebug::GetInstance()->Add_DebugText("AniPlan(%d): %s", m_iAniPlanCount,m_pCharacter_Info->m_CurAction_Ani.name().c_str());

	int iActionSize = m_pCharacter_Info->m_vecAction_Act.size();
	CDebug::GetInstance()->Add_DebugText(" --- Action(Act) Count :%d ---", iActionSize);
	for (auto pAction : m_pCharacter_Info->m_vecAction_Act)
	{
		CDebug::GetInstance()->Add_DebugText("----- Name: %s -----", pAction.name().c_str());
		CDebug::GetInstance()->Add_DebugText("Cost: %d", pAction.cost());
		CDebug::GetInstance()->Add_DebugText("- Precondition -");
		byte* pPreconditionArr = pAction.Get_Precondition_Arr_ForMapTool();
		for (int i = 0; i < AI_ARRAY_SIZE_MAX; ++i)
		{
			if (pPreconditionArr[i] != 2)
			{
				if((bool)pPreconditionArr[i] == true)
					CDebug::GetInstance()->Add_DebugText(L"%s : true", CDebug::GetInstance()->Get_WorldStateName(i));
				else
					CDebug::GetInstance()->Add_DebugText(L"%s : false", CDebug::GetInstance()->Get_WorldStateName(i));
				//CDebug::GetInstance()->Add_DebugText((bool)pPreconditionArr[i],"bool");
			}
		}
		CDebug::GetInstance()->Add_DebugText("- Effect -");
		byte* pEffectArr = pAction.Get_Effect_Arr_ForMapTool();
		for (int i = 0; i < AI_ARRAY_SIZE_MAX; ++i)
		{
			if (pEffectArr[i] != 2)
			{
				if ((bool)pEffectArr[i] == true)
					CDebug::GetInstance()->Add_DebugText(L"%s : true", CDebug::GetInstance()->Get_WorldStateName(i));
				else
					CDebug::GetInstance()->Add_DebugText(L"%s : false", CDebug::GetInstance()->Get_WorldStateName(i));
			}
		}
	}

	int iAniSize = m_pCharacter_Info->m_vecAction_Ani.size();
	CDebug::GetInstance()->Add_DebugText(" --- Action(Ani) Count :%d ---", iAniSize);
	for (auto pAction : m_pCharacter_Info->m_vecAction_Ani)
	{
		CDebug::GetInstance()->Add_DebugText("----- Name: %s -----", pAction.name().c_str());
		CDebug::GetInstance()->Add_DebugText("Cost: %d", pAction.cost());
		CDebug::GetInstance()->Add_DebugText("- Precondition -");
		byte* pPreconditionArr = pAction.Get_Precondition_Arr_ForMapTool();
		for (int i = 0; i < AI_ARRAY_SIZE_MAX; ++i)
		{
			if (pPreconditionArr[i] != 2)
			{
				if ((bool)pPreconditionArr[i] == true)
					CDebug::GetInstance()->Add_DebugText(L"%s : true", CDebug::GetInstance()->Get_WorldStateName(i));
				else
					CDebug::GetInstance()->Add_DebugText(L"%s : false", CDebug::GetInstance()->Get_WorldStateName(i));
			}
		}
		CDebug::GetInstance()->Add_DebugText("- Effect -");
		byte* pEffectArr = pAction.Get_Effect_Arr_ForMapTool();
		for (int i = 0; i < AI_ARRAY_SIZE_MAX; ++i)
		{
			if (pEffectArr[i] != 2)
			{
				if ((bool)pEffectArr[i] == true)
					CDebug::GetInstance()->Add_DebugText(L"%s : true", CDebug::GetInstance()->Get_WorldStateName(i));
				else
					CDebug::GetInstance()->Add_DebugText(L"%s : false", CDebug::GetInstance()->Get_WorldStateName(i));
			}
		}
	}
}
#endif