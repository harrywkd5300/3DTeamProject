#pragma once

#include "Defines.h"
#include "Controller.h"


class CPlayerController : public CController
{
public:
	enum BOOLTABLETYPE
	{
		InputKey_Action,	// �׼�( �̵�, ���� ���� ) Ű�� ������..
		InputKey_Attack,	// ���� Ű�� ������..
		InputKey_Kick,		// ű Ű�� ������..
		InputKey_SideStep,	// ȸ��(������) Ű�� ������..
		InputKey_Guard,		// ���� Ű�� ������..
		IsAttackState,		// ���� ��..
		IsKickState,		// ű ��..
		IsSideStepState,	// ȸ�� ��..
		IsApproachTarget,	// Ÿ�ٿ��� ���� ��( ���� �� �� Ÿ�ٿ��� )..
		IsGuardState,		// ����(ī���� �⺻ ����) ���̳�..
		InputKey_End
	};
protected:
	explicit CPlayerController(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CPlayerController(const CPlayerController& rhs);
	virtual ~CPlayerController() = default;
public:
	HRESULT Ready_Controller(void);
	virtual void Update_Component(const _float& fTimeDelta);
	virtual void LastUpdate_Component(const _float& fTimeDelta);

public:
	static CPlayerController* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone(void);
private:
	CInput_Device*	m_pInput = nullptr;
	CController*	m_pTargetExController = nullptr; // Ÿ���� ���� AI ��Ʈ�ѷ� ����


public: 
	virtual void Possess(CCharacter *pTarget) final; // ������ ĳ���͸� ���� ���ִ� �Լ�
	virtual bool UnPossess() final; // ĳ���� ���� ����
public:
	void Set_Angle_ForPlayer(const _matrix& matTargetWorld, float offsetAngle = 0.f);
	//virtual EStatus Attack_Forward_Dash();
	//virtual EStatus Move_Forward();
	//virtual EStatus Move_Right();


	//MOVE_FORWARD
	//MOVE_RIGHT
	//TURN
	//LOOKUP
	//JUMP
	//PARKOUR

	// ---- ����� �� ----
private:
	float			fPlanTime = 0.f; // �÷� �Լ� ����ð�

	//_float					m_fAngleSpeed = 360.f;
	//_float					m_fCurAngle = 0.f;
	//_float					m_fDestAngle = 0.f;
	//bool					m_bAngleCCW = false;

	//CWorldState				m_WorldState;


	//CAction					m_CurAction;
	//vector<CAction>			m_CurPlanVec;
	//vector<CAction>			m_vecAction;

	//CWorldState& pTargetWorldState;
	//vector<CAction>&  pTargetActionVector;

	_float					m_fDestinationAngle = 0.f;	// ���� �ޱ�..
	_float					m_fMoveTimeSpeed = 4.f;
	_float					m_fAccMoveTime = 0.f;
	_int					m_iPrevAttInputKey = 0;	// ���� ���ݿ� �Է��� ���� Ű..
	_ulong					m_dwBool_BitTable = 0;	// Bool ���� ���� ������ ����..


	int						m_iCurAniNum = 0;
	int						m_iCurAniEventSize = 0;
	int						m_iCurAniEventNum = 0;
	ANI_EVENT*				m_curAniEvent = nullptr;

private:
	_bool	IsCanChangeAttackTarget( const _uint& _iInputKey );	// ������ Ÿ���� �ٲ� �� �ִ���..
private:
	void Set_Bool_BitTable( const BOOLTABLETYPE& _InputKeyType, const _bool& _bIsInput );
private:
	_bool Get_Bool_BitTable( const BOOLTABLETYPE& _InputKeyType );
private:
	void Update_CharacterAngle( void );		// �÷��̾��� �ޱ� ���� ��� ����..
	void Update_TargetDistance( const _float& _fTimeDelta );	// �÷��̾�� Ÿ�� ���� �Ÿ� ����..
	_float Compute_DirToInputKey( const _uint& _iInputKey );	// Input Key �� ������ ī�޶� ���� �ޱ��� ����..
	_float Compute_CamAngleToCharAngle( _float _fAngle );		// ī�޶� ���� �ޱ��� �÷��̾� Look ���� �ޱ۷� �ٲ�..
	void Compute_DirWorldState( void );		// ���� ������ �ޱ��� DIR ���⿡ ���� WorldState �� �����Ѵ�..
	void Update_AttackTargetData( CCharacter** _pOut, const _uint& _iInputKey, const _int& _iSceneNumber, const _tchar* _pEnemyTargetTag );	// ������ Ÿ�� ���� ����..

	void Modify_NearEnemyRangeData( CCharacter* _pNearEnemy ); // ��ó �� ���� ����..


	// ---- ����� �� ----
#ifndef _FINAL
public:
	void Debug_Render();
#endif

	virtual _ulong Free(void);
};
