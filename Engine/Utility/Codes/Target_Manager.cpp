#include "..\Headers\Target_Manager.h"
#include "Target.h"
#include "Debug_Manager.h"

USING(Engine)
IMPLEMENT_SINGLETON(CTarget_Manager)

CTarget_Manager::CTarget_Manager()
{
#ifndef _FINAL
	Tag(L"CTarget_Manager", std::bind(&CTarget_Manager::Debug_Render, this));
#endif

}

#ifdef ADD_GETTER_TARGETANDSHADER
const _tchar * CTarget_Manager::Get_TargetName( const _uint & _iIndex )
{
	if ( _iIndex >= m_mapTargets.size() )
		return nullptr;

	_uint iCount = _iIndex;

	for ( auto& Pair : m_mapTargets )
	{
		if ( 0 == iCount )
			return Pair.first;
		else
			--iCount;
	}

	return nullptr;
}

_uint CTarget_Manager::Get_TargetCount( void )
{
	return (_uint)m_mapTargets.size();
}
#endif

HRESULT CTarget_Manager::Get_SurfaceLevel( const _tchar* pTargetName, const _uint & iLevel, LPDIRECT3DSURFACE9 * pOut )
{
	CTarget* pTarget = Find_Target( pTargetName );

	if ( nullptr == pTarget )
		return E_FAIL;

	return pTarget->Get_SurfaceLevel( iLevel, pOut );
}

LPDIRECT3DTEXTURE9 CTarget_Manager::Get_Texture( const _tchar * pTargetName )
{
	CTarget* pTarget = Find_Target( pTargetName );

	if ( nullptr == pTarget )
		return nullptr;

	return pTarget->Get_Texture();
}

HRESULT CTarget_Manager::Add_Target(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar * pTargetTag, const _uint & iTargetCX, const _uint & iTargetCY, D3DFORMAT eFormat, D3DXCOLOR Color)
{
	CTarget* pTarget = Find_Target(pTargetTag);
	
	if(nullptr != pTarget)
		return E_FAIL;

	pTarget = CTarget::Create(pGraphicDev, iTargetCX, iTargetCY, eFormat, Color);

	if(nullptr == pTarget)
		return E_FAIL;

	m_mapTargets.insert(MAPTARGETS::value_type(pTargetTag, pTarget));

	return NOERROR;
}

HRESULT CTarget_Manager::Add_MRT(const _tchar * pMRTTag, const _tchar * pTargetTag)
{
	CTarget* pTarget = Find_Target(pTargetTag);

	if(nullptr == pTarget)
		return E_FAIL;

	list<CTarget*>*	pMRTList = Find_MRT(pMRTTag);

	if(nullptr == pMRTList) // 준비된 멀티렌더타겟이 없다.
	{
		list<CTarget*>		MRTList;
		MRTList.push_back(pTarget);

		m_mapMRT.insert(MAPMRT::value_type(pMRTTag, MRTList));
	}

	else // 내가 타겟을 추가하고자했던 멀티렌더타겟이 생성되어있는 상황이다.
	{
		pMRTList->push_back(pTarget);
	}

	return NOERROR;
}

HRESULT CTarget_Manager::SetUp_OnShader(LPD3DXEFFECT pEffect, const _tchar * pTargetTag, const char * pConstantName)
{
	CTarget* pTarget = Find_Target(pTargetTag);

	if(nullptr == pTarget)
		return E_FAIL;	

	return pTarget->SetUp_OnShader(pEffect, pConstantName);
}

HRESULT CTarget_Manager::Begin_MRT(const _tchar * pMRTTag)
{
	list<CTarget*>*	pMRTList = Find_MRT(pMRTTag);
	if(nullptr == pMRTList)
		return E_FAIL;

	// 각각의 타겟들을 원하는 색으로 지울수 있게하자.
	for(auto& pTarget : *pMRTList)
	{
		pTarget->Clear_Target();
	}


	// 멀티렌더타겟에 추가되ㅏ어있는 타겟들을 ㅈ2ㅏㅇ치에 순서대로 셋팅하자.
	_uint		iIndex = 0;

	for(auto& pTarget : *pMRTList)
	{
		pTarget->SetUp_OnGraphicDev(iIndex++);
	}

	return NOERROR;
}

HRESULT CTarget_Manager::End_MRT(const _tchar * pMRTTag)
{
	list<CTarget*>*	pMRTList = Find_MRT(pMRTTag);
	if(nullptr == pMRTList)
		return E_FAIL;

	_uint		iIndex = 0;

	for(auto& pTarget : *pMRTList)
	{
		pTarget->Release_OnGraphicDev(iIndex++);
	}

	return NOERROR;
}

HRESULT CTarget_Manager::Begin_Target(const _tchar * pTargetTag, _uint iIndex)
{
	CTarget* pTarget = Find_Target(pTargetTag);

	if (nullptr == pTarget)
		return E_FAIL;

	pTarget->Clear_Target();
	pTarget->SetUp_OnGraphicDev(iIndex);

	return NOERROR;
}

HRESULT CTarget_Manager::End_Target(const _tchar * pTargetTag, _uint iIndex)
{
	CTarget* pTarget = Find_Target(pTargetTag);

	if (nullptr == pTarget)
		return E_FAIL;

	pTarget->Release_OnGraphicDev(iIndex);

	return NOERROR;
}

#ifdef KMH_MODIFY // By.KMH
HRESULT CTarget_Manager::Clear_MRT( const _tchar * pMRTTag )
{
	list<CTarget*>*	pMRTList = Find_MRT(pMRTTag);
	if(nullptr == pMRTList)
		return E_FAIL;

	// 각각의 타겟들을 원하는 색으로 지울수 있게하자.
	for(auto& pTarget : *pMRTList)
	{
		pTarget->Clear_Target();
	}
	
	return NOERROR;
}

HRESULT CTarget_Manager::Clear_Target( const _tchar * pTargetTag )
{
	CTarget* pTarget = Find_Target(pTargetTag);

	if (nullptr == pTarget)
		return E_FAIL;

	pTarget->Clear_Target();

	return NOERROR;
}
void CTarget_Manager::Save_Target( const _tchar * pTargetTag )
{
	CTarget* pTarget = Find_Target(pTargetTag);

	if (nullptr == pTarget)
		return;

	pTarget->Save_Target(pTargetTag);
}
#endif

HRESULT CTarget_Manager::Ready_DebugBuffer(const _tchar * pTargetTag, const _float& fX, const _float& fY, const _float& fSizeX, const _float& fSizeY)
{
	CTarget* pTarget = Find_Target(pTargetTag);

	if(nullptr == pTarget)
		return E_FAIL;

	pTarget->Ready_DebugBuffer(fX, fY, fSizeX, fSizeY);

	return NOERROR;
}

void CTarget_Manager::Render_DebugBuffer(const _tchar * pMRTTag)
{
	list<CTarget*>*	pMRTList = Find_MRT(pMRTTag);
	if(nullptr == pMRTList)
		return;

	for(auto& pTarget : *pMRTList)
	{
#ifdef RENDERTARGETNAME
		pTarget->Render_DebugBuffer( L"" );
#else
		pTarget->Render_DebugBuffer();
#endif
	}
}

void CTarget_Manager::Render_DebugTarget(const _tchar * pTargetTag)
{
	CTarget* pTarget = Find_Target(pTargetTag);

	if (nullptr == pTarget)
		return;

#ifdef RENDERTARGETNAME
	pTarget->Render_DebugBuffer( pTargetTag );
#else
	pTarget->Render_DebugBuffer();
#endif
}

void CTarget_Manager::Render_DebugAllTarget( void )
{
	for ( auto& Target : m_mapTargets )
#ifdef RENDERTARGETNAME
		Target.second->Render_DebugBuffer( Target.first );
#else
		Target.second->Render_DebugBuffer();
#endif
}

CTarget * CTarget_Manager::Find_Target(const _tchar * pTargetTag)
{
	auto& iter = find_if(m_mapTargets.begin(), m_mapTargets.end(), CFind_Tag(pTargetTag));

	if(iter == m_mapTargets.end())
		return nullptr;

	return iter->second;	
}

list<CTarget*>* CTarget_Manager::Find_MRT(const _tchar * pMRTTag)
{
	auto& iter = find_if(m_mapMRT.begin(), m_mapMRT.end(), CFind_Tag(pMRTTag));

	if(iter == m_mapMRT.end())
		return nullptr;

	return &iter->second;
}
#ifndef _FINAL
void CTarget_Manager::Debug_Render()
{
	CDebug::GetInstance()->Add_DebugText("CTarget_Manager");

}
#endif
_ulong CTarget_Manager::Free(void)
{
	for(auto& Pair : m_mapTargets)
	{
		Safe_Release(Pair.second);
	}
	m_mapTargets.clear();

	return _ulong();
}
