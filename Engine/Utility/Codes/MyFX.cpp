#include "..\Headers\MyFX.h"
#include "Component_Manager.h"
#include "Camera_Manager.h"

USING(Engine)

CMyFX::CMyFX(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
	, m_pRendererCom(nullptr)
	, m_pSystem(nullptr)
	, m_pCam_Manager(CCamera_Manager::GetInstance())
	, m_pRandom_Manager(CRandom::GetInstance())
	, m_tAtt(FX_ATT())
	, m_bIsAlive(true)
	, m_bIsClone(false)
{
	m_pRandom_Manager->AddRef();
	D3DXMatrixIdentity(&m_matRot);

#ifndef _FINAL
	Tag(L"CMyFX", std::bind(&CMyFX::Debug_Render, this));
#endif
}

CMyFX::CMyFX(const CMyFX & rhs)
	: CGameObject(rhs.m_pGraphicDev)
	, m_pRendererCom(rhs.m_pRendererCom)
	, m_pSystem(rhs.m_pSystem)
	, m_pCam_Manager(CCamera_Manager::GetInstance())
	, m_pRandom_Manager(CRandom::GetInstance())
	, m_tAtt(rhs.m_tAtt)
	, m_bIsAlive(rhs.m_bIsAlive)
	, m_bIsClone(true)
{
	if (FAILED(Ready_Component()))
		return;

	if (nullptr != m_pRendererCom)
		m_pRendererCom->AddRef();
	if (nullptr != m_pRandom_Manager)
		m_pRandom_Manager->AddRef();

	D3DXMatrixIdentity(&m_matRot);

#ifndef _FINAL
	Tag(L"CMyFX", std::bind(&CMyFX::Debug_Render, this));
#endif
}

_int CMyFX::Update_FX(const _float & fTimeDelta, EFFECT_INFO & tSystemInfo)
{
	_float	fTimeRatio = m_tAtt.fAge / m_tAtt.fLifeTime;

	// For Curve ------------------------------- //
	Curve_Scale(fTimeRatio, tSystemInfo);
	Curve_Color(fTimeRatio, tSystemInfo);

	if (tSystemInfo.m_iCurve_Alpha_Cnt > 1)
		Curve_Alpha(fTimeRatio, tSystemInfo);

	if (tSystemInfo.m_iCurve_Brt_Cnt > 1)
		Curve_Bright(fTimeRatio, tSystemInfo);

	if (tSystemInfo.m_iCurve_Spd_Cnt > 1)
		Curve_Speed(fTimeRatio, tSystemInfo);

	// ---------------------------------------- //

	m_tAtt.fAge += fTimeDelta;

	if (m_tAtt.fAge > m_tAtt.fLifeTime)
		return -1;

	Update_Movement(fTimeDelta, tSystemInfo);

	if (tSystemInfo.m_eBillBoardType != EFFECT_INFO::BB_NONE)
		Set_BillBoard(fTimeDelta, tSystemInfo);

	if (tSystemInfo.m_ePassType == EFFECT_INFO::PASS_CUTBLACK_SPRITE)
		Update_Sprite(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_ALPHA, this);

	CGameObject::Update_GameObject(fTimeDelta);

	return 0;
}

HRESULT CMyFX::Ready_GameObject(CMyEffect* pSystem)
{
	if (FAILED(Ready_Component()))
		return NOERROR;

	m_pSystem = pSystem;

	return NOERROR;
}

void CMyFX::Render_GameObject(void)
{
#ifndef _FINAL
	float fTime = 0.f;
	bool bErr = false;
	CHECK_TIME_START
#endif
	if (nullptr != m_pSystem)
		m_pSystem->Render_Effect(this);
#ifndef _FINAL
	CHECK_TIME_END(fTime, bErr);
	CDebug::GetInstance()->Add_EffectRunTime(fTime);
#endif
}

void CMyFX::Set_ParentMatrix(const D3DXMATRIX * pMatParent)
{
	m_pTransformCom->Set_ParentMatrix(pMatParent);
}

HRESULT CMyFX::Ready_Component(void)
{
	// For.Renderer
	Engine::CComponent*		pComponent = nullptr;

	pComponent = m_pTransformCom = (Engine::CTransform*)CComponent_Manager::GetInstance()->Clone_Component(0, L"CTransform");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_DYNAMIC].insert(MAPCOMPONENT::value_type(L"Com_Transform", pComponent));
	m_pTransformCom->AddRef();

	if (!m_bIsClone)
	{
		pComponent = m_pRendererCom = (Engine::CRenderer*)CComponent_Manager::GetInstance()->Clone_Component(0, L"CRenderer");
		if (nullptr == pComponent)
			return E_FAIL;
		m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"CRenderer", pComponent));
		m_pRendererCom->AddRef();
	}

	return NOERROR;
}

void CMyFX::Set_BillBoard(const _float & fTimeData, EFFECT_INFO& tInfo)
{
	if (nullptr == m_pCam_Manager)
		return;

	if (tInfo.m_eBillBoardType == EFFECT_INFO::BB_ALL)
	{
		_matrix		matInvView = m_pCam_Manager->Get_InverseViewMatrix();

		//m_pTransformCom->Set_WorldMatrix_ForEffect(matInvView); // 사라짐
		//m_pTransformCom->Set_Infomation(Engine::CTransform::INFO_POSITION, &vPos);
		m_pTransformCom->Set_Angle(matInvView);
	}
	else if (tInfo.m_eBillBoardType == EFFECT_INFO::BB_Y)
	{
		// 트랜스폼 기능 추가해야함!
	}
}

void CMyFX::Update_Sprite(const _float & fTimeDelta)
{
	m_tAtt.fCurFrame += m_tAtt.fSpriteSpd * fTimeDelta;

	// gara
	if (m_tAtt.fTotalFrame <= m_tAtt.fCurFrame)
		m_tAtt.fCurFrame = 0.f;
}

void CMyFX::Update_Movement(const _float & fTimeDelta, EFFECT_INFO& tInfo)
{
	if(tInfo.m_bUseForceRand)
		m_tAtt.vAcceleration = _vec3(m_pRandom_Manager->GetValueF(tInfo.m_fForce_X, tInfo.m_fForce_MaxX), m_pRandom_Manager->GetValueF(tInfo.m_fForce_Y, tInfo.m_fForce_MaxY), m_pRandom_Manager->GetValueF(tInfo.m_fForce_Z, tInfo.m_fForce_MaxZ));

	// 위치 업데이트
	if (tInfo.m_bUseVelocity || tInfo.m_bUseAccel)
	{
		m_tAtt.vVelocity += m_tAtt.vAcceleration * fTimeDelta;
		m_tAtt.vPosition += m_tAtt.vVelocity * fTimeDelta;
	}

	m_pTransformCom->Set_Infomation(Engine::CTransform::INFO_POSITION, &m_tAtt.vPosition);

	// 각속도 업데이트
	if (tInfo.m_bUseAngVelL) // 로컬
	{
		_vec3	vCurAngle = *m_pTransformCom->Get_Infomation(Engine::CTransform::INFO_ANGLE);
		vCurAngle += m_tAtt.vAngVelLocal * fTimeDelta;
		//D3DXMatrixRotationYawPitchRoll(&m_matRot, D3DXToRadian(vAngVel.y), D3DXToRadian(vAngVel.x), D3DXToRadian(vAngVel.z));
		
		m_pTransformCom->Set_Infomation(Engine::CTransform::INFO_ANGLE, &vCurAngle);
	}
	if (tInfo.m_bUseAngVelW) // 월드
	{
		_vec3	vAngle = m_tAtt.vAngVelWorld * fTimeDelta;
		D3DXMatrixRotationYawPitchRoll(&m_matRot, D3DXToRadian(vAngle.y), D3DXToRadian(vAngle.x), D3DXToRadian(vAngle.z));

		D3DXMATRIX	matParent = m_pTransformCom->Get_ParentMatrix();
		matParent = m_matRot * matParent;
		m_pTransformCom->Set_ParentMatrix(&matParent);
	}
}

void CMyFX::Curve_Scale(_float & fTimeRatio, EFFECT_INFO & tInfo)
{
	_bool	bIsChanged = false;
	// 스케일 커브 -> x, y, z 축으로 조절할 수 있도록 해야 함
	// x 축
	if (tInfo.m_arrCurve_Size_Cnt[EFFECT_INFO::AXIS_X] > 1)
	{
		for (int i = 0; i < tInfo.m_arrCurve_Size_Cnt[EFFECT_INFO::AXIS_X]; ++i)
		{
			if (fTimeRatio <= tInfo.m_arrCurve_Size_Axis_Time[EFFECT_INFO::AXIS_X][i])
			{
				// 1. 시간값이 0일 때
				if (tInfo.m_arrCurve_Size_Axis_Time[EFFECT_INFO::AXIS_X][i] == 0.f)
				{
					m_tAtt.vScale.x = tInfo.m_arrCurve_Size_Axis[EFFECT_INFO::AXIS_X][i] * m_tAtt.vScale_Origin.x;
					bIsChanged = true;
				}
				else
				{
					float fValuePerTime = (tInfo.m_arrCurve_Size_Axis[EFFECT_INFO::AXIS_X][i] - tInfo.m_arrCurve_Size_Axis[EFFECT_INFO::AXIS_X][i - 1])
						/ (tInfo.m_arrCurve_Size_Axis_Time[EFFECT_INFO::AXIS_X][i] - tInfo.m_arrCurve_Size_Axis_Time[EFFECT_INFO::AXIS_X][i - 1]);

					m_tAtt.vScale.x = (fTimeRatio - tInfo.m_arrCurve_Size_Axis_Time[EFFECT_INFO::AXIS_X][i - 1]) * fValuePerTime + tInfo.m_arrCurve_Size_Axis[EFFECT_INFO::AXIS_X][i - 1];
					m_tAtt.vScale.x *= m_tAtt.vScale_Origin.x;
					bIsChanged = true;
				}
				break;
			}
		}
	}

	// y 축
	if (tInfo.m_arrCurve_Size_Cnt[EFFECT_INFO::AXIS_Y] > 1)
	{
		for (int i = 0; i < tInfo.m_arrCurve_Size_Cnt[EFFECT_INFO::AXIS_Y]; ++i)
		{
			if (fTimeRatio <= tInfo.m_arrCurve_Size_Axis_Time[EFFECT_INFO::AXIS_Y][i])
			{
				// 1. 시간값이 0일 때
				if (tInfo.m_arrCurve_Size_Axis_Time[EFFECT_INFO::AXIS_Y][i] == 0.f)
				{
					m_tAtt.vScale.y = tInfo.m_arrCurve_Size_Axis[EFFECT_INFO::AXIS_Y][i] * m_tAtt.vScale_Origin.y;
					bIsChanged = true;
				}
				else
				{
					float fValuePerTime = (tInfo.m_arrCurve_Size_Axis[EFFECT_INFO::AXIS_Y][i] - tInfo.m_arrCurve_Size_Axis[EFFECT_INFO::AXIS_Y][i - 1])
						/ (tInfo.m_arrCurve_Size_Axis_Time[EFFECT_INFO::AXIS_Y][i] - tInfo.m_arrCurve_Size_Axis_Time[EFFECT_INFO::AXIS_Y][i - 1]);

					m_tAtt.vScale.y = (fTimeRatio - tInfo.m_arrCurve_Size_Axis_Time[EFFECT_INFO::AXIS_Y][i - 1]) * fValuePerTime + tInfo.m_arrCurve_Size_Axis[EFFECT_INFO::AXIS_Y][i - 1];
					m_tAtt.vScale.y *= m_tAtt.vScale_Origin.y;
					bIsChanged = true;
				}
				break;
			}
		}
	}
	// z 축
	if (tInfo.m_arrCurve_Size_Cnt[EFFECT_INFO::AXIS_Z] > 1)
	{
		for (int i = 0; i < tInfo.m_arrCurve_Size_Cnt[EFFECT_INFO::AXIS_Z]; ++i)
		{
			if (fTimeRatio <= tInfo.m_arrCurve_Size_Axis_Time[EFFECT_INFO::AXIS_Z][i])
			{
				// 1. 시간값이 0일 때
				if (tInfo.m_arrCurve_Size_Axis_Time[EFFECT_INFO::AXIS_Z][i] == 0.f)
				{
					m_tAtt.vScale.z = tInfo.m_arrCurve_Size_Axis[EFFECT_INFO::AXIS_Z][i] * m_tAtt.vScale_Origin.z;
					bIsChanged = true;
				}
				else
				{
					float fValuePerTime = (tInfo.m_arrCurve_Size_Axis[EFFECT_INFO::AXIS_Z][i] - tInfo.m_arrCurve_Size_Axis[EFFECT_INFO::AXIS_Z][i - 1])
						/ (tInfo.m_arrCurve_Size_Axis_Time[EFFECT_INFO::AXIS_Z][i] - tInfo.m_arrCurve_Size_Axis_Time[EFFECT_INFO::AXIS_Z][i - 1]);

					m_tAtt.vScale.z = (fTimeRatio - tInfo.m_arrCurve_Size_Axis_Time[EFFECT_INFO::AXIS_Z][i - 1]) * fValuePerTime + tInfo.m_arrCurve_Size_Axis[EFFECT_INFO::AXIS_Z][i - 1];
					m_tAtt.vScale.z *= m_tAtt.vScale_Origin.z;
					bIsChanged = true;
				}
				break;
			}
		}
	}

	if (bIsChanged)
		m_pTransformCom->Set_Infomation(Engine::CTransform::INFO_SCALE, &m_tAtt.vScale);
}

void CMyFX::Curve_Alpha(_float & fTimeRatio, EFFECT_INFO & tInfo)
{
	for (int i = 0; i < tInfo.m_iCurve_Alpha_Cnt; ++i)
	{
		if (fTimeRatio <= tInfo.m_arrCurve_Alpha[EFFECT_INFO::COLUMN_TIME][i])
		{
			// 1. 시간값이 0일 때
			if (tInfo.m_arrCurve_Alpha[EFFECT_INFO::COLUMN_TIME][i] == 0.f)
			{
				m_tAtt.vColor.a = tInfo.m_arrCurve_Alpha[EFFECT_INFO::COLUMN_VALUE][i] * m_tAtt.vColor_Origin.a;
			}
			else
			{
				float fValuePerTime = (tInfo.m_arrCurve_Alpha[EFFECT_INFO::COLUMN_VALUE][i] - tInfo.m_arrCurve_Alpha[EFFECT_INFO::COLUMN_VALUE][i - 1])
					/ (tInfo.m_arrCurve_Alpha[EFFECT_INFO::COLUMN_TIME][i] - tInfo.m_arrCurve_Alpha[EFFECT_INFO::COLUMN_TIME][i - 1]);

				m_tAtt.vColor.a = (fTimeRatio - tInfo.m_arrCurve_Alpha[EFFECT_INFO::COLUMN_TIME][i - 1]) * fValuePerTime + tInfo.m_arrCurve_Alpha[EFFECT_INFO::COLUMN_VALUE][i - 1];
				m_tAtt.vColor.a *= m_tAtt.vColor_Origin.a;
			}
			break;
		}
	}
}

void CMyFX::Curve_Speed(_float & fTimeRatio, EFFECT_INFO & tInfo)
{
	for (int i = 0; i < tInfo.m_iCurve_Spd_Cnt; ++i)
	{
		if (fTimeRatio <= tInfo.m_arrCurve_Spd[EFFECT_INFO::COLUMN_TIME][i])
		{
			// 1. 시간값이 0일 때
			if (tInfo.m_arrCurve_Spd[EFFECT_INFO::COLUMN_TIME][i] == 0.f)
			{
				m_tAtt.vVelocity = tInfo.m_arrCurve_Spd[EFFECT_INFO::COLUMN_VALUE][i] * m_tAtt.vVelociy_Origin;
			}
			else
			{
				float fValuePerTime = (tInfo.m_arrCurve_Spd[EFFECT_INFO::COLUMN_VALUE][i] - tInfo.m_arrCurve_Spd[EFFECT_INFO::COLUMN_VALUE][i - 1])
					/ (tInfo.m_arrCurve_Spd[EFFECT_INFO::COLUMN_TIME][i] - tInfo.m_arrCurve_Spd[EFFECT_INFO::COLUMN_TIME][i - 1]);

				float fDelta = (fTimeRatio - tInfo.m_arrCurve_Spd[EFFECT_INFO::COLUMN_TIME][i - 1]) * fValuePerTime + tInfo.m_arrCurve_Spd[EFFECT_INFO::COLUMN_VALUE][i - 1];
				m_tAtt.vVelocity = m_tAtt.vVelociy_Origin * fDelta;
			}
			break;
		}
	}
}

void CMyFX::Curve_Color(_float & fTimeRatio, EFFECT_INFO & tInfo)
{
	if (tInfo.m_iCurve_R_Cnt > 1)
	{
		for (int i = 0; i < tInfo.m_iCurve_R_Cnt; ++i)
		{
			if (fTimeRatio <= tInfo.m_arrCurve_R[EFFECT_INFO::COLUMN_TIME][i])
			{
				// 1. 시간값이 0일 때
				if (tInfo.m_arrCurve_R[EFFECT_INFO::COLUMN_TIME][i] == 0.f)
				{
					m_tAtt.vColor.r = tInfo.m_arrCurve_R[EFFECT_INFO::COLUMN_VALUE][i] * m_tAtt.vColor_Origin.r;
				}
				else
				{
					float fValuePerTime = (tInfo.m_arrCurve_R[EFFECT_INFO::COLUMN_VALUE][i] - tInfo.m_arrCurve_R[EFFECT_INFO::COLUMN_VALUE][i - 1])
						/ (tInfo.m_arrCurve_R[EFFECT_INFO::COLUMN_TIME][i] - tInfo.m_arrCurve_R[EFFECT_INFO::COLUMN_TIME][i - 1]);

					m_tAtt.vColor.r = (fTimeRatio - tInfo.m_arrCurve_R[EFFECT_INFO::COLUMN_TIME][i - 1]) * fValuePerTime + tInfo.m_arrCurve_R[EFFECT_INFO::COLUMN_VALUE][i - 1];
					m_tAtt.vColor.r *= m_tAtt.vColor_Origin.r;
				}
				break;
			}
		}
	}

	if (tInfo.m_iCurve_G_Cnt > 1)
	{
		for (int i = 0; i < tInfo.m_iCurve_G_Cnt; ++i)
		{
			if (fTimeRatio <= tInfo.m_arrCurve_G[EFFECT_INFO::COLUMN_TIME][i])
			{
				// 1. 시간값이 0일 때
				if (tInfo.m_arrCurve_G[EFFECT_INFO::COLUMN_TIME][i] == 0.f)
				{
					m_tAtt.vColor.g = tInfo.m_arrCurve_G[EFFECT_INFO::COLUMN_VALUE][i] * m_tAtt.vColor_Origin.g;
				}
				else
				{
					float fValuePerTime = (tInfo.m_arrCurve_G[EFFECT_INFO::COLUMN_VALUE][i] - tInfo.m_arrCurve_G[EFFECT_INFO::COLUMN_VALUE][i - 1])
						/ (tInfo.m_arrCurve_G[EFFECT_INFO::COLUMN_TIME][i] - tInfo.m_arrCurve_G[EFFECT_INFO::COLUMN_TIME][i - 1]);

					m_tAtt.vColor.g = (fTimeRatio - tInfo.m_arrCurve_G[EFFECT_INFO::COLUMN_TIME][i - 1]) * fValuePerTime + tInfo.m_arrCurve_G[EFFECT_INFO::COLUMN_VALUE][i - 1];
					m_tAtt.vColor.g *= m_tAtt.vColor_Origin.g;
				}
				break;
			}
		}
	}

	if (tInfo.m_iCurve_B_Cnt > 1)
	{
		for (int i = 0; i < tInfo.m_iCurve_B_Cnt; ++i)
		{
			if (fTimeRatio <= tInfo.m_arrCurve_B[EFFECT_INFO::COLUMN_TIME][i])
			{
				// 1. 시간값이 0일 때
				if (tInfo.m_arrCurve_B[EFFECT_INFO::COLUMN_TIME][i] == 0.f)
				{
					m_tAtt.vColor.b = tInfo.m_arrCurve_B[EFFECT_INFO::COLUMN_VALUE][i] * m_tAtt.vColor_Origin.b;
				}
				else
				{
					float fValuePerTime = (tInfo.m_arrCurve_B[EFFECT_INFO::COLUMN_VALUE][i] - tInfo.m_arrCurve_B[EFFECT_INFO::COLUMN_VALUE][i - 1])
						/ (tInfo.m_arrCurve_B[EFFECT_INFO::COLUMN_TIME][i] - tInfo.m_arrCurve_B[EFFECT_INFO::COLUMN_TIME][i - 1]);

					m_tAtt.vColor.b = (fTimeRatio - tInfo.m_arrCurve_B[EFFECT_INFO::COLUMN_TIME][i - 1]) * fValuePerTime + tInfo.m_arrCurve_B[EFFECT_INFO::COLUMN_VALUE][i - 1];
					m_tAtt.vColor.b *= m_tAtt.vColor_Origin.b;
				}
				break;
			}
		}
	}
}

void CMyFX::Curve_Bright(_float & fTimeRatio, EFFECT_INFO & tInfo)
{
	for (int i = 0; i < tInfo.m_iCurve_Brt_Cnt; ++i)
	{
		if (fTimeRatio <= tInfo.m_arrCurve_BRT[EFFECT_INFO::COLUMN_TIME][i])
		{
			// 1. 시간값이 0일 때
			if (tInfo.m_arrCurve_BRT[EFFECT_INFO::COLUMN_TIME][i] == 0.f)
			{
				tInfo.m_fBright *= tInfo.m_arrCurve_BRT[EFFECT_INFO::COLUMN_VALUE][i] * tInfo.m_fBright_Init;
			}
			else
			{
				float fValuePerTime = (tInfo.m_arrCurve_BRT[EFFECT_INFO::COLUMN_VALUE][i] - tInfo.m_arrCurve_BRT[EFFECT_INFO::COLUMN_VALUE][i - 1])
					/ (tInfo.m_arrCurve_BRT[EFFECT_INFO::COLUMN_TIME][i] - tInfo.m_arrCurve_BRT[EFFECT_INFO::COLUMN_TIME][i - 1]);

				tInfo.m_fBright = (fTimeRatio - tInfo.m_arrCurve_BRT[EFFECT_INFO::COLUMN_TIME][i - 1]) * fValuePerTime + tInfo.m_arrCurve_BRT[EFFECT_INFO::COLUMN_VALUE][i - 1];
				tInfo.m_fBright *= tInfo.m_fBright_Init;
			}
			break;
		}
	}
}

//CMyFX * CMyFX::Clone(void)
//{
//	return new CMyFX(*this);
//}

CMyFX * CMyFX::Create(LPDIRECT3DDEVICE9 pGraphicDev, CMyEffect * pSystem)
{
	CMyFX*	pInstance = new CMyFX(pGraphicDev);
	if (FAILED(pInstance->Ready_GameObject(pSystem)))
	{
		MSG_BOX("CMyFX Create Failed!");
		Engine::Safe_Release(pInstance);
	}

	return pInstance;
}

_ulong CMyFX::Free(void)
{
	_ulong	dwRefCnt = 0;

	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pRandom_Manager);

	dwRefCnt = CGameObject::Free();

	return dwRefCnt;
}

