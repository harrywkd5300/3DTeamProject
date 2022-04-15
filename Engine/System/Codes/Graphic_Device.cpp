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

	//��ġ�� ��ü������ �Ҵ����ش�.
	m_pSDK = Direct3DCreate9(D3D_SDK_VERSION);

	if (nullptr == m_pSDK)
		return E_FAIL;

	//��ġ�� ������ �������ִ� ������ �����Ѵ�.
	D3DCAPS9	DeviceCaps;
	ZeroMemory(&DeviceCaps, sizeof(D3DCAPS9));

	// ������ ���������� �����.
	if (FAILED(m_pSDK->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &DeviceCaps)))
	{
		MSG_BOX("Get_DeviceCaps Failed");
		return E_FAIL;
	}

	_ulong BehaviorFlag = 0;

	if (DeviceCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		BehaviorFlag = D3DCREATE_HARDWARE_VERTEXPROCESSING;		//�׷��� ī�带 �����Ѵٸ� �׷���ī�忡��.
	else
		BehaviorFlag = D3DCREATE_SOFTWARE_VERTEXPROCESSING;		//�׷��� ī�带 ����X -> CPU

	BehaviorFlag |= D3DCREATE_MULTITHREADED;

	//��ġ�� �����Ҷ� �����ڰ� ����������ϴ� ����ü.
	D3DPRESENT_PARAMETERS d3dpp;

	ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));

	d3dpp.BackBufferWidth = iBackCX;			//������� ���� ũ��
	d3dpp.BackBufferHeight = iBackCY;			//������� ���� ũ��
	d3dpp.BackBufferCount = 1;					//������� ����.
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality = 0;

	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;

	d3dpp.hDeviceWindow = hWnd;		//������ �ڵ�
	d3dpp.Windowed = eMode;			//��üȭ��:FALSE / â���:TRUE

	d3dpp.EnableAutoDepthStencil = TRUE;
	//���̷�Ʈ3D���� �ڵ����� ����/���ٽǹ��۸� ����� �����ϱ⸦ ���Ѵٸ� TRUE

	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	//���̹��۰� �����Ǵ� �ȼ��� ������ �����ϴ� �ɼ�.

	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	//��üȭ����� ����ϰ� �ɶ� ������� �ֻ����� ���󰡵��� �����ϴ� �ɼ��̴�.
	//�ֻ���

	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	//�ֻ����� ������ ������ ������ ��� �Ұ��ΰ�?

	//D3DPRESENT_INTERVAL_IMMEDIATE : ��� ȭ������ ó���� �Ѵ�.
	//D3DPRESENT_INTERVAL_DEFAULT : ȭ��ó���ϴ� ������ ���̷�Ʈ���� �����ϰ� �ȴ�.
	if (FAILED(m_pSDK->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, BehaviorFlag, &d3dpp, &m_pGraphicDev)))
	{
		//m_pD3dDevice : (����̽���ü) ȭ�鿡 �׸��� �׸��� ���� ��ü.
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
