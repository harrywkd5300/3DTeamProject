#include "..\Headers\Observer.h"



CObserver::CObserver()
{
	m_pSubject_Manager = CSubject_Manager::GetInstance();
	m_pSubject_Manager->AddRef();
}

_ulong CObserver::Free()
{
	Safe_Release(m_pSubject_Manager);

	return _ulong();
}
