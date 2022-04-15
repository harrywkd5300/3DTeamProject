
#include "Frame.h"

CFrame::CFrame(void)
: m_fTimeAcc(0.f)
{
}

CFrame::~CFrame(void)
{
}

HRESULT CFrame::Initialize(const float& fCallCnt)
{
	m_fTimePerSec = 1.f / fCallCnt;
		
	return NOERROR;
}


bool CFrame::Permit_Call(const float& fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;

	 // 일초에 지정한 갯수만큼만 트루를 리턴한다.
	if(m_fTimePerSec <= m_fTimeAcc)
	{
		m_fTimeAcc = 0.f;
		return true;
	}

	return false;
}


CFrame* CFrame::Create(const float& fCallCnt)
{
	CFrame*		pInstance = new CFrame();

	if(FAILED(pInstance->Initialize(fCallCnt)))
	{
		MessageBox(0, L"Create Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;

}

_ulong CFrame::Free(void)
{

	return 0;
}
