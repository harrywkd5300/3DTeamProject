#include "..\Headers\MyEffect_Buff.h"
#include "Component_Manager.h"
#include "Camera_Manager.h"
#include "Target_Manager.h"
#include "MyFX.h"

USING(Engine)

CMyEffect_Buff::CMyEffect_Buff(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMyEffect(pGraphicDev)
	, m_pBufferCom(nullptr)
	, m_pCam_Manager(CCamera_Manager::GetInstance())
	, m_fTimeDelta(0.f)
	, m_iTexCnt(0)
	, m_tPtcInfo(PTC_INFO())
{
	ZeroMemory(m_pTextureArr, sizeof(CTexture*) * 5);
	ZeroMemory(m_szSampler, sizeof(char) * 5);

#ifndef _FINAL
	Tag(L"CMyEffect_Buff", std::bind(&CMyEffect_Buff::Debug_Render, this));
#endif
}

CMyEffect_Buff::CMyEffect_Buff(const CMyEffect_Buff & rhs)
	: CMyEffect(rhs)
	, m_pBufferCom(rhs.m_pBufferCom)
	//, m_pBufferCom(nullptr)
	, m_pCam_Manager(CCamera_Manager::GetInstance())
	, m_fTimeDelta(0.f)
	, m_tPtcInfo(rhs.m_tPtcInfo)
	, m_iTexCnt(rhs.m_iTexCnt)
{
	/*if (nullptr != m_pBufferCom)
		m_pBufferCom->AddRef();*/

	for (size_t i = 0; i < 5; ++i) // 다시 확인하기
	{
		if (nullptr != rhs.m_pTextureArr[i])
		{
			m_pTextureArr[i] = rhs.m_pTextureArr[i];
			m_pTextureArr[i]->AddRef();
		}
		else
			m_pTextureArr[i] = nullptr;
	}

	for (size_t i = 0; i < m_iTexCnt; ++i)
		strcpy_s(m_szSampler[i], rhs.m_szSampler[i]);

	//if (FAILED(Ready_Component()))
	//	return;

	if (nullptr != m_pBufferCom)
		m_pBufferCom->AddRef();

	if (FAILED(Init_System()))
		return;

#ifndef _FINAL
	Tag(L"CMyEffect_Buff", std::bind(&CMyEffect_Buff::Debug_Render, this));
#endif
}

void CMyEffect_Buff::Set_TexComponent(CTexture* pTexture, _int& iTexIndex)
{
	if (nullptr == m_pTextureArr[iTexIndex])
	{
		char	szBuff[126] = "g_BlendTexture_";
		char	szIndex[5] = "";

		m_pTextureArr[iTexIndex] = pTexture;

		_itoa_s(iTexIndex, szIndex, 10);

		strcpy_s(m_szSampler[iTexIndex], sizeof(char) * 126, szBuff);
		strcat_s(m_szSampler[iTexIndex], szIndex);

		++m_iTexCnt;
	}
}

void CMyEffect_Buff::Reset_Texture(void)
{
	for (size_t i = 0; i < m_iTexCnt; ++i)
	{
		Engine::Safe_Release(m_pTextureArr[i]);
		strcpy_s(m_szSampler[i], "");

		m_pTextureArr[i] = nullptr;
	}

	m_iTexCnt = 0;
}

void CMyEffect_Buff::Reset_Info(void)
{
	CMyFX* pFx = nullptr;
	for (int i = 0; i < m_listBuffer.Size();)
	{
		pFx = m_listBuffer.Pop_Back();
		pFx->Set_Alive(false);
		m_listRecycles.Push_Back(pFx);
	}

	m_listBuffer.Clear();

	if (FAILED(Init_System()))
		return;
}

void CMyEffect_Buff::Set_ParentMatrix(const D3DXMATRIX * pMatParent)
{
	if (m_tEffInfo.m_eTransType == EFFECT_INFO::TYPE_POINTER)
	{
		CMyFX* pBuffer = nullptr;
		int iSize = m_listBuffer.Size();
		for (int i = 0; i < iSize; ++i)
		{
			pBuffer = m_listBuffer.Get(i);
			pBuffer->Set_ParentMatrix(pMatParent);
		}
		//for (auto& pBuffer : m_listBuffer)
		//		pBuffer->Set_ParentMatrix(pMatParent);
	}
	// Else로 묶지 않은 이유
	// -> Rate_Per_Sec / POINTER 의 경우, 생성되기 전에 부모가 세팅되는 경우가 발생하므로
	// 생성될 때 담아주기 위해서
	m_tPtcInfo.matParent = *pMatParent; 
}

void CMyEffect_Buff::Render_Effect(CMyFX * pFX)
{
	if (nullptr == m_pCam_Manager
		|| nullptr == m_pBufferCom)
		return;

	LPD3DXEFFECT pEffect = m_pShaderCom->Get_EffectHandle();
	if (nullptr == pEffect)
		return;

	SetUp_OnConstantTable(pEffect, pFX);

	_int	iPass = 0;

	//pEffect->Begin(nullptr, 0);

	// 부모 시스템에서 한번만 설정하면 됨
	if (m_tEffInfo.m_ePassType == EFFECT_INFO::PASS_BUFFER)
		iPass = SHADER_EFFECT_BUFFERNORMAL;
	else if (m_tEffInfo.m_ePassType == EFFECT_INFO::PASS_CUTBLACK_SPRITE)
		iPass = SHADER_EFFECT_CUTBLACK;
	else if (m_tEffInfo.m_ePassType == EFFECT_INFO::PASS_MASKING)
		iPass = SHADER_EFFECT_MASK;
	
	pEffect->CommitChanges();
	pEffect->BeginPass(iPass);

	m_pBufferCom->Render_Buffer();

	pEffect->EndPass();
	//pEffect->End();

}

HRESULT CMyEffect_Buff::Ready_GameObject(const EFFECT_INFO& tInfo)
{
	/*if (FAILED(CMyEffect::Ready_Component()))
		return E_FAIL;*/

	m_tEffInfo = tInfo;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	//m_pShaderCom = pShaderCom;
	//if (nullptr != m_pShaderCom)
	//	m_pShaderCom->AddRef();

	if (FAILED(Init_System()))
		return E_FAIL;

	return NOERROR;
}

_int CMyEffect_Buff::Update_GameObject(const _float & fTimeDelta)
{
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	m_fTimeDelta += fTimeDelta;
	m_tPtcInfo.fCurRate += fTimeDelta;

	if (m_tEffInfo.m_eSpawnType == EFFECT_INFO::SPAWN_TYPE::SPAWN_RATE_PER_SEC)
	{
		if (true == m_tPtcInfo.bIsLooping)
		{
			for (int i = 0; i < m_tPtcInfo.fCurRate / (1.f / m_tEffInfo.m_fRate_Emis); ++i)
			{
				Add_Particle();
				m_tPtcInfo.fCurRate -= 1.f / m_tEffInfo.m_fRate_Emis;
			}
		}
		else
		{
			if (m_tEffInfo.m_fDuration > m_fTimeDelta)
			{
				for (int i = 0; i < m_tPtcInfo.fCurRate / (1.f / m_tEffInfo.m_fRate_Emis); ++i)
				{
					Add_Particle();
					m_tPtcInfo.fCurRate -= 1.f / m_tEffInfo.m_fRate_Emis;
				}
			}
		}
	}
	else if (m_tEffInfo.m_eSpawnType = EFFECT_INFO::SPAWN_BURST)
	{
		if (true == m_tPtcInfo.bIsLooping && true == m_listBuffer.Empty())
		{
			for (size_t i = 0; i < m_tPtcInfo.dwMaxCnt; ++i)
				Add_Particle();
		}
		else if (m_tPtcInfo.bIsFirstUpdate)
		{
			for (size_t i = 0; i < m_tPtcInfo.dwMaxCnt; ++i)
				Add_Particle();
		}
	}

	// 파티클 순회하면서 Update 호출
	CMyFX* pBuffer = nullptr;
	for (int i = 0; i < m_listBuffer.Size();)
	{
		pBuffer = m_listBuffer.Get(i);
		_int iReturn = pBuffer->Update_FX(fTimeDelta, m_tEffInfo);

		if (iReturn == -1)
		{
			m_listBuffer.Pop(i);
			pBuffer->Set_Alive(false);
			m_listRecycles.Push_Back(pBuffer);
			continue;
		}
		else
			++i;
		m_tPtcInfo.bIsFirstUpdate = false;
	}

	//LISTBUFFER::iterator		iter = m_listBuffer.begin();
	//for (; iter != m_listBuffer.end();)
	//{
	//	_int iReturn = (*iter)->Update_FX(fTimeDelta, m_tEffInfo);
	//	if (iReturn == -1) // 파티클이 죽은 경우
	//	{
	//		(*iter)->Set_Alive(false);
	//		m_listRecycles.push_back(*iter);
	//		iter = m_listBuffer.erase(iter);
	//		continue;
	//	}
	//	else
	//		++iter;
	//	m_tPtcInfo.bIsFirstUpdate = false;
	//}

	if (false == m_tPtcInfo.bIsLooping
		&& m_fTimeDelta > m_tEffInfo.m_fDuration
		&& m_listBuffer.Empty())
		return 1;

	return 0;
}

HRESULT CMyEffect_Buff::Ready_Component(void)
{
	// For.Buffer
	Engine::CComponent*		pComponent = nullptr;
	pComponent = m_pBufferCom = (Engine::CRect_Texture*)CComponent_Manager::GetInstance()->Clone_Component(0, L"CRect_Texture");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"CRect_Texture", pComponent));
	m_pBufferCom->AddRef();

	pComponent = m_pShaderCom = (Engine::CShader*)CComponent_Manager::GetInstance()->Clone_Component(0, L"CShader_Effect");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"CShader", pComponent));
	m_pShaderCom->AddRef();

	for (int i = 0; i < (int)m_tEffInfo.m_iTexCnt; ++i)
	{
		pComponent = m_pTextureArr[i] = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(0, m_tEffInfo.m_szTexturePath[i]);
		if (nullptr == pComponent)
			return E_FAIL;

		char	szBuff[126] = "g_BlendTexture_";
		char	szIndex[5] = "";

		_itoa_s(i, szIndex, 10);

		strcpy_s(m_szSampler[i], sizeof(char) * 126, szBuff);
		strcat_s(m_szSampler[i], szIndex);

		++m_iTexCnt;
	}

	return NOERROR;
}

HRESULT CMyEffect_Buff::Init_System(void)
{
	if (nullptr == m_pRandomMgr
		|| nullptr == m_pGraphicDev)
		return E_FAIL;

	m_tPtcInfo = PTC_INFO();

	m_tPtcInfo.vOrigin = _vec3(m_tEffInfo.m_fPos_OriginX, m_tEffInfo.m_fPos_OriginY, m_tEffInfo.m_fPos_OriginZ);

	m_tPtcInfo.vMin = m_tEffInfo.m_vPtc_AreaMin;
	m_tPtcInfo.vMax = m_tEffInfo.m_vPtc_AreaMax;

	m_tPtcInfo.bIsLooping = m_tEffInfo.m_bIsLooping;

	int ReserveSize = 0;
	if (m_tEffInfo.m_eSpawnType == EFFECT_INFO::SPAWN_RATE_PER_SEC)
	{
		m_tPtcInfo.fRatePerSec = m_tEffInfo.m_fRate_Emis;
		m_tPtcInfo.dwMaxCnt = (_ulong)(m_tPtcInfo.fRatePerSec * m_tEffInfo.m_fLifeTime_MAX);
		ReserveSize = m_tPtcInfo.dwMaxCnt;
	}
	else
	{
		m_tPtcInfo.dwMaxCnt = (_ulong)(m_pRandomMgr->GetValueF(m_tEffInfo.m_fCnt_Min, m_tEffInfo.m_fCnt_Max));

		if (0 >= m_tPtcInfo.dwMaxCnt)
			return E_FAIL;
		ReserveSize = (int)m_tEffInfo.m_fCnt_Max;
	}

	if (m_listBuffer.Capacity() == 0)
	{
		m_listBuffer.Reserve((int)(ReserveSize*1.2f));
	}
	if (m_listRecycles.Capacity() == 0)
	{
		m_listRecycles.Reserve((int)(ReserveSize*1.2f));
	}
	//if (m_listBuffer.Capacity() != ReserveSize)
	//{
	//	if (m_listBuffer.Capacity() != 0)
	//	{
	//		JWVECTOR<CMyFX*> newVector;
	//		newVector.Reserve(ReserveSize);

	//		CMyFX* pParticle = nullptr;
	//		for (int i = 0; i < ReserveSize; ++i)
	//		{
	//			if (m_listBuffer.Empty())
	//				break;

	//			pParticle = m_listBuffer.Pop_Back();
	//			newVector.Push_Back(pParticle);
	//		}
	//		while (!m_listBuffer.Empty())
	//		{
	//			pParticle = m_listBuffer.Pop_Back();
	//			Safe_Release(pParticle);
	//		}
	//		m_listBuffer = newVector;
	//	}
	//	else
	//	{
	//		m_listBuffer.Reserve(ReserveSize);
	//	}
	//}

	//if (m_listRecycles.Capacity() != ReserveSize)
	//{
	//	if (m_listRecycles.Capacity() != 0)
	//	{
	//		JWVECTOR<CMyFX*> newVector;
	//		newVector.Reserve(ReserveSize);

	//		CMyFX* pParticle = nullptr;
	//		for (int i = 0; i < ReserveSize; ++i)
	//		{
	//			if (m_listRecycles.Empty())
	//				break;

	//			pParticle = m_listRecycles.Pop_Back();
	//			newVector.Push_Back(pParticle);
	//		}
	//		while (!m_listRecycles.Empty())
	//		{
	//			pParticle = m_listRecycles.Pop_Back();
	//			Safe_Release(pParticle);
	//		}
	//		m_listRecycles = newVector;
	//	}
	//	else
	//	{
	//		m_listRecycles.Reserve(ReserveSize);
	//	}
	//}


	m_fTimeDelta = 0.f;
	m_tPtcInfo.fCurRate = 0.f;

	// 2차 Force 값 지정(Random 있음)
	_vec3	vForceMin = _vec3(m_tEffInfo.m_fForce_X, m_tEffInfo.m_fForce_Y, m_tEffInfo.m_fForce_Z);
	_vec3	vForceMax = _vec3(m_tEffInfo.m_fForce_MaxX, m_tEffInfo.m_fForce_MaxY, m_tEffInfo.m_fForce_MaxZ);

	if (vForceMin != vForceMax)
	{
		m_tEffInfo.m_bUseForceRand = true;
		m_tEffInfo.m_bUseAccel = true;
	}
	else
	{
		if (vForceMin == _vec3(0.f, 0.f, 0.f))
			m_tEffInfo.m_bUseAccel = false;
		else
		{
			m_tEffInfo.m_bUseAccel = true;
			m_tEffInfo.m_bUseForceRand = false;
		}
	}

	// 시스템 생성 시에 속성값 활용여부 한번만 검사하도록 해서 해당 속성값 Update 함수 호출 할지말지 결정
	_vec3	vVeloMin = _vec3(m_tEffInfo.m_fVel_MinX, m_tEffInfo.m_fVel_MinY, m_tEffInfo.m_fVel_MinZ);
	_vec3	vVeloMax = _vec3(m_tEffInfo.m_fVel_MaxX, m_tEffInfo.m_fVel_MaxY, m_tEffInfo.m_fVel_MaxZ);

	if (vVeloMin == _vec3(0.f, 0.f, 0.f) && vVeloMax == _vec3(0.f, 0.f, 0.f))
		m_tEffInfo.m_bUseVelocity = false;
	else
		m_tEffInfo.m_bUseVelocity = true;

	if (m_tEffInfo.m_ePtc_Shape == EFFECT_INFO::SHAPE_SPHERE)
	{
		_float fSpeed = m_pRandomMgr->GetValueF(m_tEffInfo.m_fSpeed_Min, m_tEffInfo.m_fSpeed_Max);
		if (0 != fSpeed)
			m_tEffInfo.m_bUseVelocity = true;
		else
			m_tEffInfo.m_bUseVelocity = false;
	}

	_vec3	vAngVelL_Min = _vec3(m_tEffInfo.m_fAVel_L_MinX, m_tEffInfo.m_fAVel_L_MinY, m_tEffInfo.m_fAVel_L_MinZ);
	_vec3	vAngVelL_Max = _vec3(m_tEffInfo.m_fAVel_L_MaxX, m_tEffInfo.m_fAVel_L_MaxY, m_tEffInfo.m_fAVel_L_MaxZ);

	if (vAngVelL_Min == _vec3(0.f, 0.f, 0.f) && vAngVelL_Max == _vec3(0.f, 0.f, 0.f))
		m_tEffInfo.m_bUseAngVelL = false;
	else
		m_tEffInfo.m_bUseAngVelL = true;

	_vec3	vAngVelW_Min = _vec3(m_tEffInfo.m_fAVel_W_MinX, m_tEffInfo.m_fAVel_W_MinY, m_tEffInfo.m_fAVel_W_MinZ);
	_vec3	vAngVelW_Max = _vec3(m_tEffInfo.m_fAVel_W_MaxX, m_tEffInfo.m_fAVel_W_MaxY, m_tEffInfo.m_fAVel_W_MaxZ);

	if (vAngVelW_Min == _vec3(0.f, 0.f, 0.f) && vAngVelW_Max == _vec3(0.f, 0.f, 0.f))
		m_tEffInfo.m_bUseAngVelW = false;
	else
		m_tEffInfo.m_bUseAngVelW = true;

	D3DXMatrixIdentity(&m_tPtcInfo.matParent);

	Set_IsDead(false);

	return NOERROR;
}

void CMyEffect_Buff::Add_Particle(void)
{
	CMyFX* pAtt = nullptr;

	if (m_listRecycles.Empty())
		pAtt = CMyFX::Create(m_pGraphicDev, this);
	else
	{
		pAtt = m_listRecycles.Pop_Back();
	}
	//if (m_listRecycles.empty())
	//	pAtt = CMyFX::Create(m_pGraphicDev, this);
	//else
	//{
	//	pAtt = m_listRecycles.front();
	//	m_listRecycles.pop_front();
	//}

	Set_Attribute(pAtt);

	m_listBuffer.Push_Back(pAtt);
	//m_listBuffer.push_back(pAtt);
}

void CMyEffect_Buff::Set_Attribute(CMyFX * pAtt)
{
	CTransform* pTransform = pAtt->Get_Transform();
	if (nullptr == pTransform)
		return;

	FX_ATT&	tAtt = pAtt->Get_FXInfo();

	// 텍스쳐 정보 변경 ---------------------------------------------------------//
	for (_uint i = 0; i < m_iTexCnt; ++i)
	{
		_float	fSpeedX = m_pRandomMgr->GetValueF(m_tEffInfo.m_vUVSpeed_Min[i].x, m_tEffInfo.m_vUVSpeed_Max[i].x);
		_float	fSpeedY = m_pRandomMgr->GetValueF(m_tEffInfo.m_vUVSpeed_Min[i].y, m_tEffInfo.m_vUVSpeed_Max[i].y);

		tAtt.vUVSpeed[i] = _vec2(fSpeedX, fSpeedY);

		if (m_tEffInfo.m_ePassType == EFFECT_INFO::PASS_CUTBLACK_SPRITE)
		{
			if (m_tEffInfo.m_vSliceUV[i] != _vec2(0.f, 0.f))
			{
				tAtt.vSlice.x = m_tEffInfo.m_vSliceUV[i].x;
				tAtt.vSlice.y = m_tEffInfo.m_vSliceUV[i].y;
				tAtt.fTotalFrame = tAtt.vSlice.x * tAtt.vSlice.y;
				// 스프라이트의 속도는 UVSpeed Min으로 설정하자
				tAtt.fSpriteSpd = m_tEffInfo.m_vUVSpeed_Min[i].y - m_tEffInfo.m_vUVSpeed_Min[i].x;
			}
			else
			{
				tAtt.vSlice.x = 1.f;
				tAtt.vSlice.y = 1.f;
				tAtt.fTotalFrame = tAtt.vSlice.x * tAtt.vSlice.y;
				tAtt.fSpriteSpd = 1.f;
			}
		}
	}

	if (m_tEffInfo.m_ePtc_Shape == EFFECT_INFO::SHAPE_SPHERE)
	{
		_vec3	vOffset = _vec3(m_pRandomMgr->GetValueF(-1.f, 1.f), m_pRandomMgr->GetValueF(-1.f, 1.f), m_pRandomMgr->GetValueF(-1.f, 1.f));

		_vec3	vDir = *D3DXVec3Normalize(&vOffset, &vOffset) * m_tEffInfo.m_fPtc_Radius;

		tAtt.vPosition = m_tPtcInfo.vOrigin + vDir;
		tAtt.vVelocity = vOffset * m_pRandomMgr->GetValueF(m_tEffInfo.m_fSpeed_Min, m_tEffInfo.m_fSpeed_Max);

		//if (m_tEffInfo.m_eTransType != EFFECT_INFO::TYPE_POINTER)
		D3DXVec3TransformNormal(&tAtt.vVelocity, &tAtt.vVelocity, &m_tPtcInfo.matParent);

		tAtt.vVelociy_Origin = tAtt.vVelocity;

	}
	else if (m_tEffInfo.m_ePtc_Shape == EFFECT_INFO::SHAPE_CONE)
	{

	}
	else
	{
		// 로컬 상의 위치 ------------------------------------------------------------------------------//

		_vec3	vPos = _vec3(m_tEffInfo.m_fPos_OriginX, m_tEffInfo.m_fPos_OriginY, m_tEffInfo.m_fPos_OriginZ);

		_float	fOffsetX = m_pRandomMgr->GetValueF(m_tEffInfo.m_fOffset_MinX, m_tEffInfo.m_fOffset_MaxX);
		_float	fOffsetY = m_pRandomMgr->GetValueF(m_tEffInfo.m_fOffset_MinY, m_tEffInfo.m_fOffset_MaxY);
		_float	fOffsetZ = m_pRandomMgr->GetValueF(m_tEffInfo.m_fOffset_MinZ, m_tEffInfo.m_fOffset_MaxZ);

		tAtt.vPosition = vPos + _vec3(fOffsetX, fOffsetY, fOffsetZ);
		pTransform->Set_Infomation(Engine::CTransform::INFO_POSITION, &tAtt.vPosition);

		// 속도값 ---------------------------------------------------------------------------------------//

		_float fVelX = m_pRandomMgr->GetValueF(m_tEffInfo.m_fVel_MinX, m_tEffInfo.m_fVel_MaxX);
		_float fVelY = m_pRandomMgr->GetValueF(m_tEffInfo.m_fVel_MinY, m_tEffInfo.m_fVel_MaxY);
		_float fVelZ = m_pRandomMgr->GetValueF(m_tEffInfo.m_fVel_MinZ, m_tEffInfo.m_fVel_MaxZ);

		tAtt.vVelocity = tAtt.vVelociy_Origin = _vec3(fVelX, fVelY, fVelZ);
		//D3DXVec3TransformNormal(&tAtt.vVelocity, &tAtt.vVelocity, &m_tPtcInfo.matParent);
		tAtt.vVelociy_Origin = tAtt.vVelocity;

		tAtt.fSpeed = m_pRandomMgr->GetValueF(m_tEffInfo.m_fSpeed_Min, m_tEffInfo.m_fSpeed_Max);

	}

	if (m_tEffInfo.m_eTransType == EFFECT_INFO::TYPE_POS_UPDATED) // POS_FIXED는 애초에 회전행렬 포함X해서 전달됨
	{
		D3DXMATRIX	matTrans;
		D3DXMatrixTranslation(&matTrans, m_tPtcInfo.matParent.m[3][0], m_tPtcInfo.matParent.m[3][1], m_tPtcInfo.matParent.m[3][2]);

		m_tPtcInfo.matParent = matTrans;
		//pTransform->Set_ParentMatrix(&matTrans);
	}
	//else
		//pTransform->Set_ParentMatrix(&m_tPtcInfo.matParent);


	// Force -----------------------------------------------------------------------------------------------//

	_vec3	vForceMin = _vec3(m_tEffInfo.m_fForce_X, m_tEffInfo.m_fForce_Y, m_tEffInfo.m_fForce_Z);
	_vec3	vForceMax = _vec3(m_tEffInfo.m_fForce_MaxX, m_tEffInfo.m_fForce_MaxY, m_tEffInfo.m_fForce_MaxZ);

	tAtt.vAcceleration = vForceMin;

	// 로컬 상의 크기 -----------------------------------------------------------------------------------//

	tAtt.vScale = _vec3(m_tEffInfo.m_fScale_OriginX, m_tEffInfo.m_fScale_OriginY, m_tEffInfo.m_fScale_OriginZ);

	if (false == m_tEffInfo.m_bBuffSize_Scalar)
	{
		_float	fScaleX = m_pRandomMgr->GetValueF(m_tEffInfo.m_fScale_MinX, m_tEffInfo.m_fScale_MaxX);
		_float	fScaleY = m_pRandomMgr->GetValueF(m_tEffInfo.m_fScale_MinY, m_tEffInfo.m_fScale_MaxY);
		_float	fScaleZ = m_pRandomMgr->GetValueF(m_tEffInfo.m_fScale_MinZ, m_tEffInfo.m_fScale_MaxZ);

		tAtt.vScale.x *= fScaleX;
		tAtt.vScale.y *= fScaleY;
		tAtt.vScale.z *= fScaleZ;
	}
	else
	{
		// BuffSize_Scalar 옵션이 켜진 경우, 파티클 배율 사이즈값을 활용
		tAtt.vScale *= m_pRandomMgr->GetValueF(m_tEffInfo.m_fPtc_SizeMin, m_tEffInfo.m_fPtc_SizeMax);
	}

	tAtt.vScale_Origin = tAtt.vScale;
	pTransform->Set_Infomation(Engine::CTransform::INFO_SCALE, &tAtt.vScale);


	// 회전값 ----------------------------------------------------------------------------------------//

	_float	fRotLocalX = m_pRandomMgr->GetValueF(m_tEffInfo.m_fRotL_MinX, m_tEffInfo.m_fRotL_MaxX);
	_float	fRotLocalY = m_pRandomMgr->GetValueF(m_tEffInfo.m_fRotL_MinY, m_tEffInfo.m_fRotL_MaxY);
	_float	fRotLocalZ = m_pRandomMgr->GetValueF(m_tEffInfo.m_fRotL_MinZ, m_tEffInfo.m_fRotL_MaxZ);

	_float	fRotWorldX = m_pRandomMgr->GetValueF(m_tEffInfo.m_fRotW_MinX, m_tEffInfo.m_fRotW_MaxX);
	_float	fRotWorldY = m_pRandomMgr->GetValueF(m_tEffInfo.m_fRotW_MinY, m_tEffInfo.m_fRotW_MaxY);
	_float	fRotWorldZ = m_pRandomMgr->GetValueF(m_tEffInfo.m_fRotW_MinZ, m_tEffInfo.m_fRotW_MaxZ);

	tAtt.vRotLocal = _vec3(fRotLocalX, fRotLocalY, fRotLocalZ);
	tAtt.vRotWorld = _vec3(fRotWorldX, fRotWorldY, fRotWorldZ);

	pTransform->Set_Infomation(Engine::CTransform::INFO_ANGLE, &tAtt.vRotLocal);
	D3DXMATRIX	matRot;
	D3DXMatrixRotationYawPitchRoll(&matRot, D3DXToRadian(tAtt.vRotWorld.y), D3DXToRadian(tAtt.vRotWorld.x), D3DXToRadian(tAtt.vRotWorld.z));

	// 각속도 ------------------------------------------------------------------------//
	_float	fAngVelLclX = m_pRandomMgr->GetValueF(m_tEffInfo.m_fAVel_L_MinX, m_tEffInfo.m_fAVel_L_MaxX);
	_float	fAngVelLclY = m_pRandomMgr->GetValueF(m_tEffInfo.m_fAVel_L_MinY, m_tEffInfo.m_fAVel_L_MaxY);
	_float	fAngVelLclZ = m_pRandomMgr->GetValueF(m_tEffInfo.m_fAVel_L_MinZ, m_tEffInfo.m_fAVel_L_MaxZ);

	_float	fAngVelWldX = m_pRandomMgr->GetValueF(m_tEffInfo.m_fAVel_W_MinX, m_tEffInfo.m_fAVel_W_MaxX);
	_float	fAngVelWldY = m_pRandomMgr->GetValueF(m_tEffInfo.m_fAVel_W_MinY, m_tEffInfo.m_fAVel_W_MaxY);
	_float	fAngVelWldZ = m_pRandomMgr->GetValueF(m_tEffInfo.m_fAVel_W_MinZ, m_tEffInfo.m_fAVel_W_MaxZ);

	tAtt.vAngVelLocal = _vec3(fAngVelLclX, fAngVelLclY, fAngVelLclZ);
	tAtt.vAngVelWorld = _vec3(fAngVelWldX, fAngVelWldY, fAngVelWldZ);

	m_tPtcInfo.matParent = matRot * m_tPtcInfo.matParent;

	// 라이프타임 -----------------------------------------------------------//

	tAtt.fAge = 0.f;
	tAtt.fLifeTime = m_pRandomMgr->GetValueF(m_tEffInfo.m_fLifeTime_Min, m_tEffInfo.m_fLifeTime_MAX); 

	// 부모세팅 -> POINTER일때는 Set_Parent 함수에서
	//tAtt.matParent = m_tPtcInfo.matParent;

	// 컬러 -----------------------------------------------------------------//
	float	fColorR = m_pRandomMgr->GetValueF(m_tEffInfo.m_fCol_MinR, m_tEffInfo.m_fCol_MaxR);
	float	fColorG = m_pRandomMgr->GetValueF(m_tEffInfo.m_fCol_MinG, m_tEffInfo.m_fCol_MaxG);
	float	fColorB = m_pRandomMgr->GetValueF(m_tEffInfo.m_fCol_MinB, m_tEffInfo.m_fCol_MaxB);
	float	fColorA = m_pRandomMgr->GetValueF(m_tEffInfo.m_fCol_MinA, m_tEffInfo.m_fCol_MaxA);

	tAtt.vColor = tAtt.vColor_Origin = D3DXCOLOR(fColorR / 255.f, fColorG / 255.f, fColorB / 255.f, fColorA / 255.f);

	pTransform->Set_ParentMatrix(&m_tPtcInfo.matParent);
}

void CMyEffect_Buff::SetUp_OnConstantTable(LPD3DXEFFECT pEffect, CMyFX* pFX)
{
	pEffect->AddRef();

	CTransform* pTransform = pFX->Get_Transform();
	if (nullptr == pTransform)
		return;

	FX_ATT	tAtt = pFX->Get_FXInfo();

	pTransform->SetUp_OnShader(pEffect, "g_matWorld");

	_matrix		matView, matProj;
	matView = m_pCam_Manager->Get_ViewMatrix();
	matProj = m_pCam_Manager->Get_ProjMatrix();

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	// 예외처리 수정필요?
	if (nullptr == m_pTextureArr[0])
	{
		Engine::Safe_Release(pEffect);
		return;
	}

	pEffect->GetVectorArray("g_vUseColor", m_vDefault, 5);
	pEffect->SetVector("g_vColor", &_vec4(tAtt.vColor));
	pEffect->SetFloat("g_fBuff_Bright", m_tEffInfo.m_fBright);

	if (m_tEffInfo.m_ePassType == EFFECT_INFO::PASS_CUTBLACK_SPRITE)
	{
		if ((int)tAtt.vSlice.x == 0 || (int)tAtt.vSlice.y == 0)
			return;

		m_pTextureArr[0]->SetUp_OnShader(pEffect, "g_BaseTexture", (int)tAtt.fCurFrame);

		pEffect->SetFloat("g_fSliceX", tAtt.vSlice.x);
		pEffect->SetFloat("g_fSliceY", tAtt.vSlice.y);

		tAtt.vUVOffset.x = ((int)tAtt.fCurFrame % (int)tAtt.vSlice.x) / tAtt.vSlice.x;
		tAtt.vUVOffset.y = ((int)tAtt.fCurFrame / (int)tAtt.vSlice.x) / tAtt.vSlice.y;

		pEffect->SetFloatArray("g_vUVOffset", tAtt.vUVOffset, 2);
	} 
	else if (m_tEffInfo.m_ePassType == EFFECT_INFO::PASS_BUFFER)
	{
		m_pTextureArr[0]->SetUp_OnShader(pEffect, "g_BufferTexture");
	}
	else if (m_tEffInfo.m_ePassType == EFFECT_INFO::PASS_MASKING)
	{
		D3DXHANDLE	hUseColor, hUVSpeed;

		for (_uint i = 0; i < m_tEffInfo.m_iTexCnt; ++i)
		{
			// 텍스쳐 세팅
			m_pTextureArr[i]->SetUp_OnShader(pEffect, m_szSampler[i]);
			hUseColor = pEffect->GetParameterElement("g_vUseColor", i);
			pEffect->SetVector(hUseColor, &m_vUseColor[m_tEffInfo.m_eTex_Type[i]]);

			// UV Speed 세팅
			hUVSpeed = pEffect->GetParameterElement("g_vUVSpeed", i);
			pEffect->SetFloatArray(hUVSpeed, tAtt.vUVSpeed[i] * m_fTimeDelta, 2);
		}
	}

	CTarget_Manager::GetInstance()->SetUp_OnShader(pEffect, L"RenderTarget_Depth", "g_DepthTexture");

	Engine::Safe_Release(pEffect);
}

CMyEffect_Buff * CMyEffect_Buff::Create(LPDIRECT3DDEVICE9 pGraphicDev, const EFFECT_INFO& tInfo)
{
	CMyEffect_Buff*		pInstance = new CMyEffect_Buff(pGraphicDev);

	if ((FAILED(pInstance->Ready_GameObject(tInfo))))
	{
		MSG_BOX("CMyEffect_Buff Create Failed");
		Engine::Safe_Release(pInstance);
	}

	return pInstance;
}

CMyEffect * CMyEffect_Buff::Clone(void)
{
	return new CMyEffect_Buff(*this);
}
#ifndef _FINAL
void CMyEffect_Buff::Debug_Render()
{
	size_t listSize = m_listBuffer.Size();
	size_t listCapacity = m_listBuffer.Capacity();
	size_t listSizeRecycle = m_listRecycles.Size();
	size_t listCapacityRecycle = m_listRecycles.Capacity();
	CDebug::GetInstance()->Add_DebugText("CMyEffect_Buff");
	CDebug::GetInstance()->Add_DebugText("size:%d", listSize);
	CDebug::GetInstance()->Add_DebugText("Capacity:%d", listCapacity); 
	CDebug::GetInstance()->Add_DebugText("size(Re):%d", listSizeRecycle);
	CDebug::GetInstance()->Add_DebugText("Capacity(Re):%d", listCapacityRecycle);
	CDebug::GetInstance()->Add_DebugText(">>>  Donw (VK_DOWN) / UP (VK_UP)  <<<");

	int Count = 0;

	//for (auto pBuffer : m_listBuffer)
	//{
	CMyFX* pBuffer = nullptr;
	for(int i=0; i<m_listBuffer.Size();++i)
	{
		pBuffer = m_listBuffer.Get(i);

		if (iStartCount > Count)
		{
			++Count;
			continue;
		}
		//CDebug::GetInstance()->Add_DebugText(L"%c) %s", CDebug::DisplayKey[Count - iStartCount], pParticle);
		//if (CInput_Device::GetInstance()->Get_DownKeyState(CDebug::Key[Count]))
		//if (GetAsyncKeyState(CDebug::Key[Count - iStartCount]) & 0x0001)
		//{
		//   iStartCount = 0;
		//   //pParticle->Set_LeafNode();
		//   CDebug::GetInstance()->Set_CurDebug(std::bind(&CComponent::Debug_Render, pLayer.second), pLayer.second);
		//}
		CDebug::GetInstance()->Add_DebugText("------ %d -----", Count);
		//CDebug::GetInstance()->Add_DebugText(Count,"Count");
		//if (pBuffer->Get_FXInfo()->)
		//   CDebug::GetInstance()->Add_DebugText("Alive");
		//else
		//   CDebug::GetInstance()->Add_DebugText("Die");

		CDebug::GetInstance()->Add_DebugText(pBuffer->Get_FXInfo().fAge, "fAge");
		CDebug::GetInstance()->Add_DebugText(pBuffer->Get_FXInfo().fLifeTime, "LifeTime");
		CDebug::GetInstance()->Add_DebugText(pBuffer->Get_FXInfo().vColor.a, "Alpha");
		CDebug::GetInstance()->Add_DebugText(pBuffer->Get_FXInfo().vColor.a * 255.f, "Alpha Color");
		//CDebug::GetInstance()->Add_DebugText(pBuffer->Get_FXInfo().fSize, "Size");
		//CDebug::GetInstance()->Add_DebugText(pBuffer->Get_FXInfo().fSize_Origin, "Origin Size");
		CDebug::GetInstance()->Add_DebugText("Position: %f / %f / %f", pBuffer->Get_FXInfo().vPosition.x, pBuffer->Get_FXInfo().vPosition.y, pBuffer->Get_FXInfo().vPosition.z);
		CDebug::GetInstance()->Add_DebugText("Origin Velocity: %f / %f / %f", pBuffer->Get_FXInfo().vVelociy_Origin.x, pBuffer->Get_FXInfo().vVelociy_Origin.y, pBuffer->Get_FXInfo().vVelociy_Origin.z);
		CDebug::GetInstance()->Add_DebugText("Velocity: %f / %f / %f", pBuffer->Get_FXInfo().vVelocity.x, pBuffer->Get_FXInfo().vVelocity.y, pBuffer->Get_FXInfo().vVelocity.z);
		CDebug::GetInstance()->Add_DebugText(D3DXVec3Length(&pBuffer->Get_FXInfo().vVelocity), "Speed");
		CDebug::GetInstance()->Add_DebugText(pBuffer->Get_FXInfo().vColor.r, "R");
		CDebug::GetInstance()->Add_DebugText(pBuffer->Get_FXInfo().vColor.r * 255.f, "R Color");
		CDebug::GetInstance()->Add_DebugText(m_tEffInfo.m_fBright, "Bright");

		if (pBuffer->Get_Transform() != nullptr)
		{
			pBuffer->Get_Transform()->Debug_Render();
		}

		if (GetAsyncKeyState(VK_DOWN) & 0x0001)
		{
			iStartCount += 1;
			if (iStartCount >= (int)listSize)
				iStartCount = (int)listSize - 1;
		}
		if (GetAsyncKeyState(VK_UP) & 0x0001)
		{
			iStartCount -= 1;
			if (iStartCount < 0)
				iStartCount = 0;
		}

		++Count;
	}
}
#endif
_ulong CMyEffect_Buff::Free(void)
{
	_ulong		dwRefCnt = 0;

	Engine::Safe_Release(m_pBufferCom);

	for (int i = 0; i < 5; ++i)
		Engine::Safe_Release(m_pTextureArr[i]);

	CMyFX* pParticle = nullptr;
	while (!m_listBuffer.Empty())
	{
		pParticle = m_listBuffer.Pop_Back();
		Safe_Release(pParticle);
	}
	m_listBuffer.Clear();

	while (!m_listRecycles.Empty())
	{
		pParticle = m_listRecycles.Pop_Back();
		Safe_Release(pParticle);
	}
	m_listRecycles.Clear();

	//for (auto& pParticle : m_listBuffer)
	//	Engine::Safe_Release(pParticle);

	//for (auto& pRecycle : m_listRecycles)
	//	Engine::Safe_Release(pRecycle);

	//m_listBuffer.clear();
	//m_listRecycles.clear();

	dwRefCnt = CMyEffect::Free();

	return dwRefCnt;
}
