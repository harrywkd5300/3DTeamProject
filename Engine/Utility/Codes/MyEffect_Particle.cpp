 #include "..\Headers\MyEffect_Particle.h"
#include "Texture.h"
#include "Component_Manager.h"
#include "Camera_Manager.h"
#include "Target_Manager.h"

USING(Engine)

CMyEffect_Particle::CMyEffect_Particle(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMyEffect(pGraphicDev)
	, m_pTextureCom(nullptr)
	, m_pCam_Manager(CCamera_Manager::GetInstance())
	, m_pRendererCom(nullptr)
	, m_tPtcInfo(PTC_INFO())
	, m_iViewportHeight(0)
	, m_fTimeDelta(0.f)
	, m_pVB(nullptr)
{
	//m_pCam_Manager->AddRef();
	m_listParticles.Clear();
	m_listRecycles.Clear();
#ifndef _FINAL
	Tag(L"CMyEffect_Particle", std::bind(&CMyEffect_Particle::Debug_Render, this));
#endif
}

CMyEffect_Particle::CMyEffect_Particle(const CMyEffect_Particle & rhs)
	: CMyEffect(rhs)
	, m_pTextureCom(rhs.m_pTextureCom)
	, m_pCam_Manager(rhs.m_pCam_Manager)
	, m_pRendererCom(rhs.m_pRendererCom)
	, m_tPtcInfo(rhs.m_tPtcInfo)
	, m_iViewportHeight(rhs.m_iViewportHeight)
	, m_fTimeDelta(0.f)
	//, m_pVB(rhs.m_pVB)
	, m_pVB(nullptr)
	//, m_listParticles(nullptr)
	//, m_listRecycles(nullptr)
{
	m_listParticles.Clear();
	m_listRecycles.Clear();

	if (nullptr != m_pTextureCom)
		m_pTextureCom->AddRef();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->AddRef();

	if (nullptr != m_pVB)
		m_pVB->AddRef();

	//if (FAILED(CMyEffect::Ready_Component()))
	//	return;

	if (FAILED(Init_System()))
		return;

	if (FAILED(m_pGraphicDev->CreateVertexBuffer(m_tPtcInfo.dwSize * sizeof(VTXPTC), D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY
		, VTXPTC::FVF, D3DPOOL_DEFAULT, &m_pVB, 0)))
		return;

#ifndef _FINAL
	Tag(L"CMyEffect_Particle", std::bind(&CMyEffect_Particle::Debug_Render, this));
#endif
}

void CMyEffect_Particle::Set_TexComponent(CTexture* pTexture)
{
	if (nullptr != m_pTextureCom)
		Engine::Safe_Release(m_pTextureCom);

	m_pTextureCom = pTexture;
}

void CMyEffect_Particle::Reset_Info(void)
{
	PTC_ATTRIBUTE* pAtt = nullptr;
	int iSize = m_listParticles.Size();
	for(int i=0; i<iSize; ++i)
	{
		pAtt = m_listParticles.Pop_Back();
		pAtt->bIsAlive = false;
		m_listRecycles.Push_Back(pAtt);
	}

	if (FAILED(Init_System()))
		return;
}

void CMyEffect_Particle::Set_ParentMatrix(const D3DXMATRIX * pMatParent)
{
	//for (auto& pAtt : m_listParticles)
	//	pAtt->matParent = *pMatParent;

	m_tPtcInfo.matParent = *pMatParent;
}

HRESULT CMyEffect_Particle::Ready_GameObject(const EFFECT_INFO& tInfo)
{
	//if (FAILED(CMyEffect::Ready_Component()))
	//	return E_FAIL;

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

_int CMyEffect_Particle::Update_GameObject(const _float & fTimeDelta)
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

			//m_tPtcInfo.dwSize = m_dwMaxCnt = m_fRate / (1.f / m_tEffInfo.m_fRate_Emis);
			//m_tPtcInfo.dwOffset = 0;

			//if (m_dwMaxCnt > 300) // 세그먼트 제한수
			//	m_tPtcInfo.dwBatchSize = 300;
			//else
			//	m_tPtcInfo.dwBatchSize = m_dwMaxCnt;
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

				//m_tPtcInfo.dwSize = m_dwMaxCnt = m_fRate / (1.f / m_tEffInfo.m_fRate_Emis);
				//m_tPtcInfo.dwOffset = 0;

				//if (m_dwMaxCnt > 300) // 세그먼트 제한수
				//	m_tPtcInfo.dwBatchSize = 300;
				//else
				//	m_tPtcInfo.dwBatchSize = m_dwMaxCnt;
			}
		}
	}
	else if (m_tEffInfo.m_eSpawnType = EFFECT_INFO::SPAWN_BURST)
	{
		if (true == m_tPtcInfo.bIsLooping && m_listParticles.Empty())
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
	
	//LISTPARTICLES::iterator		iter = m_listParticles.begin();
	//LISTPARTICLES::iterator		iter_end = m_listParticles.end();
	
	PTC_ATTRIBUTE* pAtt = nullptr;

	for (int i = 0; i < m_listParticles.Size();)
	{
		pAtt = m_listParticles.Get(i);
		if(pAtt==nullptr)
			continue;

		if (m_tEffInfo.m_bUseForceRand)
			pAtt->vAcceleration = D3DXVECTOR3(m_pRandomMgr->GetValueF(m_tEffInfo.m_fForce_X, m_tEffInfo.m_fForce_MaxX), m_pRandomMgr->GetValueF(m_tEffInfo.m_fForce_Y, m_tEffInfo.m_fForce_MaxY), m_pRandomMgr->GetValueF(m_tEffInfo.m_fForce_Z, m_tEffInfo.m_fForce_MaxZ));

		float	fTimeRatio = pAtt->fAge / pAtt->fLifeTime;

		if (m_tEffInfo.m_iCurve_Spd_Cnt > 1)
			Curve_Speed(pAtt, fTimeRatio);

		pAtt->vVelocity += pAtt->vAcceleration * fTimeDelta;
		pAtt->vPosition += pAtt->vVelocity * fTimeDelta;

		// 커브
		if(m_tEffInfo.m_iCurve_Alpha_Cnt > 1)
			Curve_Alpha(pAtt, fTimeRatio);
		if (m_tEffInfo.m_iCurve_Size_Cnt > 1)
			Curve_Size(pAtt, fTimeRatio);

		Curve_Color(pAtt, fTimeRatio);

		if (m_tEffInfo.m_iCurve_Brt_Cnt > 1)
			Curve_Bright(pAtt, fTimeRatio);

		pAtt->fAge += fTimeDelta;

		if (pAtt->fAge > pAtt->fLifeTime)
		{
			pAtt->bIsAlive = false;
			m_listRecycles.Push_Back(pAtt);
			m_listParticles.Pop(i);
			continue;
		}

		m_tPtcInfo.bIsFirstUpdate = false;

		if (true == m_tPtcInfo.bIsLooping) //  재활용 여부 결정
		{
			// 파티클 생존 범위 지정
			if (false == Is_Inside(pAtt->vPosition))
			{
				pAtt->bIsAlive = false;
				m_listRecycles.Push_Back(pAtt);
				m_listParticles.Pop(i);
				continue;
			}
		}
		++i;
	}

	if (false == m_tPtcInfo.bIsLooping
		&& m_fTimeDelta > m_tEffInfo.m_fDuration
		&& m_listParticles.Empty())
		return 1;

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_ALPHA, this);


	return 0;
}

void CMyEffect_Particle::Render_GameObject(void)
{

#ifndef _FINAL
	float fTime = 0.f;
	bool bErr = false;
	CHECK_TIME_START
#endif

	if (nullptr == m_pCam_Manager
		|| nullptr == m_pShaderCom
		|| nullptr == m_pGraphicDev)
		return;

	LPD3DXEFFECT	pEffect = m_pShaderCom->Get_EffectHandle();
	if (nullptr == pEffect)
		return;

	SetUp_OnConstantTable(pEffect);

	//pEffect->Begin(nullptr, 0);
	pEffect->CommitChanges();
	pEffect->BeginPass(SHADER_EFFECT_PARTICLE);

	if (!m_listParticles.Empty())
		Render_Buffer();

	pEffect->EndPass();
	//pEffect->End();

#ifndef _FINAL
	CHECK_TIME_END(fTime, bErr);
	CDebug::GetInstance()->Add_EffectRunTime(fTime);
#endif
}

HRESULT CMyEffect_Particle::Ready_Component(void)
{
	// For.Renderer
	// 원래 시스템 클래스는 렌더러가 필요없지만 DX파티클 사용하기 때문에 예외
	Engine::CComponent*		pComponent = nullptr;

	pComponent = m_pRendererCom = (Engine::CRenderer*)CComponent_Manager::GetInstance()->Clone_Component(0, L"CRenderer");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"CRenderer", pComponent));
	m_pRendererCom->AddRef();

	pComponent = m_pShaderCom = (Engine::CShader*)CComponent_Manager::GetInstance()->Clone_Component(0, L"CShader_Effect");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"CShader", pComponent));
	m_pShaderCom->AddRef();

	if (lstrcmp(m_tEffInfo.m_szTexturePath[0], L""))
	{
		pComponent = m_pTextureCom = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(0, m_tEffInfo.m_szTexturePath[0]);
		if (nullptr == pComponent)
			return E_FAIL;
	}
	
	if (FAILED(m_pGraphicDev->CreateVertexBuffer(PARTICLE_BUFFER_SIZE * sizeof(VTXPTC), D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY
		, VTXPTC::FVF, D3DPOOL_DEFAULT, &m_pVB, 0)))
	{
		MSG_BOX("<Failed> Init_ParticleSystem");
		return E_FAIL;
	}

	return NOERROR;
}

HRESULT CMyEffect_Particle::Init_System(void)
{
	if (nullptr == m_pRandomMgr
		|| nullptr == m_pGraphicDev)
		return E_FAIL;

	// 파티클 구조체 초기화
	m_tPtcInfo = PTC_INFO();

	m_tPtcInfo.vOrigin = _vec3(m_tEffInfo.m_fPos_OriginX, m_tEffInfo.m_fPos_OriginY, m_tEffInfo.m_fPos_OriginZ);

	m_tPtcInfo.vMin = m_tEffInfo.m_vPtc_AreaMin;
	m_tPtcInfo.vMax = m_tEffInfo.m_vPtc_AreaMax;

	m_tPtcInfo.bIsLooping = m_tEffInfo.m_bIsLooping;

	_ulong ReserveSize = 0;
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
		ReserveSize = m_tEffInfo.m_fCnt_Max;
	}

	m_tPtcInfo.dwSize = PARTICLE_BUFFER_SIZE;
	m_tPtcInfo.dwOffset = 0;

	m_tPtcInfo.dwBatchSize = PARTICLE_BATCH_SIZE;

	if (m_listParticles.Capacity() == 0)
	{
		m_listParticles.Reserve((int)(ReserveSize*1.2f));
	}
	if (m_listRecycles.Capacity() == 0)
	{
		m_listRecycles.Reserve((int)(ReserveSize*1.2f));
	}
	//if (m_listParticles.Capacity() != ReserveSize)
	//{
	//	if (m_listParticles.Capacity() != 0)
	//	{
	//		JWVECTOR<PTC_ATTRIBUTE*> newVector;
	//		newVector.Reserve(ReserveSize);

	//		PTC_ATTRIBUTE* pParticle = nullptr;
	//		for (int i = 0; i < ReserveSize; ++i)
	//		{
	//			if (m_listParticles.Empty())
	//				break;

	//			pParticle = m_listParticles.Pop_Back();
	//			newVector.Push_Back(pParticle);
	//		}
	//		while (!m_listParticles.Empty())
	//		{
	//			pParticle = m_listParticles.Pop_Back();
	//			Safe_Delete(pParticle);
	//		}
	//		m_listParticles = newVector;
	//	}
	//}

	//if (m_listRecycles.Capacity() != ReserveSize)
	//{
	//	if (m_listRecycles.Capacity() != 0)
	//	{
	//		JWVECTOR<PTC_ATTRIBUTE*> newVector;
	//		newVector.Reserve(ReserveSize);

	//		PTC_ATTRIBUTE* pParticle = nullptr;
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
	//			Safe_Delete(pParticle);
	//		}
	//		m_listRecycles = newVector;
	//	}
	//}

	// 파티클 사이즈 계산을 위한 뷰포트 높이값 구하기
	D3DVIEWPORT9	ViewPort;
	ZeroMemory(&ViewPort, sizeof(D3DVIEWPORT9));

	m_pGraphicDev->GetViewport(&ViewPort);

	m_iViewportHeight = ViewPort.Height;

	m_fTimeDelta = 0.f;
	m_tPtcInfo.fCurRate = 0.f;

	// 2차 Force 값 지정(Random 있음)
	_vec3	vForceMin = _vec3(m_tEffInfo.m_fForce_X, m_tEffInfo.m_fForce_Y, m_tEffInfo.m_fForce_Z);
	_vec3	vForceMax = _vec3(m_tEffInfo.m_fForce_MaxX, m_tEffInfo.m_fForce_MaxY, m_tEffInfo.m_fForce_MaxZ);

	if (vForceMin != vForceMax)
		m_tEffInfo.m_bUseForceRand = true;
	else
		m_tEffInfo.m_bUseForceRand = false;

	D3DXMatrixIdentity(&m_tPtcInfo.matParent);

	Set_IsDead(false);

	return NOERROR;
}


void CMyEffect_Particle::Add_Particle(void)
{
	PTC_ATTRIBUTE* pAtt = nullptr;

	if (m_listRecycles.Empty())
	{
		pAtt = new PTC_ATTRIBUTE;
		//ZeroMemory(pAtt, sizeof(PTC_ATTRIBUTE));
	}
	else
	{
		pAtt = m_listRecycles.Pop_Back();
	}
	
	Set_Attribute(pAtt);

	m_listParticles.Push_Back(pAtt);
}

void CMyEffect_Particle::Set_Attribute(PTC_ATTRIBUTE * pAtt)
{
	if (m_tEffInfo.m_ePtc_Shape == EFFECT_INFO::SHAPE_SPHERE)
	{
		_vec3	vOffset = _vec3(m_pRandomMgr->GetValueF(-1.f, 1.f), m_pRandomMgr->GetValueF(-1.f, 1.f), m_pRandomMgr->GetValueF(-1.f, 1.f));
		 
		_vec3	vDir = *D3DXVec3Normalize(&vOffset, &vOffset) * m_tEffInfo.m_fPtc_Radius;

		pAtt->vPosition = m_tPtcInfo.vOrigin + vDir;

		if(m_tEffInfo.m_eTransType != EFFECT_INFO::TYPE_POINTER)
			D3DXVec3TransformCoord(&pAtt->vPosition, &pAtt->vPosition, &m_tPtcInfo.matParent);

		pAtt->vVelocity = vOffset * m_pRandomMgr->GetValueF(m_tEffInfo.m_fSpeed_Min, m_tEffInfo.m_fSpeed_Max);

		if (m_tEffInfo.m_eTransType != EFFECT_INFO::TYPE_POINTER)
			D3DXVec3TransformNormal(&pAtt->vVelocity, &pAtt->vVelocity, &m_tPtcInfo.matParent);

		pAtt->vVelociy_Origin = pAtt->vVelocity;

	}
	else if (m_tEffInfo.m_ePtc_Shape == EFFECT_INFO::SHAPE_CONE)
	{

	}
	else // Normal
	{
		float fOffset_RandX = m_pRandomMgr->GetValueF(m_tEffInfo.m_fOffset_MinX, m_tEffInfo.m_fOffset_MaxX);
		float fOffset_RandY = m_pRandomMgr->GetValueF(m_tEffInfo.m_fOffset_MinY, m_tEffInfo.m_fOffset_MaxY);
		float fOffset_RandZ = m_pRandomMgr->GetValueF(m_tEffInfo.m_fOffset_MinZ, m_tEffInfo.m_fOffset_MaxZ);

		float	fOriginX = m_tPtcInfo.vOrigin.x + fOffset_RandX;
		float	fOriginY = m_tPtcInfo.vOrigin.y + fOffset_RandY;
		float	fOriginZ = m_tPtcInfo.vOrigin.z + fOffset_RandZ;

		pAtt->vPosition = _vec3(fOriginX, fOriginY, fOriginZ);

		if (m_tEffInfo.m_eTransType != EFFECT_INFO::TYPE_POINTER)
			D3DXVec3TransformCoord(&pAtt->vPosition, &pAtt->vPosition, &m_tPtcInfo.matParent);

		float fVel_RandX = m_pRandomMgr->GetValueF(m_tEffInfo.m_fVel_MinX, m_tEffInfo.m_fVel_MaxX);
		float fVel_RandY = m_pRandomMgr->GetValueF(m_tEffInfo.m_fVel_MinY, m_tEffInfo.m_fVel_MaxY);
		float fVel_RandZ = m_pRandomMgr->GetValueF(m_tEffInfo.m_fVel_MinZ, m_tEffInfo.m_fVel_MaxZ);

		pAtt->vVelocity = _vec3(fVel_RandX, fVel_RandY, fVel_RandZ);

		if (m_tEffInfo.m_eTransType != EFFECT_INFO::TYPE_POINTER
			&& m_tEffInfo.m_eTransType != EFFECT_INFO::TYPE_POS_UPDATED)
			D3DXVec3TransformNormal(&pAtt->vVelocity, &pAtt->vVelocity, &m_tPtcInfo.matParent);

		pAtt->vVelociy_Origin = pAtt->vVelocity;
	}

	float fLifeTime_Rand = m_pRandomMgr->GetValueF(m_tEffInfo.m_fLifeTime_Min, m_tEffInfo.m_fLifeTime_MAX);
	pAtt->fLifeTime = fLifeTime_Rand;

	pAtt->bIsAlive = true;
	pAtt->fAge = 0.f;
	pAtt->fSize = pAtt->fSize_Origin = m_pRandomMgr->GetValueF(m_tEffInfo.m_fPtc_SizeMin, m_tEffInfo.m_fPtc_SizeMax);

	float	fColorR = m_pRandomMgr->GetValueF(m_tEffInfo.m_fCol_MinR, m_tEffInfo.m_fCol_MaxR);
	float	fColorG = m_pRandomMgr->GetValueF(m_tEffInfo.m_fCol_MinG, m_tEffInfo.m_fCol_MaxG);
	float	fColorB = m_pRandomMgr->GetValueF(m_tEffInfo.m_fCol_MinB, m_tEffInfo.m_fCol_MaxB);
	float	fColorA = m_pRandomMgr->GetValueF(m_tEffInfo.m_fCol_MinA, m_tEffInfo.m_fCol_MaxA);

	pAtt->vColor = pAtt->vColor_Fade = D3DXCOLOR(fColorR / 255.f, fColorG / 255.f, fColorB / 255.f, fColorA / 255.f);

	// 1차 Force 값 지정(Random 없음)
	if(false == m_tEffInfo.m_bUseForceRand)
		pAtt->vAcceleration = _vec3(m_tEffInfo.m_fForce_X, m_tEffInfo.m_fForce_Y, m_tEffInfo.m_fForce_Z);

}

void CMyEffect_Particle::SetUp_OnConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	_matrix		matView, matProj;
	matView = m_pCam_Manager->Get_ViewMatrix();
	matProj = m_pCam_Manager->Get_ProjMatrix();

	_vec3	vCamPos;
	vCamPos = m_pCam_Manager->Get_WorldPosition();

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);
	pEffect->SetVector("g_vCamera_Pos", &_vec4(vCamPos.x, vCamPos.y, vCamPos.z, 1.f));

	pEffect->SetInt("g_iViewportHeight", m_iViewportHeight);
	pEffect->SetFloat("g_fPTC_Bright", m_tEffInfo.m_fBright);

	m_pTextureCom->SetUp_OnShader(pEffect, "g_ParticleTexture");

	CTarget_Manager::GetInstance()->SetUp_OnShader(pEffect, L"RenderTarget_Depth", "g_DepthTexture");

	Engine::Safe_Release(pEffect);
}

void CMyEffect_Particle::Render_Buffer(void)
{
	m_pGraphicDev->SetFVF(VTXPTC::FVF);
	m_pGraphicDev->SetStreamSource(0, m_pVB, 0, sizeof(VTXPTC));

	if (m_tPtcInfo.dwOffset >= m_tPtcInfo.dwSize)
		m_tPtcInfo.dwOffset = 0;

	VTXPTC* pParticle = nullptr;

	m_pVB->Lock(m_tPtcInfo.dwOffset * sizeof(VTXPTC), m_tPtcInfo.dwBatchSize * sizeof(VTXPTC), (void**)&pParticle, m_tPtcInfo.dwOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

	DWORD numParticlesInBatch = 0;

	//if(m_listParticles.size() < m_tPtcInfo.dwBatchSize)
	//	return;
	PTC_ATTRIBUTE* pAtt = nullptr;
	//for (auto& pAtt : m_listParticles)
	//{
	int iSize = m_listParticles.Size();
	for(int i=0; i<iSize; ++i)
	{
		pAtt = m_listParticles.Get(i);

		if (pAtt->bIsAlive)
		{
			//
			// Copy a batch of the living particles to the
			// next vertex buffer segment
			//
			if (m_tEffInfo.m_eTransType == EFFECT_INFO::TYPE_POINTER)
				D3DXVec3TransformCoord(&pParticle->vPosition, &pAtt->vPosition, &m_tPtcInfo.matParent);
			else
				pParticle->vPosition = pAtt->vPosition;
			pParticle->fSize = pAtt->fSize;
			pParticle->vColor = (D3DCOLOR)pAtt->vColor;

			++pParticle; // next element;

			++numParticlesInBatch;

			// if this batch full?
			if (numParticlesInBatch == m_tPtcInfo.dwBatchSize)
			{
				//
				// Draw the last batch of particles that was
				// copied to the vertex buffer. 
				//
				m_pVB->Unlock();

				m_pGraphicDev->DrawPrimitive(D3DPT_POINTLIST, m_tPtcInfo.dwOffset, m_tPtcInfo.dwBatchSize);

				//
				// While that batch is drawing, start filling the
				// next batch with particles.
				//

				// move the offset to the start of the next batch
				m_tPtcInfo.dwOffset += m_tPtcInfo.dwBatchSize;

				// don't offset into memory thats outside the vb's range.
				// If we're at the end, start at the beginning.
				if (m_tPtcInfo.dwOffset >= m_tPtcInfo.dwSize)
					m_tPtcInfo.dwOffset = 0;

				m_pVB->Lock(m_tPtcInfo.dwOffset * sizeof(VTXPTC), m_tPtcInfo.dwBatchSize * sizeof(VTXPTC), (void**)&pParticle, m_tPtcInfo.dwOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

				numParticlesInBatch = 0; // reset for new batch
			}
		}
	}

	m_pVB->Unlock();

	// its possible that the LAST batch being filled never 
	// got rendered because the condition 
	// (numParticlesInBatch == _vbBatchSize) would not have
	// been satisfied.  We draw the last partially filled batch now.

	if (numParticlesInBatch)
	{
		m_pGraphicDev->DrawPrimitive(D3DPT_POINTLIST, m_tPtcInfo.dwOffset, numParticlesInBatch);
	}

	// next block
	m_tPtcInfo.dwOffset += m_tPtcInfo.dwBatchSize;

}

_bool CMyEffect_Particle::Is_Inside(const _vec3 & vPos)
{
	if (vPos.x >= m_tPtcInfo.vMin.x && vPos.y >= m_tPtcInfo.vMin.y && vPos.z >= m_tPtcInfo.vMin.z &&
		vPos.x <= m_tPtcInfo.vMax.x && vPos.y <= m_tPtcInfo.vMax.y && vPos.z <= m_tPtcInfo.vMax.z)
	{
		return true;
	}
	else
	{
		return false;
	}

	return true;
}

//void CMyEffect_Particle::Remove_DeadParticles(void)
//{
//	LISTPARTICLES::iterator		iter = m_listParticles.begin();
//
//	for (; iter != m_listParticles.end();)
//	{
//		if (false == (*iter)->bIsAlive)
//			iter = m_listParticles.erase(iter);
//		else
//			++iter;
//	}
//}

void CMyEffect_Particle::Curve_Alpha(PTC_ATTRIBUTE * pAtt, float fTimeRatio)
{
	if (nullptr == pAtt)
		return;

	for (int i = 0; i < m_tEffInfo.m_iCurve_Alpha_Cnt; ++i)
	{
		if (fTimeRatio <= m_tEffInfo.m_arrCurve_Alpha[EFFECT_INFO::COLUMN_TIME][i])
		{
			// 1. 시간값이 0일 때
			if (m_tEffInfo.m_arrCurve_Alpha[EFFECT_INFO::COLUMN_TIME][i] == 0.f)
			{
				pAtt->vColor.a = m_tEffInfo.m_arrCurve_Alpha[EFFECT_INFO::COLUMN_VALUE][i] * pAtt->vColor_Fade.a;
			}
			else
			{
				float fValuePerTime = (m_tEffInfo.m_arrCurve_Alpha[EFFECT_INFO::COLUMN_VALUE][i] - m_tEffInfo.m_arrCurve_Alpha[EFFECT_INFO::COLUMN_VALUE][i - 1])
					/ (m_tEffInfo.m_arrCurve_Alpha[EFFECT_INFO::COLUMN_TIME][i] - m_tEffInfo.m_arrCurve_Alpha[EFFECT_INFO::COLUMN_TIME][i - 1]);

				//pAtt->vColor.a = (fTimeRatio - m_tEffInfo.m_arrCurve_Alpha[EFFECT_INFO::COLUMN_TIME][i - 1]) * fValuePerTime + m_tEffInfo.m_arrCurve_Alpha[EFFECT_INFO::COLUMN_VALUE][i - 1] * m_Color_Init.a;
				pAtt->vColor.a = (fTimeRatio - m_tEffInfo.m_arrCurve_Alpha[EFFECT_INFO::COLUMN_TIME][i - 1]) * fValuePerTime + m_tEffInfo.m_arrCurve_Alpha[EFFECT_INFO::COLUMN_VALUE][i - 1];
				pAtt->vColor.a *= pAtt->vColor_Fade.a;
			}
			break;
		}
	}
}

void CMyEffect_Particle::Curve_Size(PTC_ATTRIBUTE * pAtt, float fTimeRatio)
{
	if (nullptr == pAtt)
		return;

	for (int i = 0; i < m_tEffInfo.m_iCurve_Size_Cnt; ++i)
	{
		if (fTimeRatio <= m_tEffInfo.m_arrCurve_Size[EFFECT_INFO::COLUMN_TIME][i])
		{
			// 1. 시간값이 0일 때
			if (m_tEffInfo.m_arrCurve_Size[EFFECT_INFO::COLUMN_TIME][i] == 0.f)
			{
				pAtt->fSize = m_tEffInfo.m_arrCurve_Size[EFFECT_INFO::COLUMN_VALUE][i] * pAtt->fSize_Origin;
			}
			else
			{
				float fValuePerTime = (m_tEffInfo.m_arrCurve_Size[EFFECT_INFO::COLUMN_VALUE][i] - m_tEffInfo.m_arrCurve_Size[EFFECT_INFO::COLUMN_VALUE][i - 1])
					/ (m_tEffInfo.m_arrCurve_Size[EFFECT_INFO::COLUMN_TIME][i] - m_tEffInfo.m_arrCurve_Size[EFFECT_INFO::COLUMN_TIME][i - 1]);

				pAtt->fSize = (fTimeRatio - m_tEffInfo.m_arrCurve_Size[EFFECT_INFO::COLUMN_TIME][i - 1]) * fValuePerTime + m_tEffInfo.m_arrCurve_Size[EFFECT_INFO::COLUMN_VALUE][i - 1];
				pAtt->fSize *= pAtt->fSize_Origin;
			}
			break;
		}
	}
}

void CMyEffect_Particle::Curve_Speed(PTC_ATTRIBUTE * pAtt, float fTimeRatio)
{
	if (nullptr == pAtt)
		return;

	for (int i = 0; i < m_tEffInfo.m_iCurve_Spd_Cnt; ++i)
	{
		if (fTimeRatio <= m_tEffInfo.m_arrCurve_Spd[EFFECT_INFO::COLUMN_TIME][i])
		{
			// 1. 시간값이 0일 때
			if (m_tEffInfo.m_arrCurve_Spd[EFFECT_INFO::COLUMN_TIME][i] == 0.f)
			{
				pAtt->vVelocity = m_tEffInfo.m_arrCurve_Spd[EFFECT_INFO::COLUMN_VALUE][i] * pAtt->vVelociy_Origin;
			}
			else
			{
				float fValuePerTime = (m_tEffInfo.m_arrCurve_Spd[EFFECT_INFO::COLUMN_VALUE][i] - m_tEffInfo.m_arrCurve_Spd[EFFECT_INFO::COLUMN_VALUE][i - 1])
					/ (m_tEffInfo.m_arrCurve_Spd[EFFECT_INFO::COLUMN_TIME][i] - m_tEffInfo.m_arrCurve_Spd[EFFECT_INFO::COLUMN_TIME][i - 1]);

				float fDelta = (fTimeRatio - m_tEffInfo.m_arrCurve_Spd[EFFECT_INFO::COLUMN_TIME][i - 1]) * fValuePerTime + m_tEffInfo.m_arrCurve_Spd[EFFECT_INFO::COLUMN_VALUE][i - 1];
				pAtt->vVelocity = pAtt->vVelociy_Origin * fDelta;
			}
			break;
		}
	}
}

void CMyEffect_Particle::Curve_Color(PTC_ATTRIBUTE * pAtt, float fTimeRatio)
{
	if (nullptr == pAtt)
		return;

	if (m_tEffInfo.m_iCurve_R_Cnt > 1)
	{
		for (int i = 0; i < m_tEffInfo.m_iCurve_R_Cnt; ++i)
		{
			if (fTimeRatio <= m_tEffInfo.m_arrCurve_R[EFFECT_INFO::COLUMN_TIME][i])
			{
				// 1. 시간값이 0일 때
				if (m_tEffInfo.m_arrCurve_R[EFFECT_INFO::COLUMN_TIME][i] == 0.f)
				{
					pAtt->vColor.r = m_tEffInfo.m_arrCurve_R[EFFECT_INFO::COLUMN_VALUE][i] * pAtt->vColor_Fade.r;
				}
				else
				{
					float fValuePerTime = (m_tEffInfo.m_arrCurve_R[EFFECT_INFO::COLUMN_VALUE][i] - m_tEffInfo.m_arrCurve_R[EFFECT_INFO::COLUMN_VALUE][i - 1])
						/ (m_tEffInfo.m_arrCurve_R[EFFECT_INFO::COLUMN_TIME][i] - m_tEffInfo.m_arrCurve_R[EFFECT_INFO::COLUMN_TIME][i - 1]);

					pAtt->vColor.r = (fTimeRatio - m_tEffInfo.m_arrCurve_R[EFFECT_INFO::COLUMN_TIME][i - 1]) * fValuePerTime + m_tEffInfo.m_arrCurve_R[EFFECT_INFO::COLUMN_VALUE][i - 1];
					pAtt->vColor.r *= pAtt->vColor_Fade.r;
				}
				break;
			}
		}
	}

	if (m_tEffInfo.m_iCurve_G_Cnt > 1)
	{
		for (int i = 0; i < m_tEffInfo.m_iCurve_G_Cnt; ++i)
		{
			if (fTimeRatio <= m_tEffInfo.m_arrCurve_G[EFFECT_INFO::COLUMN_TIME][i])
			{
				// 1. 시간값이 0일 때
				if (m_tEffInfo.m_arrCurve_G[EFFECT_INFO::COLUMN_TIME][i] == 0.f)
				{
					pAtt->vColor.g = m_tEffInfo.m_arrCurve_G[EFFECT_INFO::COLUMN_VALUE][i] * pAtt->vColor_Fade.g;
				}
				else
				{
					float fValuePerTime = (m_tEffInfo.m_arrCurve_G[EFFECT_INFO::COLUMN_VALUE][i] - m_tEffInfo.m_arrCurve_G[EFFECT_INFO::COLUMN_VALUE][i - 1])
						/ (m_tEffInfo.m_arrCurve_G[EFFECT_INFO::COLUMN_TIME][i] - m_tEffInfo.m_arrCurve_G[EFFECT_INFO::COLUMN_TIME][i - 1]);

					pAtt->vColor.g = (fTimeRatio - m_tEffInfo.m_arrCurve_G[EFFECT_INFO::COLUMN_TIME][i - 1]) * fValuePerTime + m_tEffInfo.m_arrCurve_G[EFFECT_INFO::COLUMN_VALUE][i - 1];
					pAtt->vColor.g *= pAtt->vColor_Fade.g;
				}
				break;
			}
		}
	}
	
	if (m_tEffInfo.m_iCurve_B_Cnt > 1)
	{
		for (int i = 0; i < m_tEffInfo.m_iCurve_B_Cnt; ++i)
		{
			if (fTimeRatio <= m_tEffInfo.m_arrCurve_B[EFFECT_INFO::COLUMN_TIME][i])
			{
				// 1. 시간값이 0일 때
				if (m_tEffInfo.m_arrCurve_B[EFFECT_INFO::COLUMN_TIME][i] == 0.f)
				{
					pAtt->vColor.b = m_tEffInfo.m_arrCurve_B[EFFECT_INFO::COLUMN_VALUE][i] * pAtt->vColor_Fade.b;
				}
				else
				{
					float fValuePerTime = (m_tEffInfo.m_arrCurve_B[EFFECT_INFO::COLUMN_VALUE][i] - m_tEffInfo.m_arrCurve_B[EFFECT_INFO::COLUMN_VALUE][i - 1])
						/ (m_tEffInfo.m_arrCurve_B[EFFECT_INFO::COLUMN_TIME][i] - m_tEffInfo.m_arrCurve_B[EFFECT_INFO::COLUMN_TIME][i - 1]);

					pAtt->vColor.b = (fTimeRatio - m_tEffInfo.m_arrCurve_B[EFFECT_INFO::COLUMN_TIME][i - 1]) * fValuePerTime + m_tEffInfo.m_arrCurve_B[EFFECT_INFO::COLUMN_VALUE][i - 1];
					pAtt->vColor.b *= pAtt->vColor_Fade.b;
				}
				break;
			}
		}
	}
	
}

void CMyEffect_Particle::Curve_Bright(PTC_ATTRIBUTE * pAtt, float fTimeRatio)
{
	if (nullptr == pAtt)
		return;

	for (int i = 0; i < m_tEffInfo.m_iCurve_Brt_Cnt; ++i)
	{
		if (fTimeRatio <= m_tEffInfo.m_arrCurve_BRT[EFFECT_INFO::COLUMN_TIME][i])
		{
			// 1. 시간값이 0일 때
			if (m_tEffInfo.m_arrCurve_BRT[EFFECT_INFO::COLUMN_TIME][i] == 0.f)
			{
				m_tEffInfo.m_fBright *= m_tEffInfo.m_arrCurve_BRT[EFFECT_INFO::COLUMN_VALUE][i] * m_tEffInfo.m_fBright_Init;
			}
			else
			{
				float fValuePerTime = (m_tEffInfo.m_arrCurve_BRT[EFFECT_INFO::COLUMN_VALUE][i] - m_tEffInfo.m_arrCurve_BRT[EFFECT_INFO::COLUMN_VALUE][i - 1])
					/ (m_tEffInfo.m_arrCurve_BRT[EFFECT_INFO::COLUMN_TIME][i] - m_tEffInfo.m_arrCurve_BRT[EFFECT_INFO::COLUMN_TIME][i - 1]);

				m_tEffInfo.m_fBright = (fTimeRatio - m_tEffInfo.m_arrCurve_BRT[EFFECT_INFO::COLUMN_TIME][i - 1]) * fValuePerTime + m_tEffInfo.m_arrCurve_BRT[EFFECT_INFO::COLUMN_VALUE][i - 1];
				m_tEffInfo.m_fBright *= m_tEffInfo.m_fBright_Init;
			}
			break;
		}
	}
}

CMyEffect_Particle * CMyEffect_Particle::Create(LPDIRECT3DDEVICE9 pGraphicDev, const EFFECT_INFO& tInfo)
{
	CMyEffect_Particle*		pInstance = new CMyEffect_Particle(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject(tInfo)))
	{
		MSG_BOX("CMyEffect_Particle Create Failed");
		Engine::Safe_Release(pInstance);
	}

	return pInstance;
}

CMyEffect * CMyEffect_Particle::Clone(void)
{
	return new CMyEffect_Particle(*this);
}
#ifndef _FINAL
void CMyEffect_Particle::Debug_Render()
{
	//size_t listSize = m_listParticles.size();
	int listSize = m_listParticles.Size();
	CDebug::GetInstance()->Add_DebugText("CMyEffect_Particle");
	CDebug::GetInstance()->Add_DebugText("size:%d", listSize);
	CDebug::GetInstance()->Add_DebugText(">>>  Donw (VK_DOWN) / UP (VK_UP)  <<<");
	int Count = 0;
	CDebug::GetInstance()->Add_DebugText("-- ParticleSystem --");
	CDebug::GetInstance()->Add_DebugText(m_tEffInfo.m_fBright, "Bright");

	PTC_ATTRIBUTE* pParticle = nullptr;
	//for (auto pParticle : m_listParticles)
	for(int i=0; i<listSize; ++i)
	{
		pParticle = m_listParticles.Get(i);
		if (iStartCount > Count)
		{
			++Count;
			continue;
		}
		//CDebug::GetInstance()->Add_DebugText(L"%c) %s", CDebug::DisplayKey[Count - iStartCount], pParticle);
		//if (CInput_Device::GetInstance()->Get_DownKeyState(CDebug::Key[Count]))
		//if (GetAsyncKeyState(CDebug::Key[Count - iStartCount]) & 0x0001)
		//{
		//	iStartCount = 0;
		//	//pParticle->Set_LeafNode();
		//	CDebug::GetInstance()->Set_CurDebug(std::bind(&CComponent::Debug_Render, pLayer.second), pLayer.second);
		//}
		CDebug::GetInstance()->Add_DebugText("------ %d -----",Count);
		//CDebug::GetInstance()->Add_DebugText(Count,"Count");
		if (pParticle->bIsAlive)
			CDebug::GetInstance()->Add_DebugText("Alive");
		else
			CDebug::GetInstance()->Add_DebugText("Die");

		CDebug::GetInstance()->Add_DebugText(pParticle->vPosition, "vPosition");
		CDebug::GetInstance()->Add_DebugText(D3DXVec3Length(&pParticle->vVelocity), "Speed");
		CDebug::GetInstance()->Add_DebugText(pParticle->vVelocity, "vVelocity");
		CDebug::GetInstance()->Add_DebugText(pParticle->vVelociy_Origin, "vVelociy_Origin");
		CDebug::GetInstance()->Add_DebugText(pParticle->vAcceleration, "vAcceleration");
		CDebug::GetInstance()->Add_DebugText(pParticle->fAge, "fAge");
		CDebug::GetInstance()->Add_DebugText(pParticle->fLifeTime, "LifeTime");
		CDebug::GetInstance()->Add_DebugText(pParticle->fSize, "Size");
		CDebug::GetInstance()->Add_DebugText(pParticle->fSize_Origin, "Origin Size");
		CDebug::GetInstance()->Add_DebugText("(COLOR) R:%f / G:%f / B:%f / A:%f", pParticle->vColor.r * 255.f, pParticle->vColor.g * 255.f, pParticle->vColor.b * 255.f, pParticle->vColor.a * 255.f );
		CDebug::GetInstance()->Add_DebugText("(ORIGIN) R:%f / G:%f / B:%f / A:%f", pParticle->vColor_Fade.r * 255.f, pParticle->vColor_Fade.g * 255.f, pParticle->vColor_Fade.b * 255.f, pParticle->vColor_Fade.a * 255.f);

	

		if (GetAsyncKeyState(VK_DOWN) & 0x0001)
		{
			iStartCount += 1;
			if (iStartCount >= listSize)
				iStartCount = listSize - 1;
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
_ulong CMyEffect_Particle::Free(void)
{
	_ulong		dwRefCnt = 0;

	Engine::Safe_Release(m_pTextureCom);
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pVB);

	PTC_ATTRIBUTE* pParticle = nullptr;
	while (!m_listParticles.Empty())
	{
		pParticle = m_listParticles.Pop_Back();
		Safe_Delete(pParticle);
	}
	m_listParticles.Clear();

	while (!m_listRecycles.Empty())
	{
		pParticle = m_listRecycles.Pop_Back();
		Safe_Delete(pParticle);
	}
	m_listRecycles.Clear();

	//for (auto& pParticle : m_listParticles)
	//	Engine::Safe_Delete(pParticle);

	//for (auto& pRecycle : m_listRecycles)
	//	Engine::Safe_Delete(pRecycle);

	//m_listParticles.clear();
	//m_listRecycles.clear();


	dwRefCnt = Engine::CMyEffect::Free();

	return dwRefCnt;
}
