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
	CCharacter*	m_pCharacter = nullptr;	// ��Ʈ�ѷ��� ������ ĳ����
	CHARINFO*	m_pCharacter_Info = nullptr;// ��Ʈ�ѷ��� ������ ĳ���� ����

	CCharacter*	m_pTarget = nullptr;	// ������ ĳ���Ͱ� ������ Ÿ��

	CWorldState m_GoalState_Signal;		// �ñ׳� �� ������Ʈ
	CWorldState	m_GoalState_Animation;	// �ִϸ��̼� ���� �� ������Ʈ
	CWorldState	m_GoalState_Idle;		// �ൿ ���� �� ������Ʈ(IDLE)
	CWorldState	m_GoalState_Walk;		// �ൿ ���� �� ������Ʈ(WALK)
	CWorldState	m_GoalState_Attack;		// �ൿ ���� �� ������Ʈ(����)
	CWorldState	m_GoalState_SideStep;	// �ൿ ���� �� ������Ʈ(ȸ��)
	CWorldState	m_GoalState_Hit;		// �ൿ ���� �� ������Ʈ(��Ʈ)
	CWorldState	m_GoalState_Run;		// �ൿ ���� �� ������Ʈ(�߰�/����)
	CWorldState	m_GoalState_Death;		// �ൿ ���� �� ������Ʈ(�߰�/����)
	CWorldState	m_GoalState_Interaction;		// �ൿ ���� �� ������Ʈ(��ȣ�ۿ�)
	CWorldState	m_GoalState_Guard;		// �ൿ ���� �� ������Ʈ(����(ī����))

	int	m_iTargetAniSetCount = 0; // ������ ĳ���� �ִϸ��̼� �� ����
	//_float m_fTimeDelta = 0.f	// �ش� ������ �ӵ�
protected:
	HRESULT Ready_GoalState(void);
public:
	HRESULT Ready_Controller(void);

	HRESULT Ready_Behavior_Tree();
	virtual void Update_Component(const _float& fTimeDelta)PURE;
	virtual void LastUpdate_Component(const _float& fTimeDelta)PURE;
public:
	virtual void Possess(CCharacter *pTarget); // ������ ĳ���͸� ���� ���ִ� �Լ�
	virtual bool UnPossess(); // ĳ���� ���� ����
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

	//CTransform* m_pTargetTransform = nullptr; // ��Ʈ�ѷ��� ������ Ÿ�� Ʈ������
protected:
	void Update_CharacterAngle();
	void Update_CharacterTarget(CCharacter* pNearEnemy, CCharacter* pNearTeam, CCharacter* pPlayer, CCharacter* pAttackedEnemy);
	// --------- Debug -----------
public:
	//std::shared_ptr<BrainTree::BehaviorTree> m_Tree; // ���߿� AI��Ʈ�ѷ� �Ű�
	BrainTree::BehaviorTree m_Tree; // ���߿� AI��Ʈ�ѷ� �Ű�
	int		m_iActPlanCount = 0;
	int		m_iAniPlanCount = 0;

	int						m_iCurAniEventSize = 0;
	int						m_iCurAniEventNum = 0;
	ANI_EVENT*				m_curAniEvent = nullptr;
	// --------- Debug -----------
public:
	void Get_TargetInfo(CCharacter* pTarget); // ĳ���� ���� ��ȯ

public:
#ifndef _FINAL
	void Debug_Render();
#endif
public:
	virtual CComponent* Clone(void) PURE;
	virtual _ulong Free(void);

};

END