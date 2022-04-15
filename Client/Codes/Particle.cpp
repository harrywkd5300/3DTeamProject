#include "stdafx.h"
#include "..\Headers\Particle.h"
#include "Component_Manager.h"
#include "Target_Manager.h"
#include "Camera_Manager.h"
#include "Graphic_Device.h"

CParticle::CParticle(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
	//, m_pTransformCom(nullptr)
	, m_pRendererCom(nullptr)
	, m_pShaderCom(nullptr)
	, m_pTextureCom(nullptr)
	, m_pRandomCom(Engine::CRandom::GetInstance())
	, m_pCam_Manager(Engine::CCamera_Manager::GetInstance())
	, m_pVB (nullptr)
	, m_dwTotalCnt(0)
	, m_iViewportHeight(0)
	, m_fTimeDelta(0.f)
{
	ZeroMemory(&m_tInfo, sizeof(PTC_INFO));

	m_pRandomCom->AddRef();
	m_pCam_Manager->AddRef();
#ifndef _FINAL
	Tag(L"CParticle", std::bind(&CParticle::Debug_Render, this));
#endif

}

HRESULT CParticle::Ready_GameObject(void)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	if (FAILED(Init_ParticleSystem()))
		return E_FAIL;

	return NOERROR;
}

_int CParticle::Update_GameObject(const _float & fTimeDelta)
{
	if (m_bIsDead)
		return 1;

	Engine::CGameObject::Update_GameObject(fTimeDelta);

	m_fTimeDelta += fTimeDelta;

	for (auto& pParticle : m_listParticles)
	{
		pParticle->vPosition += pParticle->vVelocity * fTimeDelta;
		//pParticle->fAge += fTimeDelta;

		if (false == Is_PointInside(pParticle->vPosition))
		{
			Set_Attribute(pParticle);
		}

		/*if (pParticle->fAge > pParticle->fLifeTime)
			pParticle->bIsAlive = false;*/
	}

	//Remove_DeadParticles();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_ALPHA, this);

	return 0;
}

void CParticle::Render_GameObject(void)
{
	if (nullptr == m_pGraphicDev
		|| nullptr == m_pShaderCom
		|| nullptr == m_pCam_Manager)
		return;

	if (!m_listParticles.empty())
	{
		LPD3DXEFFECT pEffect = m_pShaderCom->Get_EffectHandle();
		if (nullptr == pEffect)
			return;

		SetUp_OnConstantTable(pEffect);

		pEffect->Begin(nullptr, 0);
		pEffect->BeginPass(SHADER_EFFECT_PARTICLE);

		//m_pMeshCom->Render_Mesh();
		Render_Buffer();

		pEffect->EndPass();
		pEffect->End();
	}

	/*char	szBuff[128] = "";
	sprintf_s(szBuff, "Time : %f", m_fTimeDelta);

	CGraphic_Device::GetInstance()->Get_Font()->DrawTextW(30.f, 60.f, D3DXCOLOR(1.f, 1.f, 1.f, 1.f), szBuff);
	*/
}

HRESULT CParticle::Ready_Component(void)
{
	Engine::CComponent*			pComponent = nullptr;

	// For.Transform
	pComponent = m_pTransformCom = (Engine::CTransform*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CTransform");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_DYNAMIC].insert(MAPCOMPONENT::value_type(L"Com_Transform", pComponent));
	m_pTransformCom->AddRef();

	// For.Renderer
	pComponent = m_pRendererCom = (Engine::CRenderer*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CRenderer");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Renderer", pComponent));
	m_pRendererCom->AddRef();

	// For.Shader
	pComponent = m_pShaderCom = (Engine::CShader*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"CShader_Effect");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Shader", pComponent));
	m_pShaderCom->AddRef();

	// For. Texture
	pComponent = m_pTextureCom = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"CTexture_Particle_Test1");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Texture", pComponent));
	m_pTextureCom->AddRef();

	return NOERROR;
}

HRESULT CParticle::Init_ParticleSystem(void)
{
	// 시스템 구조체 초기화
	ZeroMemory(&m_tInfo, sizeof(PTC_INFO));

	m_tInfo.vOrigin = _vec3(5.f, 5.f, 5.f);
	m_tInfo.vMin = _vec3(0.f, 0.f, 0.f);
	m_tInfo.vMax = _vec3(7.f, 7.f, 30.f);

	m_tInfo.fRatePerSec = 20.f;
	m_tInfo.fLoopTime = 10.f;

	m_dwTotalCnt = (_ulong)m_tInfo.fRatePerSec * m_tInfo.fLoopTime;

	m_tInfo.dwSize = m_dwTotalCnt;
	m_tInfo.dwOffset = 0;
	if(m_dwTotalCnt > 300)
		m_tInfo.dwBatchSize = 300;
	else
		m_tInfo.dwBatchSize = m_dwTotalCnt;

	for (size_t i = 0; i < m_dwTotalCnt; ++i)
		Add_Particle();

	// 버퍼 초기화
	// 매회 버텍스 버퍼에 접근해야 함 -> 동적 버텍스 버퍼를 사용해야 함 -> default 풀로 사용해야함
	if (FAILED(m_pGraphicDev->CreateVertexBuffer(
		m_tInfo.dwSize * sizeof(VTXPTC), D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY
		, VTXPTC::FVF, D3DPOOL_DEFAULT, &m_pVB, 0)))
	{
		MSG_BOX("Particle Buffer Create Failed");
		return E_FAIL;
	}

	// 파티클 사이즈 계산을 위한 뷰포트 높이값 구하기
	D3DVIEWPORT9	Viewport;
	ZeroMemory(&Viewport, sizeof(D3DVIEWPORT9));

	m_pGraphicDev->GetViewport(&Viewport);
	m_iViewportHeight = Viewport.Height;

	return NOERROR;
}

void CParticle::Add_Particle(void)
{
	PTC_ATTRIBUTE*		pAtt = new PTC_ATTRIBUTE;
	ZeroMemory(pAtt, sizeof(PTC_ATTRIBUTE));

	Set_Attribute(pAtt);

	m_listParticles.push_back(pAtt);
}

void CParticle::Set_Attribute(PTC_ATTRIBUTE * pAtt)
{
	pAtt->bIsAlive = true;

	pAtt->vPosition = _vec3(m_pRandomCom->GetValueF(m_tInfo.vMin.x, m_tInfo.vMax.x), m_tInfo.vMax.y, m_pRandomCom->GetValueF(m_tInfo.vMin.z, m_tInfo.vMax.z));

	pAtt->vVelocity.x = m_pRandomCom->GetValueF(-1.f, 1.f);
	pAtt->vVelocity.y = m_pRandomCom->GetValueF(0.f, 1.f) * -1.f;
	pAtt->vVelocity.z = m_pRandomCom->GetValueF(-1.f, 1.f);

	pAtt->fLifeTime = 20.f;
	pAtt->fAge = 0.f;
	pAtt->fSize = 0.1f;

	pAtt->vColor = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	pAtt->vColor_Fade = D3DXCOLOR(0.f, 0.f, 0.f, 0.f);
}

HRESULT CParticle::SetUp_OnConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorld");

	_matrix			matView, matProj;
	matView = m_pCam_Manager->Get_ViewMatrix();
	matProj = m_pCam_Manager->Get_ProjMatrix();

	_vec3	vCam_Pos;
	vCam_Pos = m_pCam_Manager->Get_WorldPosition();

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);
	pEffect->SetVector("g_vCamera_Pos", &_vec4(vCam_Pos.x, vCam_Pos.y, vCam_Pos.z, 1.f));

	pEffect->SetInt("g_iViewportHeight", m_iViewportHeight);

	m_pTextureCom->SetUp_OnShader(pEffect, "g_ParticleTexture");

	CTarget_Manager::GetInstance()->SetUp_OnShader(pEffect, L"RenderTarget_Depth", "g_DepthTexture");

	Engine::Safe_Release(pEffect);

	return NOERROR;
}

void CParticle::Render_Buffer(void)
{
	m_pGraphicDev->SetFVF(VTXPTC::FVF);
	m_pGraphicDev->SetStreamSource(0, m_pVB, 0, sizeof(VTXPTC));

	if (m_tInfo.dwOffset >= m_tInfo.dwSize)
		m_tInfo.dwOffset = 0;

	VTXPTC* pParticle = nullptr;

	m_pVB->Lock(m_tInfo.dwOffset * sizeof(VTXPTC), m_tInfo.dwBatchSize * sizeof(VTXPTC), (void**)&pParticle, m_tInfo.dwOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

	DWORD numParticlesInBatch = 0;

	for (auto&	pAtt : m_listParticles)
	{
		if (pAtt->bIsAlive)
		{
			//
			// Copy a batch of the living particles to the
			// next vertex buffer segment
			//
			pParticle->vPosition = pAtt->vPosition;
			pParticle->fSize = pAtt->fSize;
			pParticle->vColor = (D3DCOLOR)pAtt->vColor;

			
			++pParticle; // next element;

			++numParticlesInBatch;

								   // if this batch full?
			if (numParticlesInBatch == m_tInfo.dwBatchSize)
			{
				//
				// Draw the last batch of particles that was
				// copied to the vertex buffer. 
				//
				m_pVB->Unlock();

				m_pGraphicDev->DrawPrimitive(D3DPT_POINTLIST, m_tInfo.dwOffset, m_tInfo.dwBatchSize);

				//
				// While that batch is drawing, start filling the
				// next batch with particles.
				//

				// move the offset to the start of the next batch
				m_tInfo.dwOffset += m_tInfo.dwBatchSize;

				// don't offset into memory thats outside the vb's range.
				// If we're at the end, start at the beginning.
				if (m_tInfo.dwOffset >= m_tInfo.dwSize)
					m_tInfo.dwOffset = 0;

				m_pVB->Lock(m_tInfo.dwOffset * sizeof(VTXPTC), m_tInfo.dwBatchSize * sizeof(VTXPTC), (void**)&pParticle, m_tInfo.dwOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

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
		m_pGraphicDev->DrawPrimitive(D3DPT_POINTLIST, m_tInfo.dwOffset, numParticlesInBatch);
	}

	// next block
	m_tInfo.dwOffset += m_tInfo.dwBatchSize;

}

_bool CParticle::Is_PointInside(const _vec3 & vPos)
{
	if (vPos.x >= m_tInfo.vMin.x && vPos.y >= m_tInfo.vMin.y && vPos.z >= m_tInfo.vMin.z &&
		vPos.x <= m_tInfo.vMax.x && vPos.y <= m_tInfo.vMax.y && vPos.z <= m_tInfo.vMax.z)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void CParticle::Remove_DeadParticles(void)
{
	LISTPARTICLES::iterator		iter = m_listParticles.begin();

	for (; iter != m_listParticles.end();)
	{
		if (false == (*iter)->bIsAlive)
			iter = m_listParticles.erase(iter);
		else
			++iter;
	}
}

CParticle * CParticle::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CParticle*		pInstance = new CParticle(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MSG_BOX("CParticle Create Failed");
		Engine::Safe_Release(pInstance);
	}

	return pInstance;
}
#ifndef _FINAL
void CParticle::Debug_Render()
{
	CDebug::GetInstance()->Add_DebugText("CParticle");

}
#endif
_ulong CParticle::Free(void)
{
	_ulong		dwRefCnt = 0;

	//Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pShaderCom);
	Engine::Safe_Release(m_pTextureCom);
	Engine::Safe_Release(m_pRandomCom);
	Engine::Safe_Release(m_pCam_Manager);

	Engine::Safe_Release(m_pVB);

	for (auto& pParticle : m_listParticles)
		Engine::Safe_Delete(pParticle);

	m_listParticles.clear();

	dwRefCnt = Engine::CGameObject::Free();

	return dwRefCnt;
}
