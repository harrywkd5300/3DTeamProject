
// ShaderTool_KMH.h : ShaderTool_KMH ���� ���α׷��� ���� �� ��� ����
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"       // �� ��ȣ�Դϴ�.


// CShaderTool_KMHApp:
// �� Ŭ������ ������ ���ؼ��� ShaderTool_KMH.cpp�� �����Ͻʽÿ�.
//

class CShaderTool_KMHApp : public CWinApp
{
public:
	CShaderTool_KMHApp();


// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CShaderTool_KMHApp theApp;
