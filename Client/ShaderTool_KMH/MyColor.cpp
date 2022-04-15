// MyColor.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ShaderTool_KMH.h"
#include "MyColor.h"

#ifndef _WIN32_WCE // Windows CE에서는 CColorDialog이(가) 지원되지 않습니다.

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



// CMyColor 메시지 처리기입니다.


#endif // !_WIN32_WCE
