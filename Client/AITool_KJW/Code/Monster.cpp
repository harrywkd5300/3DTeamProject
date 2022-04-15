#include "stdafx.h"
#include "..\Headers\Monster.h"
#include "Clinet_Component.h"
#include "Object_Manager.h"
#include "Component_Manager.h"
#include "Sword.h"
#include "Navigation.h"
#include "Graphic_Device.h"

#include "Camera_Player.h"
#include "Collider_Manager.h"
CMonster::CMonster(LPDIRECT3DDEVICE9 pGraphicDev)
	: CCharacter(pGraphicDev)
{
	m_tInfo.m_eCharType = CHARINFO::CHAR_MONSTER;
#ifndef _FINAL
	Tag(L"CMonster", std::bind(&CMonster::Debug_Render, this));
#endif

}


//HRESULT CMonster::Ready_GameObject(void)
//{
//	//_vec3* TestLeakvec = new _vec3(0.f, 0.f, 0.f);
//
//	if (FAILED(CCharacter::Ready_Character_Component()))
//		return E_FAIL;
//
//	if (FAILED(Ready_Component()))
//		return E_FAIL;		
//
//	if (FAILED(Ready_Controller()))
//		return E_FAIL;
//
//
//	m_tInfo.m_fSpeed = 5.f;
//
//	m_pTransformCom->Set_Infomation(Engine::CTransform::INFO_POSITION, &_vec3(90.2f, 0.f, 115.0f));
//	m_pTransformCom->Set_Infomation(Engine::CTransform::INFO_SCALE, &_vec3(0.02f, 0.02f, 0.02f));
//
//	m_pMeshCom->Set_AnimationSet(0);
//	m_pMeshCom->Play_Animation(0.f);
//
//	return NOERROR;
//}

_int CMonster::Update_GameObject(const _float & fTimeDelta)
{
	
#ifndef _FINAL
	m_bFirstMonster = true;
#endif
	//if (nullptr != m_pRendererCom)
	//{
	//	m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_NONALPHA, this);
	//}


	return CCharacter::Update_GameObject(fTimeDelta);
}

_int CMonster::LastUpdate_GameObject(const _float & fTimeDelta)
{
	m_fTempTimeDelta += fTimeDelta;
	m_fEvade_TempTimeDelta += fTimeDelta;

	if (1.f <= m_fTempTimeDelta && false == m_tInfo.m_bShield)
	{
		m_tInfo.m_iShield_Point += m_iAdd_Shield_Point;
		m_fTempTimeDelta = 0.f;
	}

	if (1.f <= m_fEvade_TempTimeDelta && false == m_tInfo.m_bEvade)
	{
		m_tInfo.m_fEvade_Point += m_fAdd_Evade_Point;
		m_fEvade_TempTimeDelta = 0.f;
	}



	if (1.f <= m_tInfo.m_fEvade_Point)
		m_tInfo.m_fEvade_Point = 1.f;
	if (0.f >= m_tInfo.m_fEvade_Point)
		m_tInfo.m_fEvade_Point = 0.f;

	if (m_iShieldMax <= m_tInfo.m_iShield_Point)
		m_tInfo.m_iShield_Point = m_iShieldMax;
	if (0 >= m_tInfo.m_iShield_Point)
		m_tInfo.m_iShield_Point = 0;

	if (1.f <= m_tInfo.m_fEvade_Point)
		m_tInfo.m_bEvade = true;
	if (0.f >= m_tInfo.m_fEvade_Point)
		m_tInfo.m_bEvade = false;

	if (m_iShieldMax <= m_tInfo.m_iShield_Point)
		m_tInfo.m_bShield = true;
	if (0 >= m_tInfo.m_iShield_Point)
		m_tInfo.m_bShield = false;


	return CCharacter::LastUpdate_GameObject(fTimeDelta);
}


void CMonster::SignalUpdate(CCharacterGroup::CHARDATA* ptChar)
{
}

void CMonster::OnCollision( const Engine::Collision & _Collision )
{
	switch ( _Collision.CollisionType )
	{
		case Engine::COLLISION_ENTER:	// 현재 충돌한 콜라이더와 이전 프레임에 충돌을 한 적이 없음..
		{
			list<Engine::OPTIONDATA>	OptionList;
			if ( false == _Collision.pOtherCollider->Get_OptionList( OptionList ) )
				return;

			for ( auto& Option : OptionList )
			{
				if ( Option.dwOption == SIGNAL_ACT_INTERACTION )
				{

#ifndef _FINAL
					//CDebug::GetInstance()->Add_ConsoleDebugText( L"Counter" );
#endif

					if ( false == Option.DetailDataList.empty() )
					{
						auto IterDetailData = Option.DetailDataList.begin();

						m_tInfo.m_WorldState.setVariable( (*IterDetailData), true );	// 실행해야 할 애니메이션을 true 로 바꿔준다..
					}

					m_tInfo.m_WorldState.setVariable( Option.dwOption, true );	// 카운터 공격을 맞았다는 신호를 보낸다..
				}
				else if ( Option.dwOption == SIGNAL_HIT_ENEMY )
				{
#ifndef _FINAL
					//CDebug::GetInstance()->Add_ConsoleDebugText( L"Hit" );
#endif

					// 만약 회피를 할 수 있다면??..

					if (true == m_tInfo.m_bEvade)
					{
						m_tInfo.m_fEvade_Point -= 1;
						m_tInfo.m_WorldState.setVariable(SIGNAL_EVADE_ON, true);


						_uint iCount = CRandom::GetInstance()->GetValue(0, 3);

						switch (iCount)
						{
						case 0:
							m_tInfo.m_WorldState.setVariable(SIGNAL_EVADE_BACK, true);
							break;
						case 1:
							m_tInfo.m_WorldState.setVariable(SIGNAL_EVADE_LEFT, true);
							break;
						case 2:
							m_tInfo.m_WorldState.setVariable(SIGNAL_EVADE_RIGHT, true);
							break;
						}


					}
					else if ( false == m_tInfo.m_bEvade )
					{
						m_tInfo.m_WorldState.setVariable( SIGNAL_EVADE_ON, false );

						// 만약 방어를 할 수 있다면??..
						if ( true == m_tInfo.m_bShield )
						{
							m_tInfo.m_iShield_Point -= 10;

							if ( 0 >= m_tInfo.m_iShield_Point )
								m_tInfo.m_WorldState.setVariable( SIGNAL_SHIELD_BREAK_ON, true );
							else
								m_tInfo.m_WorldState.setVariable( SIGNAL_SHIELD_BREAK_ON, false );

							m_tInfo.m_WorldState.setVariable( SIGNAL_SHIELD_ON, true );
						}
						else if ( false == m_tInfo.m_bShield )
						{
							m_tInfo.m_WorldState.setVariable( SIGNAL_SHIELD_ON, false );

							// 적에게 데미지를 받아서 죽었는지 체크..
							m_tInfo.m_iCurHp -= 10;

							if ( m_tInfo.m_iCurHp <= 0 )
							{
#ifndef _FINAL
								//CDebug::GetInstance()->Add_ConsoleDebugText( L"Death" );
#endif
								m_tInfo.m_WorldState.setVariable( SIGNAL_DIE, true );
							}
						}
					}


					//CDebug::GetInstance()->Add_ConsoleDebugText( L"Hit" );

					m_tInfo.m_WorldState.setVariable( SIGNAL_HIT_ENEMY, true );

					m_tInfo.m_WorldState.setVariable( Option.dwOption, true );	// 맞았다는 신호를 보낸다..

					// 나를 때린 적을 멤버에 보관한다..
					m_tInfo.m_pEnemy_Attacked = dynamic_cast<CCharacter*>(_Collision.pOtherCollider->Get_GameObject());
				}
			}
		}
		break;
		case Engine::COLLISION_STAY:	// 현재 충돌한 콜라이더와 이전 프레임에 충돌을 했음..
		{

		}
		break;
	}
}

void CMonster::Move_SignalUpdate( CCharacterGroup::CHARDATA * ptChar )
{
}

//
//void CMonster::Render_GameObject(void)
//{
//	if (nullptr == m_pMeshCom)
//		return;
//
//	if (nullptr == m_pGraphicDev || nullptr == m_pShaderCom)
//		return;
//
//#ifndef _FINAL
//	m_pColliderCom->Render_Buffer();
//#endif 
//
//	LPD3DXEFFECT	pEffect = m_pShaderCom->Get_EffectHandle();
//	if (nullptr == pEffect)
//		return;
//	pEffect->AddRef();
//
//
//	SetUp_OnConstantTable(pEffect);
//
//	pEffect->Begin(nullptr, 0);
//
//	pEffect->BeginPass(0);
//
//	m_pMeshCom->Render_Mesh(pEffect, "g_BaseTexture", "g_NormalTexture", "g_SpecularTexture");
//
//	pEffect->EndPass();
//
//	pEffect->End();
//
//	Safe_Release(pEffect);
//
//	m_pNaviCom->Render_Navigation();
//	
//}

HRESULT CMonster::Ready_Component(void)
{
	Engine::CComponent*			pComponent = nullptr;
		



	// For.Navigation
	pComponent = m_pNaviCom = (Engine::CNavigation*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"CNavigation");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Navigation", pComponent));
	m_pNaviCom->AddRef();
	m_pNaviCom->Set_Currrent_Index(0);

	// For.Shader
	pComponent = m_pShaderCom = (Engine::CShader*)CComponent_Manager::GetInstance()->Clone_Component(0, L"CShader_Mesh");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Shader", pComponent));
	m_pShaderCom->AddRef();



	return NOERROR;
}

HRESULT CMonster::Ready_Controller(void)
{
	Engine::CComponent*			pComponent = nullptr;

	// For.Controller
	pComponent = m_pController = (CAIController*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CAIController");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Controller", pComponent));
	m_pController->AddRef();

	dynamic_cast<CAIController*>(pComponent)->Possess(this);
	  
	return NOERROR;
}

#ifndef _FINAL
_bool CMonster::m_bFirstMonster = true;
_uint CMonster::m_iType = 0;

void CMonster::Render_DebugSphere( void )
{
	if ( m_bFirstMonster == false )
		return;

	m_bFirstMonster = false;

	CDebug::GetInstance()->Add_DebugText( 1500.f, 600.f, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), "몬스터 범위( Near, Far 등의 ) 띄우기 -> NumPad8" );

	switch ( m_iType )
	{
		case 0:
			CDebug::GetInstance()->Add_DebugText( 1500.f, 620.f, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), "몬스터 Close 범위 보기 -> NumPad8" );
			return;
		case 1:
			CDebug::GetInstance()->Add_DebugText( 1500.f, 620.f, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), "몬스터 Close 범위( Near 범위 보기 -> NumPad8 )" );
			break;
		case 2:
			CDebug::GetInstance()->Add_DebugText( 1500.f, 620.f, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), "몬스터 Near 범위( Mid 범위 보기 -> NumPad8 )" );
			break;
		case 3:
			CDebug::GetInstance()->Add_DebugText( 1500.f, 620.f, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), "몬스터 Mid 범위( Far 범위 보기 -> NumPad8 )" );
			break;
		case 4:
			CDebug::GetInstance()->Add_DebugText( 1500.f, 620.f, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), "몬스터 Far 범위( ToFar 범위 보기 -> NumPad8 )" );
			break;
		case 5:
			CDebug::GetInstance()->Add_DebugText( 1500.f, 620.f, D3DXCOLOR( 1.f, 0.f, 1.f, 1.f ), "몬스터 범위 안띄우기 -> NumPad8" );
			break;
	}

	if ( nullptr == m_pGreenTexture )
	{
		if ( FAILED( D3DXCreateTexture( m_pGraphicDev, 1, 1, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_pGreenTexture ) ) )
		{
			MSG_BOX( "Texture Create Failed" );
			return;
		}

		if ( FAILED( D3DXCreateTexture( m_pGraphicDev, 1, 1, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_pRedTexture ) ) )
		{
			MSG_BOX( "Texture Create Failed" );
			return;
		}

		D3DLOCKED_RECT			Lock_Rect;

		m_pGreenTexture->LockRect(0, &Lock_Rect, nullptr, 0);
		for (size_t i = 0; i < 1; i++)	
		{
			_byte*	pPixel = (_byte*)Lock_Rect.pBits + Lock_Rect.Pitch * i;

			for (size_t j = 0; j < 1; j++)
			{
				((_ulong*)pPixel)[j] = D3DXCOLOR(0.f, 1.f, 0.f, 1.f);
			}
		}
		m_pGreenTexture->UnlockRect(0);

		m_pRedTexture->LockRect(0, &Lock_Rect, nullptr, 0);
		for (size_t i = 0; i < 1; i++)
		{
			_byte*	pPixel = (_byte*)Lock_Rect.pBits + Lock_Rect.Pitch * i;

			for (size_t j = 0; j < 1; j++)
			{
				((_ulong*)pPixel)[j] = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);
			}
		}
		m_pRedTexture->UnlockRect(0);	

		if ( FAILED( D3DXCreateSphere( m_pGraphicDev, 1.f, 40, 30, &m_pSphere, nullptr ) ) )
		{
			MSG_BOX( "Sphere Create Failed" );
			return;
		}
	}

	_matrix matScale[5];
	_matrix matTrans;

	_vec3 vPos = *m_pTransformCom->Get_Infomation( Engine::CTransform::INFO_POSITION );

	D3DXMatrixTranslation( &matTrans, vPos.x, vPos.y, vPos.z );

	D3DXMatrixScaling( &matScale[0], m_tInfo.m_fRange_Close, m_tInfo.m_fRange_Close, m_tInfo.m_fRange_Close );
	D3DXMatrixScaling( &matScale[1], m_tInfo.m_fRange_Near, m_tInfo.m_fRange_Near, m_tInfo.m_fRange_Near );
	D3DXMatrixScaling( &matScale[2], m_tInfo.m_fRange_Mid, m_tInfo.m_fRange_Mid, m_tInfo.m_fRange_Mid );
	D3DXMatrixScaling( &matScale[3], m_tInfo.m_fRange_Far, m_tInfo.m_fRange_Far, m_tInfo.m_fRange_Far );
	D3DXMatrixScaling( &matScale[4], m_tInfo.m_fRange_TooFar, m_tInfo.m_fRange_TooFar, m_tInfo.m_fRange_TooFar );

	m_matSphereWorld[m_iType - 1] = matScale[m_iType - 1] * matTrans;

	// 그리기 전에 전에 세팅 된 World 행렬을 잠깐 보관..
	_matrix matTempWorld;
	m_pGraphicDev->GetTransform( D3DTS_WORLD, &matTempWorld );


	// 디버그 용 Sphere 렌더..
	m_pGraphicDev->SetRenderState( D3DRS_LIGHTING, FALSE );
	m_pGraphicDev->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
	m_pGraphicDev->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	m_pGraphicDev->SetTexture( 0, m_pGreenTexture );

	m_pGraphicDev->SetTransform( D3DTS_WORLD, &m_matSphereWorld[m_iType - 1] );

	m_pSphere->DrawSubset( 0 );

	m_pGraphicDev->SetRenderState( D3DRS_LIGHTING, TRUE );
	m_pGraphicDev->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
	m_pGraphicDev->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );


	// 그린 후 전에 세팅 된 World 행렬 다시 세팅..
	m_pGraphicDev->SetTransform( D3DTS_WORLD, &matTempWorld );
}
#endif

HRESULT CMonster::Ready_Monster_Component()
{
	if (FAILED(CCharacter::Ready_Character_Component()))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	if (FAILED(Ready_Controller()))
		return E_FAIL;

	

	return NOERROR;
}

void CMonster::Move_OnTerrain(void)
{
	if (nullptr == m_pTransformCom)
		return;

	//if (nullptr == m_pTargetBufferCom)
	//{
	//	m_pTargetBufferCom = (CVIBuffer*)Engine::CObject_Manager::GetInstance()->Get_Component(SCENE_STAGE, L"Layer_BackGround", 0, L"Com_Buffer");
	//	m_pTargetBufferCom->AddRef(); 
	//}
	//m_pTransformCom->Move_OnBuffer(m_pTargetBufferCom);
}


//HRESULT CMonster::SetUp_OnConstantTable(LPD3DXEFFECT pEffect)
//{
//	pEffect->AddRef();
//
//	//m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorld");
//	m_pTransformCom->SetUp_OnShader_FORCED(pEffect, "g_matWorld");
//
//	_matrix			matView, matProj;
//	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
//	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
//
//	pEffect->SetMatrix("g_matView", &matView);
//	pEffect->SetMatrix("g_matProj", &matProj);	
//
//	Safe_Release(pEffect);
//
//	return NOERROR;
//}

//
//CMonster * CMonster::Create(LPDIRECT3DDEVICE9 pGraphicDev)
//{
//	CMonster *	pInstance = new CMonster(pGraphicDev);
//
//	if (FAILED(pInstance->Ready_GameObject()))
//	{
//		MSG_BOX("CMonster Created Failed");
//		Engine::Safe_Release(pInstance);
//	}
//	return pInstance;
//}

Engine::EStatus CMonster::Move_Forward(float fScale /*= 1.f*/)
{
	if (nullptr == m_pTransformCom)
		return EStatus::Failure;

	m_pTransformCom->Go_Straight(
		m_tInfo.m_fSpeed,
		m_pTimer->Get_TimeDelta());

	return EStatus::Success;
}

Engine::EStatus CMonster::Move_Right(float fScale /*= 1.f*/)
{
	if (nullptr == m_pTransformCom)
		return EStatus::Failure;

	m_pTransformCom->Go_Right(
		m_tInfo.m_fSpeed,
		m_pTimer->Get_TimeDelta());

	return EStatus::Success;
}


#ifndef _FINAL
void CMonster::Debug_Render(void)
{
	CDebug::GetInstance()->Add_DebugText("CMonster");

}
#endif
_ulong CMonster::Free(void)
{
	_ulong		dwRefCnt = 0;
	
#ifndef _FINAL
	Engine::Safe_Release( m_pGreenTexture );
	Engine::Safe_Release( m_pRedTexture );
	Engine::Safe_Release( m_pSphere );
#endif

	//Engine::Safe_Release(m_pTargetBufferCom);//Jang
	//Engine::Safe_Release(m_pPickingCom);//Jung
	//Engine::Safe_Release(m_pRSword);

	dwRefCnt = CCharacter::Free();

	return dwRefCnt;
}
