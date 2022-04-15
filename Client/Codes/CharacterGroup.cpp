#include "stdafx.h"
#include "..\Headers\CharacterGroup.h"

#include "Component_Manager.h"
#include "CharacterGroupLayer.h"

#include "Monster.h"

CCharacterGroup::CCharacterGroup( LPDIRECT3DDEVICE9 _pGraphicDev )
	: m_pGraphicDev( _pGraphicDev )
{
	m_pGraphicDev->AddRef();
}

_uint CCharacterGroup::Get_CharSize( void )
{
	return m_GroupData.iCharCount;
}

HRESULT CCharacterGroup::Add_Character( CMonster* _pCharacter )
{
	CHARDATA	CharData;

	CharData.pCharacter = _pCharacter;
	CharData.pController = _pCharacter->Get_Controller();
	CharData.pTransform = _pCharacter->Get_Transform();

	if ( nullptr == CharData.pCharacter ||
		 nullptr == CharData.pController )
		return E_FAIL;

	CharData.pCharacter->AddRef();
	CharData.pController->AddRef();
	CharData.pTransform->AddRef();

	CharData.pController->Set_Target( m_pTarget );

	m_GroupData.CharList.push_back( CharData );

	++m_GroupData.iCharCount;

	return NOERROR;
}

CMonster* CCharacterGroup::Erase_Character( void )
{
	if ( m_GroupData.CharList.empty() )
		return nullptr;

	CHARDATA& CharData = m_GroupData.CharList.front();
	CMonster* pCharacter = CharData.pCharacter;

	CharData.Release();

	m_GroupData.CharList.pop_front();

	--m_GroupData.iCharCount;

	return pCharacter;
}

void CCharacterGroup::Update_CharacterGroup( const _float & _fTimeDelta )
{
	for ( auto iter = m_GroupData.CharList.begin(); iter != m_GroupData.CharList.end(); )
	{
		if ( false == (*iter).pCharacter->Get_IsDead() )
		{
			(*iter).vPos = *(*iter).pTransform->Get_Infomation( Engine::CTransform::INFO_POSITION );

			++iter;
		}
		else
		{
			(*iter).Release();

			iter = m_GroupData.CharList.erase( iter );
			
			m_GroupData.iCharCount = (_uint)m_GroupData.CharList.size();

			m_pCharGroupLayer->DieGroupCharacter( this );
		}
	}

	for ( auto& Char : m_GroupData.CharList )
	{
		CHARINFO& CharInfo = const_cast<CHARINFO&>(Char.pCharacter->Get_Info());

		Char.fTime_Atk -= _fTimeDelta;
		Char.fTime -= _fTimeDelta;

		//_vec3 vSeparate( 0.f, 0.f, 0.f );
		//_vec3 vAlign( 0.f, 0.f, 0.f );
		//_vec3 vCohesion( 0.f, 0.f, 0.f );
		//_vec3 vResult( 0.f, 0.f, 0.f );
		//
		//Char.Separate( vSeparate, m_GroupData );
		//Char.Align( vAlign, m_GroupData );
		//Char.Cohesion( vCohesion, m_GroupData );
		//
		//_vec3 vLook = *Char.pTransform->Get_WorldLookVec();
		//vLook.y = 0.f;
		//D3DXVec3Normalize( &vLook, &vLook );
		//
		//vSeparate *= 2.5f;
		//vAlign *= 1.f;
		//vCohesion *= 1.f;
		//
		//vResult = (vSeparate + vAlign + vCohesion);
		//vResult.y = 0.f;
		//
		//Char.vVelocity += vResult;
		//Char.Limit( Char.vVelocity, m_GroupData.fMaxSpeed );
		//
		//D3DXVec3Normalize( &vResult, &Char.vVelocity );
		//
		//_float fCos = D3DXVec3Dot( &vResult, &vLook );
		//fCos = D3DXToDegree( acosf( fCos ) );
		//
		//CDebug::GetInstance()->Add_ConsoleDebugText( "%f", fCos );
		//
		//_vec3 vMove( 0.f, 0.f, 0.f );
		//vMove.x = sinf( fCos );
		//vMove.z = cosf( fCos );
		//
		//Char.pTransform->Go_Direction( vMove, 2.f, _fTimeDelta );
		//
		//if(fCos <= 45.f || fCos > 315.f )
		//	CharInfo.m_WorldState.setVariable( SIGNAL_CHANGE_DIR_FRONT, true );
		//else if(fCos <= 135.f && fCos > 45.f )
		//	CharInfo.m_WorldState.setVariable( SIGNAL_CHANGE_DIR_RIGHT, true );
		//else if(fCos <= 225.f && fCos > 135.f )
		//	CharInfo.m_WorldState.setVariable( SIGNAL_CHANGE_DIR_BACK, true );
		//else if(fCos <= 315.f && fCos > 225.f )
		//	CharInfo.m_WorldState.setVariable( SIGNAL_CHANGE_DIR_LEFT, true );



		if (0.f >= Char.fTime_Atk)
		{
			if (true == CharInfo.m_WorldState.getVariable(CH_STATE_MID_TARGET) &&
				false == CharInfo.m_WorldState.getVariable(ACT_ATTACK))
			{
				CharInfo.m_WorldState.setVariable(SIGNAL_ATTACK_TARGET, true);

#ifndef _FINAL
				//CDebug::GetInstance()->Add_ConsoleDebugText(L"Signal Attack");
#endif

				//몬스터 어택 가상함수 부름
				Char.pCharacter->SignalUpdate(&Char);
			}
		}



		switch ( Char.iActState )
		{
			case 0:
			{
				if ( Char.fTime <= 0.f )
				{
					//몬스터 이동 가상 함수를 부름;
					Char.pCharacter->Move_SignalUpdate(&Char);
				}
			}
			break;
		}
	}
}

HRESULT CCharacterGroup::Ready_CharacterGroup( Engine::CCharacter* _pTarget, CCharacterGroupLayer* _pCharGroupLayer )
{
	if ( nullptr == _pTarget ||
		 nullptr == _pCharGroupLayer )
		return E_FAIL;

	m_pTarget = _pTarget;
	m_pTarget->AddRef();

	m_pCharGroupLayer = _pCharGroupLayer;

	return NOERROR;
}

CCharacterGroup * CCharacterGroup::Create( LPDIRECT3DDEVICE9 _pGraphicDev, Engine::CCharacter* _pTarget, CCharacterGroupLayer* _pCharGroupLayer )
{
	if ( nullptr == _pGraphicDev )
		return nullptr;

	CCharacterGroup* pInstance = new CCharacterGroup( _pGraphicDev );

	if ( pInstance->Ready_CharacterGroup( _pTarget, _pCharGroupLayer ) )
	{
		CREATE_FAIL( pInstance, "CCharacterGroup" );
	}

	return pInstance;
}

_ulong CCharacterGroup::Free( void )
{
	_ulong dwRefCnt = 0;

	for ( auto& CharData : m_GroupData.CharList )
		CharData.Release();

	m_GroupData.CharList.clear();
	m_GroupData.iCharCount = 0;

	dwRefCnt = Engine::Safe_Release( m_pTarget );
	dwRefCnt = Engine::Safe_Release( m_pGraphicDev );

	return 0;
}

void CCharacterGroup::tagCharData::Release( void )
{
	Engine::Safe_Release( pCharacter );
	Engine::Safe_Release( pController );
	Engine::Safe_Release( pTransform );
}

void CCharacterGroup::tagCharData::Separate( _vec3 & _vOut, const GROUPDATA & _GroupData )
{
	float desiredseparation = 8.f;
	_vec3 steer( 0.f, 0.f, 0.f );
	int count = 0;

	// For every boid in the system, check if it's too close
	for ( auto& Char : _GroupData.CharList )
	{
		if ( Char.pCharacter->GetKey() == pCharacter->GetKey() )
			continue;

		_vec3 vDiff = vPos - Char.vPos;
		float d = D3DXVec3Length( &vDiff );

		if ( d > 0 && d < desiredseparation*desiredseparation )
		{
			// If the distance is greater than 0 and less than an arbitrary amount (0 when you are yourself)
			// Calculate vector pointing away from neighbor
			vDiff /= d;        // Weight by distance
			steer += vDiff;
			++count;
		}
	}

	if ( count > 0 )
	{
		steer /= (float)count;
		D3DXVec3Normalize( &steer, &steer );
	}

	_vOut = steer;
}

void CCharacterGroup::tagCharData::Align( _vec3 & _vOut, const GROUPDATA & _GroupData )
{
	float neighbordist = 50;
	_vec3 sum( 0.f, 0.f, 0.f );
	int count = 0;

	for ( auto& Char : _GroupData.CharList )
	{
		if ( Char.pCharacter->GetKey() == pCharacter->GetKey() )
			continue;

		float d = D3DXVec3Length( &(vPos - Char.vPos) );

		if ((d > 0) && (d < neighbordist))
		{
			sum += Char.vVelocity;
			count++;
		}
	}

	if (count > 0)
	{
		sum /= (float)count;
		// First two lines of code below could be condensed with new Vector2 setMag() method
		// Not using this method until Processing.js catches up
		// sum.setMag(maxspeed);

		// Implement Reynolds: Steering = Desired - Velocity
		D3DXVec3Normalize( &sum, &sum );
		sum *= _GroupData.fMaxSpeed;
		_vec3 steer = sum - vVelocity;
		Limit( steer, _GroupData.fMaxForce );
	} 
}

// Cohesion
// For the average location (i.e. center) of all nearby boids, calculate steering vector towards that location
void CCharacterGroup::tagCharData::Cohesion( _vec3 & _vOut, const GROUPDATA & _GroupData )
{
	float neighbordist = 50;
	_vec3 sum( 0.f, 0.f, 0.f );   // Start with empty vector to accumulate all locations
	int count = 0;

	for ( auto& Char : _GroupData.CharList )
	{
		if ( Char.pCharacter->GetKey() == pCharacter->GetKey() )
			continue;

		float d = D3DXVec3Length( &(vPos - Char.vPos) );

		if ((d > 0) && (d < neighbordist))
		{
			sum += Char.vPos; // Add location
			count++;
		}
	}

	if (count > 0)
	{
		sum /= count;
		_vec3 desired = sum - vPos;  // A vector pointing from the location to the target
											 // Scale to maximum speed
		D3DXVec3Normalize( &desired, &desired );
		desired *= _GroupData.fMaxSpeed;

		// Above two lines of code below could be condensed with new Vector2 setMag() method
		// Not using this method until Processing.js catches up
		// desired.setMag(maxspeed);

		// Steering = Desired minus Velocity
		_vec3 steer = desired - vVelocity;
		Limit( steer, _GroupData.fMaxForce );  // Limit to maximum steering force
	} 
}

void CCharacterGroup::tagCharData::Limit( _vec3& _vOut, const _float& _fMax )
{
	_float fLength = D3DXVec3Length( &_vOut );
	_vec3 vNormalized;
	D3DXVec3Normalize( &vNormalized, &_vOut );

	if ( fLength > _fMax*_fMax )
		_vOut = vNormalized*_fMax;
}
