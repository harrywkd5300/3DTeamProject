#pragma once


// CMyColor

class CMyColor : public CColorDialog
{
	DECLARE_DYNAMIC(CMyColor)

public:
	CMyColor(COLORREF clrInit = 0, DWORD dwFlags = 0, CWnd* pParentWnd = NULL);
	virtual ~CMyColor();

protected:
	DECLARE_MESSAGE_MAP()
};


