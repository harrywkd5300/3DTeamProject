#pragma once

#include "Engine_Defines.h"
#include "Component.h"
#include "src/AI.h"
#include "AI\GOAP_AI_Manager.h"

//namespace BrainTree
//{
//	class BehaviorTree;
//}


BEGIN(Engine)

class CTransform;
class CCharacter;

class DLL_EXPORT CController abstract : public CComponent
{
public:
	const int SIGNAL_SUCCESS_MAX_COUNT = 100;
protected:
	explicit CController(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CController(const CController& rhs);
	virtual ~CController() = default;
protected:
	CCharacter*	m_pCharacter = nullptr;	// 컨트롤러가 빙의한 캐릭터
	CHARINFO*	m_pCharacter_Info = nullptr;// 컨트롤러가 빙의한 캐릭터 정보

	CCharacter*	m_pTarget = nullptr;	// 빙의한 캐릭터가 설정한 타겟

	CWorldState m_GoalState_Signal;		// 시그널 골 스테이트
	CWorldState	m_GoalState_Animation;	// 애니메이션 변경 골 스테이트
	CWorldState	m_GoalState_Idle;		// 행동 변경 골 스테이트(IDLE)
	CWorldState	m_GoalState_Walk;		// 행동 변경 골 스테이트(WALK)
	CWorldState	m_GoalState_Attack;		// 행동 변경 골 스테이트(공격)
	CWorldState	m_GoalState_SideStep;	// 행동 변경 골 스테이트(회피)
	CWorldState	m_GoalState_Hit;		// 행동 변경 골 스테이트(히트)
	CWorldState	m_GoalState_Run;		// 행동 변경 골 스테이트(추격/도주)
	CWorldState	m_GoalState_Death;		// 행동 변경 골 스테이트(추격/도주)
	CWorldState	m_GoalState_Interaction;		// 행동 변경 골 스테이트(상호작용)
	CWorldState	m_GoalState_Guard;		// 행동 변경 골 스테이트(가드(카운터))

	int	m_iTargetAniSetCount = 0; // 빙의한 캐릭터 애니메이션 셋 개수
	//_float m_fTimeDelta = 0.f	// 해당 프레임 속도
protected:
	HRESULT Ready_GoalState(void);
public:
	HRESULT Ready_Controller(void);

	HRESULT Ready_Behavior_Tree();
	virtual void Update_Component(const _float& fTimeDelta)PURE;
	virtual void LastUpdate_Component(const _float& fTimeDelta)PURE;
public:
	virtual void Possess(CCharacter *pTarget); // 빙의할 캐릭터를 설정 해주는 함수
	virtual bool UnPossess(); // 캐릭터 설정 해제
	CCharacter* Get_Character() { return m_pCharacter; }
public:
	_float Get_Angle(const _matrix & matDestTargetWorld, float offsetAngle);
	_float Get_Angle(const _vec3& vecDestTargetPosition, float offsetAngle = 0.f);
	void Set_Angle(float offsetAngle);
	void Set_Angle(const _matrix& matTargetWorld, float offsetAngle = 0.f);
	void Set_Angle_FORCED(const _matrix& matTargetWorld, float offsetAngle = 0.f);
	void Set_Target(CCharacter* pTarget);

	//virtual EStatus Attack_Forward_Dash() { return EStatus::Success; }
	//virtual EStatus Move_Forward() { return EStatus::Success; }
	//virtual EStatus Move_Right() { return EStatus::Success; }
	//bool Find_EnemyTarget() { return true; }
	//bool Attack_Target() { return true; }

	//CTransform* m_pTargetTransform = nullptr; // 컨트롤러가 소유한 타겟 트랜스폼
protected:
	void Update_CharacterAngle();
	void Update_CharacterTarget(CCharacter* pNearEnemy, CCharacter* pNearTeam, CCharacter* pPlayer, CCharacter* pAttackedEnemy);
	// --------- Debug -----------
public:
	//std::shared_ptr<BrainTree::BehaviorTree> m_Tree; // 나중에 AI컨트롤러 옮겨
	BrainTree::BehaviorTree m_Tree; // 나중에 AI컨트롤러 옮겨
	int		m_iActPlanCount = 0;
	int		m_iAniPlanCount = 0;

	int						m_iCurAniEventSize = 0;
	int						m_iCurAniEventNum = 0;
	ANI_EVENT*				m_curAniEvent = nullptr;
	// --------- Debug -----------
public:
	void Get_TargetInfo(CCharacter* pTarget); // 캐릭터 정보 반환

public:
#ifndef _FINAL
	void Debug_Render();
#endif
public:
	virtual CComponent* Clone(void) PURE;
	virtual _ulong Free(void);

};

END