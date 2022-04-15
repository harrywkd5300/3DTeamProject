
#include "String_Manager.h"
#include "Graphic_Device.h"
IMPLEMENT_SINGLETON(CString_Manager)


CString_Manager::CString_Manager()
{

}

_ulong CString_Manager::Free()
{
	_ulong dwRefCount = 0;

	for (auto pTchar : m_tcharList)
	{
		Safe_Delete_Array(pTchar);
	}
	for (auto pChar : m_charList)
	{
		Safe_Delete_Array(pChar);
	}
	
	return dwRefCount;
}
