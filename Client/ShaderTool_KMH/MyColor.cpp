// MyColor.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "ShaderTool_KMH.h"
#include "MyColor.h"

#ifndef _WIN32_WCE // Windows CE������ CColorDialog��(��) �������� �ʽ��ϴ�.

// CMyColor

IMPLEMENT_DYNAMIC(CMyColor, CColorDialog)

CMyColor::CMyColor(COLORREF clrInit, DWORD dwFlags, CWnd* pParentWnd) :
	CColorDialog(clrInit, dwFlags, pParentWnd)
{

}

CMyColor::~CMyColor()
{
}


BEGIN_MESSAGE_MAP(CMyColor, CColorDialog)
END_MESSAGE_MAP()



// CMyColor �޽��� ó�����Դϴ�.


#endif // !_WIN32_WCE
