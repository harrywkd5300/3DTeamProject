#pragma once

#define FORCEINLINE __forceinline									/* Force code to be inline */
#define FORCENOINLINE __declspec(noinline)							/* Force code to NOT be inline */
#define CONSTEXPR constexpr

#define BEGIN(NAMESPACE) namespace NAMESPACE {
#define END }

#define USING(NAMESPACE) using namespace NAMESPACE;

#define MSG_BOX(MESSAGE) MessageBox(0, TEXT(MESSAGE), TEXT("System Error"), MB_OK)

#define DLL_EXPORT _declspec(dllexport)

#define NO_COPY(CLASSNAME)							\
	private:										\
	CLASSNAME(const CLASSNAME&);					\
	CLASSNAME& operator = (const CLASSNAME&);		

#define DECLARE_SINGLETON(CLASSNAME)				\
	NO_COPY(CLASSNAME)								\
	private:										\
	static CLASSNAME*	m_pInstance;				\
	public:											\
	static CLASSNAME*	GetInstance( void );		\
	static unsigned long DestroyInstance( void );			

#define IMPLEMENT_SINGLETON(CLASSNAME)				\
	CLASSNAME*	CLASSNAME::m_pInstance = NULL;		\
	CLASSNAME*	CLASSNAME::GetInstance( void )	{	\
	if(NULL == m_pInstance) {						\
	m_pInstance = new CLASSNAME;					\
	}												\
	return m_pInstance;								\
	}												\
	unsigned long CLASSNAME::DestroyInstance( void ) {		\
	unsigned long dwRefCnt = 0;						\
	if(NULL != m_pInstance)	{						\
	dwRefCnt = m_pInstance->Release();				\
	if(0 == dwRefCnt)								\
	m_pInstance = NULL;								\
	}												\
	return dwRefCnt;								\
	}


// �Լ� ���� �ð� �����ϴ� ��ũ��
// �� START - END ���� ����� ��.
// fTime = �и������� / bErr - FALSE�� �� ����
//���� :
// bool bErr, float fTime;
// CHECK_TIME_START
// TestFunc()
// CHECK_TIME_END(fTime,bErr)
//if(bErr) 
//	printf("TestFunc() �ɸ� �ð���.. %8.6f�̸�����.",fTime);
#define CHECK_TIME_START __int64 freq, start, end; if (QueryPerformanceFrequency((_LARGE_INTEGER*)&freq)) {QueryPerformanceCounter((_LARGE_INTEGER*)&start);  
#define CHECK_TIME_END(fTime,bErr) QueryPerformanceCounter((_LARGE_INTEGER*)&end); fTime=(float)((double)(end - start)/freq*1000); bErr=TRUE; } else bErr=FALSE;

