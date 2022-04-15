#pragma once

#include "Defines.h"
#include "Controller.h"


class CPlayerController : public CController
{
public:
	enum BOOLTABLETYPE
	{
		InputKey_Action,	// 액션( 이동, 공격 등의 ) 키를 눌렀다..
		InputKey_Attack,	// 공격 키를 눌렀다..
		InputKey_Kick,		// 킥 키를 눌렀다..
		InputKey_SideStep,	// 회피(구르기) 키를 눌렀다..
		InputKey_Guard,		// 가드 키를 눌렀다..
		IsAttackState,		// 공격 중..
		IsKickState,		// 킥 중..
		IsSideStepState,	// 회피 중..
		IsApproachTarget,	// 타겟에게 접근 중( 공격 중 먼 타겟에게 )..
		IsGuardState,		// 가드(카운터 기본 상태) 중이냐..
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
	CController*	m_pTargetExController = nullptr; // 타겟의 이전 AI 컨트롤러 보관


public: 
	virtual void Possess(CCharacter *pTarget) final; // 빙의할 캐릭터를 설정 해주는 함수
	virtual bool UnPossess() final; // 캐릭터 설정 해제
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

	// ---- 디버그 용 ----
private:
	float			fPlanTime = 0.f; // 플랜 함수 수행시간

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

	_float					m_fDestinationAngle = 0.f;	// 도착 앵글..
	_float					m_fMoveTimeSpeed = 4.f;
	_float					m_fAccMoveTime = 0.f;
	_int					m_iPrevAttInputKey = 0;	// 이전 공격에 입력한 방향 키..
	_ulong					m_dwBool_BitTable = 0;	// Bool 여러 개를 보관할 변수..


	int						m_iCurAniNum = 0;
	int						m_iCurAniEventSize = 0;
	int						m_iCurAniEventNum = 0;
	ANI_EVENT*				m_curAniEvent = nullptr;

private:
	_bool	IsCanChangeAttackTarget( const _uint& _iInputKey );	// 공격할 타겟을 바꿀 수 있는지..
private:
	void Set_Bool_BitTable( const BOOLTABLETYPE& _InputKeyType, const _bool& _bIsInput );
private:
	_bool Get_Bool_BitTable( const BOOLTABLETYPE& _InputKeyType );
private:
	void Update_CharacterAngle( void );		// 플레이어의 앵글 값을 계속 갱신..
	void Update_TargetDistance( const _float& _fTimeDelta );	// 플레이어와 타겟 간의 거리 갱신..
	_float Compute_DirToInputKey( const _uint& _iInputKey );	// Input Key 를 가지고 카메라 기준 앵글을 구함..
	_float Compute_CamAngleToCharAngle( _float _fAngle );		// 카메라 기준 앵글을 플레이어 Look 기준 앵글로 바꿈..
	void Compute_DirWorldState( void );		// 현재 구해진 앵글을 DIR 방향에 관한 WorldState 를 결정한다..
	void Update_AttackTargetData( CCharacter** _pOut, const _uint& _iInputKey, const _int& _iSceneNumber, const _tchar* _pEnemyTargetTag );	// 공격할 타겟 정보 갱신..

	void Modify_NearEnemyRangeData( CCharacter* _pNearEnemy ); // 근처 적 정보 갱신..


	// ---- 디버그 용 ----
#ifndef _FINAL
public:
	void Debug_Render();
#endif

	virtual _ulong Free(void);
};
