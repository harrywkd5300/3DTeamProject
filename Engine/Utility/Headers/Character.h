// Jangwon
#pragma once

#include "Engine_Defines.h"
#include "gameObject.h"
#include "AI/GOAP_AI_Manager.h"
#include "Controller.h"

BEGIN(Engine)

typedef struct tag_CharacterInfo
{
	enum TYPE{ CHAR_PLAYER ,CHAR_MONSTER, CHAR_END}; //아군, 적군
	enum CHARACTERSIZEKIND { CharacterSize_Small, CharacterSize_Mid, CharacterSize_Big, CharacterSize_End };

	_int					m_iCurHp = 100;		// 현재체력
	_int					m_iMaxHp = 100;		// 최대 체력
	_float					m_fHpAcc = 1.f;		// 현재체력/최대체력

	_float					m_fCurCp = 30.f;	// 현재 카운터 By.PSH
	_float					m_fMaxCp = 30.f;	// 최대 카운터
	_float					m_fCspCp = 10.f;	// 카운터 감소량
	_float					m_fIcmCp = 3.f;		// 카운터 초당 증가량
	_float					m_fCpAcc = 1.f;		// 현재카운터/최대카운터

	_float					m_fCurEp = 30;		// 현재 기력 By.PSH
	_float					m_fMaxEp = 30;		// 최대 기력
	_float					m_fEpAcc = 1.f;		// 현재기력/최대기력

	_int					m_iDefence = 0;		// 방어력


	_float					m_fAngle = 0.f;  // 현재 Y축회전값 // By.PSH

	_bool					m_bHit = false;	// 히트되었는지
	_vec3					m_MoveDelta = _vec3(0.f, 0.f, 0.f);	// 이동변위값
	_float					m_fSpeed = 0.f; //이동속도 값
	tag_CharacterInfo::TYPE	m_eCharType = CHAR_MONSTER; // 캐릭터 타입
	CHARACTERSIZEKIND		m_eCharSizeType = CharacterSize_End;	// 캐릭터 사이즈 타입( 소형, 중형, 대형 )..

	_vec3					m_vecAniMoveDelta = _vec3(0.f, 0.f, 0.f); // (루트모션)포지션변화값
	_vec3					m_vecAniAngleDelta = _vec3(0.f, 0.f, 0.f); // (루트모션)앵글변화값 

	_float					m_fAngleSpeed = 5.f;				// 앵글 스피드
	_float					m_fDestAngle = 0.f;					// 이동할 앵글
	_float					m_fAngleGap_CameraToPlayer = 0.f;	// 카메라-플레이어 룩벡터 앵글 갭

	_float					m_fIdleDelay = 8.f;			// 평화 모드로 넘어가기 까지 시간

														//CCharacter*				m_pNearEnemy = nullptr;		// 근처 적
	CCharacter*				m_pEnemy_Attacked = nullptr;	// 나를 공격한 적

	_uint					m_iMoveColliderGroupCount = 0;	// 충돌용 콜리더 그룹 번호..

	_float					m_fRange_FOV	= 75.f;	// 캐릭터의 시야 각..

	_float					m_fRange_Close	= 2.5f;
	_float					m_fRange_Near	= 10.f;
	_float					m_fRange_Mid	= 20.f;
	_float					m_fRange_Far	= 30.f;
	_float					m_fRange_TooFar = 40.f;
	_float					m_fRange_FoundEnemy = 20.f;

	//--------------------Monster State----------------------------------

	_int					m_iShield_Point = 0;
	_float					m_fEvade_Point = 0.f;

	_bool					m_bShield = false;
	_bool					m_bEvade = false;

	//------------------------------------------------------


	// --------- AI 사용 --------
	_tchar					m_StartAniName[32];			// 캐릭터 애니메이션 시작 스트링

	CWorldState				m_WorldState;				// 캐릭터 월드 스테이트

	vector<CAction>			m_vecAction_Act;			// 캐릭터 행동 변경 액션 벡터
	CAction					m_CurAction_Act;			// 캐릭터 현재 행동 변경 액션

	vector<CAction>			m_vecAction_Ani;			// 캐릭터 애니메이션 변경 액션 벡터
	CAction					m_CurAction_Ani;			// 캐릭터 현재 애니메이션 변경 액션

	int						m_StartAniNum = 0;			// 캐릭터 애니메이션 시작 넘버
	int						m_EndAniNum = 0;			// 캐릭터 애미네이션 종료 넘버

														//tag_CharacterInfo()
														//{};
														//tag_CharacterInfo(const tag_CharacterInfo& rhs)
														//{
														//	m_iCurHp = rhs.m_iCurHp;
														//	m_iMaxHp = rhs.m_iMaxHp;
														//	m_fHpAcc = rhs.m_fHpAcc;

														//	m_iCurCp = rhs.m_iCurCp;
														//	m_iMaxCp = rhs.m_iMaxCp;
														//	m_fCpAcc = rhs.m_fCpAcc;

														//	m_iCurEp = rhs.m_iCurEp;
														//	m_iMaxEp = rhs.m_iMaxEp;
														//	m_fEpAcc = rhs.m_fEpAcc;

														//	m_fAngle = rhs.m_fAngle;

														//	m_bHit = rhs.m_bHit;
														//	m_MoveDelta = rhs.m_MoveDelta;
														//	m_fSpeed = rhs.m_fSpeed;
														//	m_eCharType = rhs.m_eCharType;

														//	m_vecAniMoveDelta = rhs.m_vecAniMoveDelta;
														//	m_vecAniAngleDelta = rhs.m_vecAniAngleDelta;

														//	m_fAngleSpeed = rhs.m_fAngleSpeed;
														//	m_fDestAngle = rhs.m_fDestAngle;
														//	m_fAngleGap_CameraToPlayer = rhs.m_fAngleGap_CameraToPlayer;

														//	m_fIdleDelay = rhs.m_fIdleDelay;

														//	m_pEnemy_Attacked = rhs.m_pEnemy_Attacked;

														//	m_iMoveColliderGroupCount = rhs.m_iMoveColliderGroupCount;

														//	m_fRange_Close = rhs.m_fRange_Close;
														//	m_fRange_Near = rhs.m_fRange_Near;
														//	m_fRange_Mid = rhs.m_fRange_Mid;
														//	m_fRange_Far = rhs.m_fRange_Far;
														//	m_fRange_TooFar = rhs.m_fRange_TooFar;
														//	m_fRange_FoundEnemy = rhs.m_fRange_FoundEnemy;


														//	m_WorldState = rhs.m_WorldState;

														//	m_vecAction_Act = rhs.m_vecAction_Act;
														//	m_CurAction_Act = rhs.m_CurAction_Act;

														//	m_vecAction_Ani = rhs.m_vecAction_Ani;
														//	m_CurAction_Ani = rhs.m_CurAction_Ani;

														//	m_StartAniNum = rhs.m_StartAniNum;
														//	m_EndAniNum = rhs.m_EndAniNum;
														//};
														// --------- AI 사용 --------
}CHARINFO;


class CRenderer;
class CTransform;
class CPicking;
class CMaterial;
class CVIBuffer;
class CMesh_Dynamic;
class CCollider;
class CNavigation;
class CShader;
//class CController;
class CEquipment;
class CCamera;
class CCollider_Manager;

#ifdef CHARACTER_COLLIDERMODIFY
typedef struct tagColliderGroup
{
	const _tchar*			pGroupName = nullptr;	// 그룹 이름 ( 쓸지 안쓸지, 혹시 몰라서 )..
	const _tchar*			pLayerName = nullptr;	// 그룹이 속해 있는 레이어 이름( or 속할 예정인 레이어 이름 )..
	Engine::CCollider**		ppCollider = nullptr;	// 여러 개의 콜리더가 동적 배열의 형태로 담김..
	_uint					iColliderCount = 0;		// 콜리더 개수..

	tagColliderGroup( const _tchar* _pGroupName = nullptr, Engine::CCollider* _pCollider = nullptr )
		: pGroupName( _pGroupName )
		, pLayerName( nullptr )
	{
		if ( nullptr != _pCollider )
			Add_Collider( _pCollider );
	}

	void Add_Collider( Engine::CCollider* _pCollider = nullptr )
	{
		// 콜리더를 담을 공간을 늘리고 담겨 있는 콜리더를 담아놓음..
		Engine::CCollider**	ppTemp = new Engine::CCollider*[iColliderCount + 1];

		for ( _uint i = 0; i < iColliderCount; ++i )
		{
			ppTemp[i] = ppCollider[i];
		}

		ppTemp[iColliderCount] = _pCollider;

		// 원래 있던 공간은 삭제하고 담아 놓은 콜리더를 받아옴..
		Engine::Safe_Delete_Array( ppCollider );

		ppCollider = ppTemp;

		++iColliderCount;
	}

	void Release( void );

} COLLIDERGROUP;

typedef struct tagColliderGroupData
{
	COLLIDERGROUP*			pGroup = nullptr;
	_uint					iGroupCount = 0;		// 콜리더 개수..

	tagColliderGroupData()
		: pGroup( nullptr ), iGroupCount( 0 )
	{}

	void Add_ColliderGroup( const COLLIDERGROUP& _ColliderGroup )
	{
		// 콜리더 그룹을 담을 공간을 늘리고 담겨 있는 콜리더 그룹을 담아놓음..
		COLLIDERGROUP* pTemp = new COLLIDERGROUP[iGroupCount + 1];

		for ( _uint i = 0; i < iGroupCount; ++i )
		{
			pTemp[i] = pGroup[i];
		}

		pTemp[iGroupCount] = _ColliderGroup;

		// 원래 있던 공간은 삭제하고 담아 놓은 콜리더를 받아옴..
		Engine::Safe_Delete_Array( pGroup );

		pGroup = pTemp;

		++iGroupCount;
	}

	void Release()
	{
		for ( _uint i = 0; i < iGroupCount; ++i )
		{
			pGroup[i].Release();
		}

		Engine::Safe_Delete_Array( pGroup );

		iGroupCount = 0;
	}

} COLLIDERGROUPDATA;
#endif

class DLL_EXPORT CCharacter abstract
	: public CGameObject
{
protected:
	explicit CCharacter(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCharacter() = default;

	/*FUNCTION*/
	//public:
	//CHAR_TYPE Get_CharType() { return m_tInfo.m_eCharType; }
public:
	virtual HRESULT Ready_GameObject() PURE;
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void	Render_GameObject() PURE;
	virtual void	Render_Shadow( LPD3DXEFFECT pEffect, const char* pConstantWorldMatrixName ) override;
	virtual void	Reset_WorldState();
public:
	// 애니메이션 이벤트가 발생했을 때 호출할 함수..
	void Act_AnimationEvent( const ANI_STATUS& _AniStatus, const _uint& _iEventIndex );
public:
	// 상호 작용 실행 시 호출되는 함수( 상호 작용을 하는 대상이 상호 작용을 당하는 대상의 함수를 호출해줌 )..
	void OnInteraction( const INTERACTIONINFO& _InteractionInfo, const _uint& _iInteractionIndex );
	_bool IsCanInteraction( void );
protected:
	HRESULT Ready_Character_Component();
	HRESULT Ready_LoadCollider( const _tchar* _pFilePath, Engine::CCollider* _pCollder );
	HRESULT Ready_CollidersToColliderManager( const _int& _iGroupType, Engine::CGameObject * _pGameObject );
	_bool	Colliders_Add_ColliderLayer( const _int& _iGroupType, const _tchar* _pLayerTag, const _uint& _iGroupIndex );
	_bool	Colliders_Exclude_ColliderLayer( const _int& _iGroupType, const _uint& _iGroupIndex );
protected:
	// 콜라이더를 키라는 애니메이션 이벤트가 발생했을 때 호출하는 함수( 가상 함수로 만들어 자식에서 재정의 )..
	virtual HRESULT Event_ColliderActOn( const ANI_STATUS & _AniStatus, const _uint & _iEventIndex );
	HRESULT Event_ColliderActOff( const ANI_STATUS & _AniStatus, const _uint & _iEventIndex );
protected:
	void LookTargetLookDir( CCharacter* _pCharacter, const _float& _fOffset );
protected:
	void Acc_CurrentHp( const _uint& _iAccHp );
	void Acc_CurrentCp( const _float& _fAccCp );
	/*MEMBER*/
protected:
	CCollider_Manager*		m_pCollider_Manager = nullptr;	// Add By.KMH
protected:
	//CTransform*				m_pTransformCom = nullptr;
	CMesh_Dynamic*			m_pMeshCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
#ifdef CHARACTER_COLLIDERMODIFY
	COLLIDERGROUPDATA		m_ColliderGroups;
#else
	CCollider*				m_pColliderCom = nullptr;
#endif
	CNavigation*			m_pNaviCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CController*			m_pController = nullptr; // 움직이려면 컨트롤러 등록 필수
	CCamera*				m_pCamera = nullptr;

															//vector<CAction>			m_vecAction;			// 캐릭터 액션 벡터
															//CWorldState				m_WorldState;			// 캐릭터 월드 스테이트
															//int						m_StartAnimNum = 0;		// 캐릭터 애니메이션 시작 넘버
															//int						m_EndAniNum = 0;		// 캐릭터 애미네이션 종료 넘버
	_tchar*					m_TrailEffectTag = nullptr;
	CHARINFO				m_tInfo;		// 캐릭터 정보
public:
	//FORCEINLINE int	Get_StartAniNum() { return m_StartAnimNum; } // 캐릭터 액션 벡터 반환
	//FORCEINLINE int	Get_EndAniNum() { return m_EndAniNum; } // 캐릭터 액션 벡터 반환
	//FORCEINLINE const vector<CAction>*	Get_ActionVector() const { return &m_vecAction; } // 캐릭터 액션 벡터 반환
	//FORCEINLINE vector<CAction>*		Get_ActionVector_Point() { return &m_vecAction; } // (절대주의_포인트) 캐릭터 액션 벡터 반환
	//FORCEINLINE vector<CAction>&		Get_ActionVector_Reference() { return m_vecAction; } // (절대주의_레퍼런스) 캐릭터 액션 벡터 반환
	//FORCEINLINE const CWorldState&		Get_WorldState() const { return m_WorldState; } // 캐릭터 월드스테이트 반환
	//FORCEINLINE CWorldState&			Get_WorldState_Reference() { return m_WorldState; } // (절대주의_레퍼런스) 캐릭터 월드스테이트 반환

	FORCEINLINE const CHARINFO&			Get_Info() const { return m_tInfo; }		// 캐릭터 정보 반환
	FORCEINLINE CController*			Get_Controller() { return m_pController; }
	//FORCEINLINE CTransform*				Get_Transform() { return m_pTransformCom; }
	FORCEINLINE CMesh_Dynamic*			Get_Mesh() { return m_pMeshCom; }
	FORCEINLINE CCamera*				Get_Camera() { return m_pCamera; }
	FORCEINLINE CHARINFO&				Get_Info_ForMapTool() { return m_tInfo; }		// 사용 금지

public:
	void Set_Controller(CController* pController);// 사용 금지
	HRESULT Set_Camera(CCamera* pCamera); // 사용 자제할 것.
										  // --------- 장비 ------------
private:
	vector<CEquipment*>	m_pEquipVector; //일부로 private 해놓음 -JW
	int m_iItemCount = 0;
public:
	HRESULT Equip_Item(CEquipment* pItem,_matrix* pBoneMatrix);
	void	Hide_Item(bool bHide,int iIndex = -1); // true - 숨김 / false - 보임
	CEquipment*	Get_Equipment(int iIndex = 0) { 
		if (m_pEquipVector.size() > iIndex)
			return m_pEquipVector[iIndex];
		else
			return nullptr;
	}
	FORCEINLINE int	Get_EquipmentSize() { return m_iItemCount; }
	// --------- 장비 ------------


	// 행동 함수(필요 함수만 하위 클래스에서 오버로딩)
public:
	virtual EStatus Move_Forward(float fScale = 1.f);
	virtual EStatus Move_Right(float fScale = 1.f);
	//virtual EStatus Move_Jump(float fScale = 1.f) PURE;
	//virtual EStatus Turn(float fScale = 1.f) PURE;

	//virtual EStatus	Change_Player();
	//virtual EStatus	Tag_Leo();
	//virtual EStatus	Tag_Mike();
	//virtual EStatus	Tag_Raph();
	//virtual EStatus	Tag_Don();


	//
	//virtual EStatus Mode_On_Stealth();
	//virtual EStatus Mode_Off_Stealth();
	//virtual EStatus Attack_Forward_Dash();

private:
	void  Move_Navigation();

	// Test  삭제할것
protected:
	_bool	m_bNavi = false;

public:
	friend void CController::Get_TargetInfo(CCharacter* pTarget);

#ifndef _FINAL
public:
	void	Debug_Render(void);
#endif
protected:
	virtual _ulong	Free() override;
};

END