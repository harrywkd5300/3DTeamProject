
#include "Frame_Manager.h"
#include "Frame.h"

USING(Engine)
IMPLEMENT_SINGLETON(CFrame_Manager)

CFrame_Manager::CFrame_Manager(void)
{
}

CFrame_Manager::~CFrame_Manager(void)
{
}


HRESULT CFrame_Manager::Add_Frame(const TCHAR* pFrameTag, const float& fCallCnt)
{
	if(NULL != Find_Frame(pFrameTag))
		return E_FAIL;

	CFrame*		pFrame = CFrame::Create(fCallCnt);
	if(NULL == pFrame)
		return E_FAIL;

	m_mapFrame.insert(MAPFRAME::value_type(pFrameTag, pFrame));
	
	return NOERROR;
}

bool CFrame_Manager::Permit_Call(const TCHAR* pFrameTag, const float& fTimeDelta)
{
	CFrame*			pFrame = NULL;

	pFrame = Find_Frame(pFrameTag);

	if(NULL == pFrame)
		return false;

	return pFrame->Permit_Call(fTimeDelta);
}

CFrame* CFrame_Manager::Find_Frame(const TCHAR* pFrameTag)
{
	MAPFRAME::iterator iter = find_if(m_mapFrame.begin(), m_mapFrame.end(), CFind_Tag(pFrameTag));

	if(iter == m_mapFrame.end())
		return NULL;

	return iter->second;
}

_ulong CFrame_Manager::Free(void)
{
	for (auto& Pair : m_mapFrame)
	{
		Engine::Safe_Release(Pair.second);
	}
	m_mapFrame.clear();

	return 0;
}
