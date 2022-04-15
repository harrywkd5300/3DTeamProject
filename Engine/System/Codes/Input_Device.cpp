#include "..\Headers\Input_Device.h"

IMPLEMENT_SINGLETON(CInput_Device)

CInput_Device::CInput_Device()
	: m_pInputSDK(nullptr)
	, m_pKeyBoard(nullptr)
	, m_pMouse(nullptr)
{
	ZeroMemory(&m_bDownKeyState, sizeof(bool) * 256);
	ZeroMemory(&m_bDownMouseState, sizeof(bool) * 4);
}


CInput_Device::~CInput_Device()
{

}

HRESULT CInput_Device::Ready_Input_Device(HINSTANCE hInst, HWND hWnd)
{
	// 입력장치에 대한 초기화작업을 수행하자.

	//  m_pInputSDK 먼저 생성하자.
	if (FAILED(DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pInputSDK, nullptr)))
		return E_FAIL;

	if (FAILED(Ready_KeyBoard_Device(hWnd)))
		return E_FAIL;

	if (FAILED(Ready_Mouse_Device(hWnd)))
		return E_FAIL;

	return NOERROR;
}

void CInput_Device::Check_InputState(void)
{
	if (nullptr == m_pKeyBoard
		|| nullptr == m_pMouse)
		return;

	for (int i = 0; i < 256; ++i)
	{
		if (m_KeyState[i] & 0x80)
			m_bDownKeyState[i] = true;
		else
			m_bDownKeyState[i] = false;
	}

	for (int i = 0; i < 4; ++i)
	{
		if (m_MouseState.rgbButtons[i] & 0x80)
			m_bDownMouseState[i] = true;
		else
			m_bDownMouseState[i] = false;

	}

	ZeroMemory(&m_KeyState, sizeof(_byte) * 256);
	ZeroMemory(&m_MouseState, sizeof(DIMOUSESTATE));


	// 유저가 입력한 키에해당하는 숫자번째 배열에 음수를 담아준다.
	m_pKeyBoard->GetDeviceState(sizeof(_byte) * 256, m_KeyState);

	m_pMouse->GetDeviceState(sizeof(DIMOUSESTATE), &m_MouseState);

	for (int i = 0; i < 256; ++i)
	{
		if (m_KeyState[i] & 0x80)
		{
			if (m_bDownKeyState[i] == true)
				m_bDownKeyState[i] = false;
			else
				m_bDownKeyState[i] = true;
		}
		else
			m_bDownKeyState[i] = false;
	}
	for (int i = 0; i < 4; ++i)
	{
		if (m_MouseState.rgbButtons[i] & 0x80)
		{
			if (m_bDownMouseState[i] == true)
				m_bDownMouseState[i] = false;
			else
				m_bDownMouseState[i] = true;
		}
		else
			m_bDownMouseState[i] = false;
	}
}

HRESULT CInput_Device::Ready_KeyBoard_Device(HWND hWnd)
{
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, nullptr)))
		return E_FAIL;

	if (FAILED(m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard)))
		return E_FAIL;

	if (FAILED(m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE)))
		return E_FAIL;

	m_pKeyBoard->Acquire();

	return NOERROR;
}

HRESULT CInput_Device::Ready_Mouse_Device(HWND hWnd)
{

	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
		return E_FAIL;

	if (FAILED(m_pMouse->SetDataFormat(&c_dfDIMouse)))
		return E_FAIL;

	if (FAILED(m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE)))
		return E_FAIL;

	m_pMouse->Acquire();

	return NOERROR;
}

_ulong CInput_Device::Free(void)
{
	_ulong		dwRefCnt = 0;

	if (dwRefCnt = Safe_Release(m_pKeyBoard))
	{
		MSG_BOX("KeyBoard Release Failed");
		return dwRefCnt;
	}
	if (dwRefCnt = Safe_Release(m_pMouse))
	{
		MSG_BOX("Mouse Release Failed");
		return dwRefCnt;
	}
	if (dwRefCnt = Safe_Release(m_pInputSDK))
	{
		MSG_BOX("InputSDK Release Failed");
		return dwRefCnt;
	}

	return dwRefCnt;
}
