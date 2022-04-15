#include <stdio.h>
#include <stdarg.h>
#include <string.h>

CBase::CBase(void)
	: m_dwRefCnt(0), m_dwKey(MakeKey())
{


#ifndef _FINAL
	m_pDebugNode = new tag_DebugNode;
	m_pDebugNode->str = L"CBase";
	m_pDebugNode->pNextNode = nullptr;
	m_pDebugNode->pPrevNode = nullptr;
#endif

}

CBase::~CBase(void)
{

}

unsigned long CBase::GetRefCnt(void) const
{
	return this->m_dwRefCnt;
}

unsigned long CBase::GetKey(void) const
{
	return this->m_dwKey;
}


unsigned long CBase::MakeKey(void)
{
	static unsigned long dwKey = 0;
	return ++dwKey;
}

unsigned long CBase::AddRef(void) 
{
	return ++(this->m_dwRefCnt);
}

unsigned long CBase::Release(void)
{
	if (0 == this->m_dwRefCnt)
	{
		unsigned long dwFreeCnt = 0;

		if (dwFreeCnt = Free())
			return dwFreeCnt;

#ifndef _FINAL
		Set_TopNode();
			m_pDebugNode->Delete();
			m_pDebugNode = nullptr;
#endif

		delete this;

		return 0;
	}
	else
		return this->m_dwRefCnt--;
}
