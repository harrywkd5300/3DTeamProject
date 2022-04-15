#include "..\Headers\Graphic_Device.h"

USING(Engine)
IMPLEMENT_SINGLETON(CGraphic_Device)

CGraphic_Device::CGraphic_Device()
	: m_pSDK(nullptr)
	, m_pGraphicDev(nullptr)
	//, m_pD3dFont(nullptr)
{
}

CGraphic_Device::~CGraphic_Device()
{
}

HRESULT CGraphic_Device::Ready_GraphicDev(LPDIRECT3DDEVICE9* ppGraphicDevPtr, HWND hWnd, WINMODE eMode, const _uint & iBackCX, const _uint & iBackCY)
{	
	if (nullptr != m_pSDK
		|| nullptr != m_pGraphicDev)
		return E_FAIL;

	//장치를 자체적으로 할당해준다.
	m_pSDK = Direct3DCreate9(D3D_SDK_VERSION);

	if (nullptr == m_pSDK)
		return E_FAIL;

	//장치의 성능을 조사해주는 변수를 설정한다.
	D3DCAPS9	DeviceCaps;
	ZeroMemory(&DeviceCaps, sizeof(D3DCAPS9));

	// 장지의 성능정보를 엉언돠.
	if (FAILED(m_pSDK->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &DeviceCaps)))
	{
		MSG_BOX("Get_DeviceCaps Failed");
		return E_FAIL;
	}

	_ulong BehaviorFlag = 0;

	if (DeviceCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		BehaviorFlag = D3DCREATE_HARDWARE_VERTEXPROCESSING;		//그래픽 카드를 지원한다면 그래픽카드에서.
	else
		BehaviorFlag = D3DCREATE_SOFTWARE_VERTEXPROCESSING;		//그래픽 카드를 지원X -> CPU

	BehaviorFlag |= D3DCREATE_MULTITHREADED;

	//장치를 생성할때 개발자가 설정해줘야하는 구조체.
	D3DPRESENT_PARAMETERS d3dpp;

	ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));

	d3dpp.BackBufferWidth = iBackCX;			//백버퍼의 가로 크기
	d3dpp.BackBufferHeight = iBackCY;			//백버퍼의 세로 크기
	d3dpp.BackBufferCount = 1;					//백버퍼의 개수.
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality = 0;

	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;

	d3dpp.hDeviceWindow = hWnd;		//윈도우 핸들
	d3dpp.Windowed = eMode;			//전체화면:FALSE / 창모드:TRUE

	d3dpp.EnableAutoDepthStencil = TRUE;
	//다이렉트3D에서 자동으로 깊이/스텐실버퍼를 만들고 관리하기를 원한다면 TRUE

	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	//깊이버퍼가 관리되는 픽셀의 범위를 지정하는 옵션.

	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	//전체화면모드롤 출력하게 될때 모니터의 주사율을 따라가도록 설정하는 옵션이다.
	//주사율

	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	//주사율과 프레임 사이의 간격을 어떻게 할것인가?

	//D3DPRESENT_INTERVAL_IMMEDIATE : 즉시 화면으로 처리를 한다.
	//D3DPRESENT_INTERVAL_DEFAULT : 화면처리하는 간격을 다이렉트에서 결정하게 된다.
	if (FAILED(m_pSDK->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, BehaviorFlag, &d3dpp, &m_pGraphicDev)))
	{
		//m_pD3dDevice : (디바이스객체) 화면에 그림을 그리기 위한 객체.
		MSG_BOX("m_pGraphicDev Created Failed");
		return E_FAIL;
	}

	if (nullptr != ppGraphicDevPtr)
	{
		*ppGraphicDevPtr = m_pGraphicDev;
		m_pGraphicDev->AddRef();
	}

	//m_pD3dFont = C3D_Text::Create(m_pGraphicDev, L"Arial", 15);

	//if (nullptr == m_pD3dFont)
	//{
	//	return E_FAIL;
	//}

	return NOERROR;
}

_ulong Engine::CGraphic_Device::Free(void)
{
	_ulong dwRefCnt = 0;




	if (dwRefCnt += Engine::Safe_Release(m_pGraphicDev))
	{
		MSG_BOX("m_pGraphicDev Release Failed");
		return dwRefCnt;
	}

	if (dwRefCnt += Engine::Safe_Release(m_pSDK))
	{
		MSG_BOX("m_pSDK Release Failed");
		return dwRefCnt;
	}

	return dwRefCnt;
}
