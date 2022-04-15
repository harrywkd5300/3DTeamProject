#include "..\Headers\Controller.h"
#include "Component_Manager.h"
#include "Character.h"
#include "Camera.h"

USING(Engine)

CController::CController(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev)
{
#ifndef _FINAL
	Tag(L"CController", std::bind(&CController::Debug_Render, this));
#endif

}

CController::CController(const CController & rhs)
	: CComponent(rhs),
	m_GoalState_Animation(rhs.m_GoalState_Animation),
	m_GoalState_Idle(rhs.m_GoalState_Idle),
	m_GoalState_Attack(rhs.m_GoalState_Attack),
	m_GoalState_SideStep(rhs.m_GoalState_SideStep),
	m_GoalState_Hit(rhs.m_GoalState_Hit),
	m_GoalState_Run(rhs.m_GoalState_Run),
	m_GoalState_Walk(rhs.m_GoalState_Walk),
	m_GoalState_Death(rhs.m_GoalState_Death),
	m_GoalState_Guard(rhs.m_GoalState_Guard)
	
{
#ifndef _FINAL
	Tag(L"CController", std::bind(&CController::Debug_Render, this));
#endif
}

HRESULT CController::Ready_Controller(void)
{

	return NOERROR;
}

HRESULT CController::Ready_GoalState(void)
{

	// 시그널 처리 골
	m_GoalState_Signal.setVariable(AI_SIGNAL_SUCCESS,true);
	m_GoalState_Signal.priority_ = 1000; // 우선순위

	// 애니메이션 변경 골
	m_GoalState_Animation.setVariable(SIGNAL_CHANGE_ANIMATION, false);
	m_GoalState_Animation.priority_ = 1000; // 우선순위

											// 행동 변경 골(히트)
	m_GoalState_Hit.setVariable(ACT_HIT, true);
	m_GoalState_Hit.priority_ = 1000; // 우선순위

									  // 행동 변경 골(회피)
	m_GoalState_SideStep.setVariable(ACT_SIDESTEP, true);
	m_GoalState_SideStep.priority_ = 1000; // 우선순위

										   // 행동 변경 골(공격)
	m_GoalState_Attack.setVariable(ACT_ATTACK, true);
	m_GoalState_Attack.priority_ = 1000; // 우선순위

										 // 행동 변경 골(IDLE)
	m_GoalState_Idle.setVariable(ACT_IDLE, true);
	m_GoalState_Idle.priority_ = 1000; // 우선순위=

									   // 행동 변경 골(WALK)
	m_GoalState_Walk.setVariable(ACT_WALK, true);
	m_GoalState_Walk.priority_ = 1000; // 우선순위

									   // 행동 변경 골(RUN)
	m_GoalState_Run.setVariable(ACT_RUN, true);
	m_GoalState_Run.priority_ = 1000; // 우선순위

	m_GoalState_Interaction.setVariable(ACT_INTERACTION, true);
	m_GoalState_Interaction.priority_ = 1000; // 우선순위

	m_GoalState_Death.setVariable(ACT_DEATH, true);
	m_GoalState_Death.priority_ = 1000; // 우선순위

	m_GoalState_Guard.setVariable( ACT_GUARD, true );
	m_GoalState_Guard.priority_ = 1000;
	

	return NOERROR;
}

HRESULT CController::Ready_Behavior_Tree()
{
	//BrainTree::BehaviorTree tree; 

	// each tree has one blackboard each, which the leafs can use
	auto &blackboard = m_Tree.GetBlackboard();

	//create a sequence
	auto attackEnemySequence = std::make_shared<BrainTree::Sequence>();
	auto Action_Wait = std::make_shared<WaitNode>(blackboard);
	//auto targetNearestEnemyNode =new WaitNode(blackboard);
	//auto moveToEnemyNode = std::make_shared<MoveToEnemyNode>(blackboard);
	//auto attackEnemyNode = std::make_shared<AttackEnemyNode>(blackboard);
	attackEnemySequence->add_child(Action_Wait);
	//attackEnemySequence->AddChild(moveToEnemyNode);
	//attackEnemySequence->AddChild(attackEnemyNode);

	// ...

	// create a selector
	auto selector = std::make_shared<BrainTree::Selector>();
	selector->add_child(attackEnemySequence);
	selector->add_child(Action_Wait);

	// set the root of the tree
	m_Tree.SetRoot(selector);

	//// inside game loop
	//tree.update();

	return NOERROR;
}

void CController::Possess(CCharacter * pTarget)
{
	if (nullptr == pTarget)
		return;

	if (nullptr != m_pCharacter)
		UnPossess();
	

	m_pCharacter = pTarget;
	Get_TargetInfo(m_pCharacter);

	// 타겟이 가지고 있는 컨트롤러가 this가 아니라면 세팅
	if(m_pCharacter->Get_Controller() != this)
		m_pCharacter->Set_Controller(this);

	m_iTargetAniSetCount = m_pCharacter->Get_Mesh()->Get_MaxAniSet();

	//m_pTarget->AddRef();
}

bool CController::UnPossess()
{
	//Safe_Release(m_pTarget);
	if (m_pCharacter != nullptr)
	{
		m_pCharacter->Set_Controller(nullptr);
	}

	m_pCharacter = nullptr;
	m_pCharacter_Info = nullptr;

	return true;
}

_float CController::Get_Angle(const _matrix & matDestTargetWorld, float offsetAngle)
{
	_vec3 vDestTargetLook = *(_vec3*)matDestTargetWorld.m[2];
	vDestTargetLook.y = 0.f;
	D3DXVec3Normalize(&vDestTargetLook, &vDestTargetLook);
	
	_vec3 vMyTargetLook = *m_pCharacter->Get_Transform()->Get_WorldLookVec();
	vMyTargetLook.y = 0.f;
	D3DXVec3Normalize(&vMyTargetLook, &vMyTargetLook);
	
	float Angle = D3DXToDegree(acosf(D3DXVec3Dot(&vDestTargetLook, &vMyTargetLook )));
	if (isnan(Angle))
		Angle = 0.f;
	
	_vec3 CheckDirVec;
	D3DXVec3Cross(&CheckDirVec, &vDestTargetLook, &vMyTargetLook);

	CDebug::GetInstance()->Add_DebugText( 800.f, 800.f, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), "Angle : %f", Angle );
	
	if (CheckDirVec.y >= 0.f)
	{
		Angle = 360.f - Angle;
	}
	else
	{
		int a = 0;
	}

	//if (vMyTargetLook.x <= 0.f)
	//{
	//	Angle = 360.f - Angle;
	//}

	Angle += offsetAngle;
	//m_pTargetInfo->m_fDestAngle = offsetAngle;

	if (Angle > 360.f)
	{
		Angle -= 360.f;
	}
	if (Angle < 0.f)
	{
		Angle += 360.f;
	}


	return Angle;
}

_float CController::Get_Angle(const _vec3 & vecDestTargetPosition, float offsetAngle)
{
	_vec3 vDestTargetLook = vecDestTargetPosition - *m_pCharacter->Get_Transform()->Get_Infomation(CTransform::INFO_POSITION);
	vDestTargetLook.y = 0.f;
	D3DXVec3Normalize(&vDestTargetLook, &vDestTargetLook);

	_vec3 vMyTargetLook = *m_pCharacter->Get_Transform()->Get_WorldLookVec();
	vMyTargetLook.y = 0.f;
	D3DXVec3Normalize(&vMyTargetLook, &vMyTargetLook);


	float Angle = D3DXToDegree(acosf(D3DXVec3Dot(&vDestTargetLook, &vMyTargetLook)));
	if (isnan(Angle))
		Angle = 0.f;

	_vec3 CheckDirVec;
	D3DXVec3Cross(&CheckDirVec, &vDestTargetLook, &vMyTargetLook);

	if (CheckDirVec.y >= 0.f)
	{
		Angle = 360.f - Angle;
	}
	else
	{
		int a = 0;
	}

	//if (vMyTargetLook.x <= 0.f)
	//{
	//	Angle = 360.f - Angle;
	//}

	Angle += offsetAngle;
	//m_pTargetInfo->m_fDestAngle = offsetAngle;

	if (Angle > 360.f)
	{
		Angle -= 360.f;
	}
	if (Angle < 0.f)
	{
		Angle += 360.f;
	}


	return Angle;
}

void CController::Set_Angle(float offsetAngle)
{
	m_pCharacter_Info->m_fDestAngle = offsetAngle;

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
		if ((360.f - fAngleGap) * m_pTimer->Get_TimeDelta() * m_pCharacter_Info->m_fAngleSpeed <= SMALL_NUMBER)//* fabs(m_pTargetInfo->m_fAngleGap_CameraToPlayer))
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

void CController::Set_Angle(const _matrix & matTargetWorld, float offsetAngle)
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

	//_float fAngleGap = m_pCharacter_Info->m_fDestAngle - m_pCharacter->Get_Transform()->Get_Infomation(CTransform::INFO_ANGLE)->y;


	//if (fAngleGap <= 0.f)
	//{
	//	fAngleGap += 360.f;
	//}

	//if (fAngleGap >= 180.f)
	//{
	//	if ((360.f - fAngleGap) * m_pTimer->Get_TimeDelta() * m_pCharacter_Info->m_fAngleSpeed <=  SMALL_NUMBER )//* fabs(m_pTargetInfo->m_fAngleGap_CameraToPlayer))
	//	{
	//		m_pCharacter_Info->m_WorldState.setVariable(SIGNAL_CHANGE_ANGLE, false);
	//		return;
	//	}
	//	m_pCharacter->Get_Transform()->Add_Infomation(CTransform::INFO_ANGLE,
	//		&_vec3(0.f,
	//		-(360.f - fAngleGap) * m_pTimer->Get_TimeDelta() * m_pCharacter_Info->m_fAngleSpeed,
	//			0.f));
	//}
	//else
	//{
	//	if ((fAngleGap)* m_pTimer->Get_TimeDelta() * m_pCharacter_Info->m_fAngleSpeed <= SMALL_NUMBER)//* fabs(m_pTargetInfo->m_fAngleGap_CameraToPlayer))
	//	{
	//		m_pCharacter_Info->m_WorldState.setVariable(SIGNAL_CHANGE_ANGLE, false);
	//		return;
	//	}
	//	m_pCharacter->Get_Transform()->Add_Infomation(CTransform::INFO_ANGLE,
	//		&_vec3(0.f,
	//		(fAngleGap)* m_pTimer->Get_TimeDelta() * m_pCharacter_Info->m_fAngleSpeed,
	//			0.f));
	//}

	//m_pCharacter_Info->m_WorldState.setVariable(SIGNAL_CHANGE_ANGLE, true);
}

void CController::Set_Angle_FORCED(const _matrix & matTargetWorld, float offsetAngle)
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

	if (m_pCharacter_Info->m_fDestAngle > 360.f)
	{
		m_pCharacter_Info->m_fDestAngle -= 360.f;
	}
	if (m_pCharacter_Info->m_fDestAngle < 0.f)
	{
		m_pCharacter_Info->m_fDestAngle += 360.f;
	}


	m_pCharacter->Get_Transform()->Rotation_Y(m_pCharacter_Info->m_fDestAngle);

	m_pCharacter_Info->m_WorldState.setVariable(SIGNAL_CHANGE_ANGLE_FORCED, false);
}

void CController::Set_Target(CCharacter * pTarget)
{
	m_pTarget = pTarget;
	m_pCharacter_Info->m_WorldState.setVariable(SIGNAL_CHANGE_TARGET_EXTERNAL,true);
}


void CController::Update_CharacterAngle()
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
				m_pCharacter_Info->m_fDestAngle = Get_Angle(*m_pTarget->Get_Transform()->Get_Infomation(CTransform::INFO_POSITION));

				// 플레이어 방향 앵글값 월드스테이트에 입력
				if (m_pCharacter_Info->m_fDestAngle < 22.5f)
				{
					m_pCharacter_Info->m_WorldState.setVariable(DEST_DIR_0, true);
				}
				else if (m_pCharacter_Info->m_fDestAngle < 67.5f)
				{
					m_pCharacter_Info->m_WorldState.setVariable(DEST_DIR_45, true);
				}
				else if (m_pCharacter_Info->m_fDestAngle < 112.5f)
				{
					m_pCharacter_Info->m_WorldState.setVariable(DEST_DIR_90, true);
				}
				else if (m_pCharacter_Info->m_fDestAngle < 157.5f)
				{
					m_pCharacter_Info->m_WorldState.setVariable(DEST_DIR_135, true);
				}
				else if (m_pCharacter_Info->m_fDestAngle < 202.5)
				{
					m_pCharacter_Info->m_WorldState.setVariable(DEST_DIR_180, true);
				}
				else if (m_pCharacter_Info->m_fDestAngle < 247.5)
				{
					m_pCharacter_Info->m_WorldState.setVariable(DEST_DIR_225, true);
				}
				else if (m_pCharacter_Info->m_fDestAngle < 292.5)
				{
					m_pCharacter_Info->m_WorldState.setVariable(DEST_DIR_270, true);
				}
				else if (m_pCharacter_Info->m_fDestAngle < 337.5)
				{
					m_pCharacter_Info->m_WorldState.setVariable(DEST_DIR_315, true);
				}
				else if (m_pCharacter_Info->m_fDestAngle <= 360.f)
				{
					m_pCharacter_Info->m_WorldState.setVariable(DEST_DIR_0, true);
				}
				Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), m_pCharacter_Info->m_fDestAngle);
				//CDebug::GetInstance()->Add_ConsoleDebugText("m_pCharacter_Info->m_fDestAngle :%f", m_pCharacter_Info->m_fDestAngle);
			}
			else
			{
				if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_0))
					Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 0.f);
				else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_45))
					Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 45.f);
				else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_90))
					Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 90.f);
				else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_135))
					Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 135.f);
				else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_180))
					Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 180.f);
				else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_225))
					Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 225.f);
				else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_270))
					Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 270.f);
				else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_315))
					Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 315.f);
			}
		}
		else
		{
			if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_0))
				Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 0.f);
			else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_45))
				Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 45.f);
			else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_90))
				Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 90.f);
			else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_135))
				Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 135.f);
			else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_180))
				Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 180.f);
			else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_225))
				Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 225.f);
			else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_270))
				Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 270.f);
			else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_315))
				Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 315.f);
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
				m_pCharacter_Info->m_fDestAngle = Get_Angle(*m_pTarget->Get_Transform()->Get_Infomation(CTransform::INFO_POSITION));

				// 플레이어 방향 앵글값 월드스테이트에 입력
				if (m_pCharacter_Info->m_fDestAngle < 22.5f)
				{
					m_pCharacter_Info->m_WorldState.setVariable(DEST_DIR_0, true);
				}
				else if (m_pCharacter_Info->m_fDestAngle < 67.5f)
				{
					m_pCharacter_Info->m_WorldState.setVariable(DEST_DIR_45, true);
				}
				else if (m_pCharacter_Info->m_fDestAngle < 112.5f)
				{
					m_pCharacter_Info->m_WorldState.setVariable(DEST_DIR_90, true);
				}
				else if (m_pCharacter_Info->m_fDestAngle < 157.5f)
				{
					m_pCharacter_Info->m_WorldState.setVariable(DEST_DIR_135, true);
				}
				else if (m_pCharacter_Info->m_fDestAngle < 202.5)
				{
					m_pCharacter_Info->m_WorldState.setVariable(DEST_DIR_180, true);
				}
				else if (m_pCharacter_Info->m_fDestAngle < 247.5)
				{
					m_pCharacter_Info->m_WorldState.setVariable(DEST_DIR_225, true);
				}
				else if (m_pCharacter_Info->m_fDestAngle < 292.5)
				{
					m_pCharacter_Info->m_WorldState.setVariable(DEST_DIR_270, true);
				}
				else if (m_pCharacter_Info->m_fDestAngle < 337.5)
				{
					m_pCharacter_Info->m_WorldState.setVariable(DEST_DIR_315, true);
				}
				else if (m_pCharacter_Info->m_fDestAngle <= 360.f)
				{
					m_pCharacter_Info->m_WorldState.setVariable(DEST_DIR_0, true);
				}
				Set_Angle_FORCED(m_pCharacter->Get_Transform()->Get_WorldMatrix(), m_pCharacter_Info->m_fDestAngle);
			}
			else
			{
				if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_0))
					Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 0.f);
				else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_45))
					Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 45.f);
				else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_90))
					Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 90.f);
				else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_135))
					Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 135.f);
				else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_180))
					Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 180.f);
				else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_225))
					Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 225.f);
				else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_270))
					Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 270.f);
				else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_315))
					Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 315.f);
			}

		}
		else
		{
			if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_0))
				Set_Angle_FORCED(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 0.f);
			else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_45))
				Set_Angle_FORCED(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 45.f);
			else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_90))
				Set_Angle_FORCED(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 90.f);
			else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_135))
				Set_Angle_FORCED(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 135.f);
			else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_180))
				Set_Angle_FORCED(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 180.f);
			else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_225))
				Set_Angle_FORCED(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 225.f);
			else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_270))
				Set_Angle_FORCED(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 270.f);
			else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_315))
				Set_Angle_FORCED(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 315.f);
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
				m_pCharacter_Info->m_fDestAngle = Get_Angle(*m_pTarget->Get_Transform()->Get_Infomation(CTransform::INFO_POSITION));

				// 플레이어 방향 앵글값 월드스테이트에 입력
				if (m_pCharacter_Info->m_fDestAngle < 22.5f)
				{
					m_pCharacter_Info->m_WorldState.setVariable(DEST_DIR_0, true);
				}
				else if (m_pCharacter_Info->m_fDestAngle < 67.5f)
				{
					m_pCharacter_Info->m_WorldState.setVariable(DEST_DIR_45, true);
				}
				else if (m_pCharacter_Info->m_fDestAngle < 112.5f)
				{
					m_pCharacter_Info->m_WorldState.setVariable(DEST_DIR_90, true);
				}
				else if (m_pCharacter_Info->m_fDestAngle < 157.5f)
				{
					m_pCharacter_Info->m_WorldState.setVariable(DEST_DIR_135, true);
				}
				else if (m_pCharacter_Info->m_fDestAngle < 202.5)
				{
					m_pCharacter_Info->m_WorldState.setVariable(DEST_DIR_180, true);
				}
				else if (m_pCharacter_Info->m_fDestAngle < 247.5)
				{
					m_pCharacter_Info->m_WorldState.setVariable(DEST_DIR_225, true);
				}
				else if (m_pCharacter_Info->m_fDestAngle < 292.5)
				{
					m_pCharacter_Info->m_WorldState.setVariable(DEST_DIR_270, true);
				}
				else if (m_pCharacter_Info->m_fDestAngle < 337.5)
				{
					m_pCharacter_Info->m_WorldState.setVariable(DEST_DIR_315, true);
				}
				else if (m_pCharacter_Info->m_fDestAngle <= 360.f)
				{
					m_pCharacter_Info->m_WorldState.setVariable(DEST_DIR_0, true);
				}
				Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), m_pCharacter_Info->m_fDestAngle);
				//CDebug::GetInstance()->Add_ConsoleDebugText("m_pCharacter_Info->m_fDestAngle :%f", m_pCharacter_Info->m_fDestAngle);
			}
			else
			{
				if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_0))
					Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 0.f);
				else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_45))
					Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 45.f);
				else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_90))
					Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 90.f);
				else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_135))
					Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 135.f);
				else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_180))
					Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 180.f);
				else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_225))
					Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 225.f);
				else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_270))
					Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 270.f);
				else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_315))
					Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 315.f);
			}
		}
		else
		{
			if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_0))
				Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 0.f);
			else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_45))
				Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 45.f);
			else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_90))
				Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 90.f);
			else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_135))
				Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 135.f);
			else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_180))
				Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 180.f);
			else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_225))
				Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 225.f);
			else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_270))
				Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 270.f);
			else if (m_pCharacter_Info->m_WorldState.getVariable(DEST_DIR_315))
				Set_Angle(m_pCharacter->Get_Transform()->Get_WorldMatrix(), 315.f);
		}
		m_pCharacter_Info->m_WorldState.setVariable(SIGNAL_CHANGE_ANGLE_ONCE, false);
	}
	
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

	//m_pCharacter_Info->m_WorldState.setVariable(SIGNAL_CHANGE_ANGLE, true);
}

void CController::Update_CharacterTarget(CCharacter* pNearEnemy, CCharacter* pNearTeam, CCharacter* pPlayer, CCharacter* pAttackedEnemy)
{
	if (m_pCharacter_Info->m_WorldState.getVariable(TARGET_EXTERNAL))
	{
		// 타겟과의 거리 초기화
		m_pCharacter_Info->m_WorldState.setVariable(CH_STATE_CLOSE_TARGET, CH_STATE_TOOFAR_TARGET, false);

		m_pCharacter_Info->m_WorldState.setVariable(TARGET_TYPE_START, TARGET_TYPE_END, false);
		m_pCharacter_Info->m_WorldState.setVariable(TARGET_EXTERNAL, true);
		m_pCharacter_Info->m_WorldState.setVariable(SIGNAL_CHANGE_TARGET_EXTERNAL, false);
	}
	else
	{
		if (m_pCharacter_Info->m_WorldState.getVariable(SIGNAL_CHANGE_TARGET_EXTERNAL))
		{
			// 타겟과의 거리 초기화
			m_pCharacter_Info->m_WorldState.setVariable(CH_STATE_CLOSE_TARGET, CH_STATE_TOOFAR_TARGET, false);

			m_pCharacter_Info->m_WorldState.setVariable(TARGET_TYPE_START, TARGET_TYPE_END, false);
			m_pCharacter_Info->m_WorldState.setVariable(TARGET_EXTERNAL, true);
			m_pCharacter_Info->m_WorldState.setVariable(SIGNAL_CHANGE_TARGET_EXTERNAL, false);
		}
		else if ( m_pCharacter_Info->m_WorldState.getVariable( SIGNAL_CHANGE_TARGET_NONE ) )
		{
			// 타겟과의 거리 초기화
			m_pCharacter_Info->m_WorldState.setVariable(CH_STATE_CLOSE_TARGET, CH_STATE_TOOFAR_TARGET, false);

			m_pCharacter_Info->m_WorldState.setVariable(TARGET_TYPE_START, TARGET_TYPE_END, false);
			m_pCharacter_Info->m_WorldState.setVariable(TARGET_NONE, true);
			m_pCharacter_Info->m_WorldState.setVariable(SIGNAL_CHANGE_TARGET_NONE, false);
			m_pTarget = nullptr;
		}
		else if (m_pCharacter_Info->m_WorldState.getVariable(SIGNAL_CHANGE_TARGET_HIT_ENEMY))
		{
			// 타겟과의 거리 초기화
			m_pCharacter_Info->m_WorldState.setVariable(CH_STATE_CLOSE_TARGET, CH_STATE_TOOFAR_TARGET, false);

			m_pCharacter_Info->m_WorldState.setVariable(TARGET_TYPE_START, TARGET_TYPE_END, false);
			m_pCharacter_Info->m_WorldState.setVariable(TARGET_HIT_ENEMY, true);
			m_pCharacter_Info->m_WorldState.setVariable(SIGNAL_CHANGE_TARGET_HIT_ENEMY, false);
			m_pTarget = pAttackedEnemy;
		}
		else if (m_pCharacter_Info->m_WorldState.getVariable(SIGNAL_CHANGE_TARGET_NEAR_ENEMY))
		{
			// 타겟과의 거리 초기화
			m_pCharacter_Info->m_WorldState.setVariable(CH_STATE_CLOSE_TARGET, CH_STATE_TOOFAR_TARGET, false);

			m_pCharacter_Info->m_WorldState.setVariable(TARGET_TYPE_START, TARGET_TYPE_END, false);
			m_pCharacter_Info->m_WorldState.setVariable(TARGET_NEAR_ENEMY, true);
			m_pCharacter_Info->m_WorldState.setVariable(SIGNAL_CHANGE_TARGET_NEAR_ENEMY, false);
			m_pTarget = pNearEnemy;
		}
		else if (m_pCharacter_Info->m_WorldState.getVariable(SIGNAL_CHANGE_TARGET_NEAR_TEAM))
		{
			// 타겟과의 거리 초기화
			m_pCharacter_Info->m_WorldState.setVariable(CH_STATE_CLOSE_TARGET, CH_STATE_TOOFAR_TARGET, false);

			m_pCharacter_Info->m_WorldState.setVariable(TARGET_TYPE_START, TARGET_TYPE_END, false);
			m_pCharacter_Info->m_WorldState.setVariable(TARGET_NEAR_TEAM, true);
			m_pCharacter_Info->m_WorldState.setVariable(SIGNAL_CHANGE_TARGET_NEAR_TEAM, false);
			m_pTarget = pNearTeam;
		}
		else if (m_pCharacter_Info->m_WorldState.getVariable(SIGNAL_CHANGE_TARGET_PLAYER))
		{
			// 타겟과의 거리 초기화
			m_pCharacter_Info->m_WorldState.setVariable(CH_STATE_CLOSE_TARGET, CH_STATE_TOOFAR_TARGET, false);

			m_pCharacter_Info->m_WorldState.setVariable(TARGET_TYPE_START, TARGET_TYPE_END, false);
			m_pCharacter_Info->m_WorldState.setVariable(TARGET_PLAYER, true);
			m_pCharacter_Info->m_WorldState.setVariable(SIGNAL_CHANGE_TARGET_PLAYER, false);
			m_pTarget = pPlayer;
		}
		else
		{
			return;
		}
	}

	if (m_pTarget != nullptr)
	{
		if (m_pTarget->Get_IsDead())
		{
			m_pTarget = nullptr;
			m_pCharacter_Info->m_WorldState.setVariable(TARGET_TYPE_START, TARGET_TYPE_END, false);
			m_pCharacter_Info->m_WorldState.setVariable(TARGET_NONE, true);
		}
	}

	if (m_pTarget != nullptr)
	{
		const CHARINFO& TargetCharInfo = m_pTarget->Get_Info();

		// ---- 타겟 상태 갱신 시작 ----
		if (TargetCharInfo.m_WorldState.getVariable(CH_STATE_ATTACK))
			m_pCharacter_Info->m_WorldState.setVariable(TARGET_STATE_ATTACK, true);
		else
			m_pCharacter_Info->m_WorldState.setVariable(TARGET_STATE_ATTACK, false);
		// ---- 타겟 상태 갱신 끝 ----


		float fTargetToMeDist = D3DXVec3Length(&(*m_pTarget->Get_Transform()->Get_Infomation(CTransform::INFO_POSITION)
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

}
void CController::Get_TargetInfo(CCharacter * pTarget)
{
	if (pTarget != nullptr) 
		m_pCharacter_Info = &pTarget->m_tInfo;
}		
#ifndef _FINAL
void CController::Debug_Render()
{
	CDebug::GetInstance()->Add_DebugText("CController");
	CDebug::GetInstance()->Add_DebugText(m_iTargetAniSetCount,"m_iTargetAniSetCount");
}
#endif

_ulong CController::Free(void)
{
	//Safe_Release(m_pTarget);
	return CComponent::Free();
}
