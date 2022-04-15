#include "Timer.h"

CTimer::CTimer(void)
: m_fTimeDelta(0.f)
{
}

CTimer::~CTimer(void)
{
}

HRESULT CTimer::Initialize(void)
{
	// GetTickCount()
	QueryPerformanceCounter(&m_FrameTime);		//현재 Cpu가 카운팅한 시간을 받아온다.
	QueryPerformanceCounter(&m_FixTime);		//지금은 초기화를 한다고 생각하자!
	QueryPerformanceCounter(&m_LastTime);	

	QueryPerformanceFrequency(&m_CpuTick);		//CPU의 1초에 카운팅할 수 있는 Tick을 얻어오는 함수.

	//1초당 떨림수를 판단하기 위해서 사용한다.
	return NOERROR;
}


void CTimer::Compute_Timer(void)
{
	//매 프레임, 즉 호출이 될때마다 새로운 시간을 구한다.
	QueryPerformanceCounter(&m_FrameTime);


	//새로 구한시간과 처음 Init되었을때의 시간 사이를 구한다.
	if( (m_FrameTime.QuadPart - m_LastTime.QuadPart) > m_CpuTick.QuadPart)
	{
		//프로세스 부하가 걸렸을때 시간이 뒤로 밀리는것을 방지하기 위한 분기문.

		//CPU Tick을 다시 갱신하다.
		QueryPerformanceFrequency(&m_CpuTick);

		//1초 뒤 값을 새롭게 갱신한다.
		m_LastTime.QuadPart = m_FrameTime.QuadPart;
	}

	//메인게임 Update() 호출되고 다음 호출까지의 인터벌값을 진동수로 나누게 된다면
	//우리가 프레임당 움직일수 있는 시간의 값이 나온다.

	m_fTimeDelta = float( m_FrameTime.QuadPart - m_FixTime.QuadPart ) / m_CpuTick.QuadPart;

	m_FixTime	  = m_FrameTime;
}

CTimer* CTimer::Create(void)
{
	CTimer*		pInstance = new CTimer();

	if(FAILED(pInstance->Initialize()))
	{
		MessageBox(0, L"CTimer Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

_ulong CTimer::Free(void)
{	
	return 0;
}
