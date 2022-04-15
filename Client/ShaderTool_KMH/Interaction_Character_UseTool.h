#pragma once
#include "Tool_Defines.h"
#include "Character.h"

namespace Engine
{
	class CRenderer;
	class CShader;
	class CMesh_Dynamic;
	class CCollider_Manager;
}

class CInteraction_Character_UseTool final :
	public Engine::CCharacter
{
	enum LeoColliderGroupKind { LeoColGroup_LWeapon, LeoColGroup_RWeapon, LeoColGroup_Hit, LeoColGroup_RFoot, LeoColGroup_LFoot, LeoColGroup_End };
	enum DonColliderGroupKind { DonColGroup_Hit, DonColGroup_RWeapon, DonColGroup_RFoot, DonColGroup_LFoot, DonColGroup_End };
	enum RaphColliderGroupKind { RaphColGroup_Hit, RaphColGroup_RWeapon, RaphColGroup_LWeapon, RaphColGroup_RFoot, RaphColGroup_LFoot, RaphColGroup_RKnee, RaphColGroup_LKnee, RaphColGroup_End };
	enum MikeColliderGroupKind { MikeColGroup_Hit, MikeColGroup_RWeapon, MikeColGroup_LWeapon, MikeColGroup_RKnee, MikeColGroup_LKnee, MikeColGroup_End };
	enum { SlimThugColGroup_Hit, SlimThugColGroup_Attack, SlimThugColGroup_End };
	enum { BigThugColGroup_Hit, BigThugColGroup_Attack, BigThugColGroup_End };
	enum { KnifeThugColGroup_Hit, KnifeThugColGroup_Attack, KnifeThugColGroup_End };
public:
	explicit CInteraction_Character_UseTool( LPDIRECT3DDEVICE9 _pGraphicDev );
	virtual ~CInteraction_Character_UseTool() = default;
public:
	void Set_CurAniIndex( const _uint& _iAniIndex );
	void Set_AniSpeed( const _float& _fAniSpeed );
	void Set_IsStartAni( const _bool& _bIsStartAni );
	void Set_StartPosition( const _vec3& _vPos );
	void Set_StartAngle( const _vec3& _vAngle );
public:
	_double Get_AniPeriod( void );
	_double Get_AniPosition( void );
public:
	void LookCharacter( CInteraction_Character_UseTool* _pLookCharacter, const _float& _fOffset );
	void PositionUpdate( CInteraction_Character_UseTool* _pLookCharacter, const _float& _fDist );
public:
	HRESULT Ready_GameObject( void );
	_int Update_GameObject( const _float& _fTimeDelta );
	_int LastUpdate_GameObject( const _float& _fTimeDelta );
	void Render_GameObject( void );
private:
	CCollider_Manager*		m_pCollider_Manager = nullptr;
private:
	LPD3DXEFFECT			m_pEffect = nullptr;
	_double*				m_pPeriod = nullptr;
	D3DXTRACK_DESC*			m_pTrackDesc = nullptr;
private:
	_bool					m_bIsStartAni = true;
	_vec3					m_vStartPos = _vec3( 0.f, 0.f, 0.f );
	_vec3					m_vStartAngle = _vec3( 0.f, 0.f, 0.f );
	_vec3					m_vStartScale = _vec3( 0.01f, 0.01f, 0.01f );
	_float					m_fSpeed = 10.f;
	_float					m_fRotSpeed = 10.f;
private:
	HRESULT Ready_Interaction_Character( const CHARACTERKIND& _eCharKind );
	HRESULT Ready_Component( const CHARACTERKIND& _eCharKind );
	HRESULT Ready_DefaultSetting( void );
	HRESULT SetUp_OnConstantTable( LPD3DXEFFECT _pEffect );
private:
	HRESULT Character_LoadCollider( const CHARACTERKIND& _eCharKind, Engine::CCollider* _pCollder );
	HRESULT Character_ColliderSetting( const CHARACTERKIND& _eCharKind );
	HRESULT Ready_Equipment( const CHARACTERKIND& _eCharKind );
private:
	HRESULT Create_Mesh( const CHARACTERKIND& _eCharKind, const _tchar* _pMeshComTag );
	_float Compute_LookAngle( CTransform* _pTransform, const _float& _fOffsetAngle );
	void Reset_TransformData( void );
public:
	static CInteraction_Character_UseTool* Create( LPDIRECT3DDEVICE9 _pGraphicDev, const CHARACTERKIND& _eCharKind );
private:
	virtual _ulong Free( void ) override;
};

