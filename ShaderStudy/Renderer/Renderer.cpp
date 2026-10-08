#include "Renderer.h"

HRESULT Renderer::Initialize(HWND hwnd)
{
	HRESULT result = CreateDeviceAndSwapChain(hwnd);

    if (FAILED(result))
    {
        return result;
    }

    result = CreateRenderTarget();

    if (FAILED(result))
    {
        return result;
    }

    result = CreateViewport();

    if (FAILED(result))
    {
        return result;
    }

    return S_OK;
}

HRESULT Renderer::CreateDeviceAndSwapChain(HWND hwnd)
{
    /*
   * SwapChain의 설명서, DESC는 Description을 줄임말
   * 여기서 DXGI란 DirectX Graphics Infrastructure를 의미한다.
   * 그래픽 장치와 화면 출력 관련 기능을 담당.
   * Direct3D는 그리기를 담당하고, DXGI는 그 결과를 창에 표시하는 쪽을 담당한다
   */
    DXGI_SWAP_CHAIN_DESC desc{};

    /*
    * 그림을 담을 버퍼의 크기 지정
    */
    desc.BufferDesc.Width = 800;
    desc.BufferDesc.Height = 600;

    /*
    * 픽셀 하나에 색상을 어떻게 지정할 지 지정
    * R8: 빨강 8비트
    * G8: 초록 8비트
    * B8: 파랑 8비트
    * A8: 알파 8비트
    * UNORM: 저장된 정수값을 사용할때 0~1범위로 해석
    *
    * 8 * 4 = 32비트, 즉 4바이트를 사용한다.
    */
    desc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;

    /*
    *
    */
    desc.SampleDesc.Count = 1;

    /*
    * 렌더링 결과를 그려 넣을 버퍼로 사용하겠다고 지정
    */
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

    /*
    * 백 버퍼의 수를 지정
    * 백 버퍼란 그림을 그려두는 공간으로, 현재 화면과 분리해서 미리 다음 화면을 그려둔다.
    * 만약 현재 화면에 다음 화면을 그려버리면 이전화면과 새 화면이 섞여 보일 수 있기 때문이다.
    */
    desc.BufferCount = 1;

    /*
    * 어느 창에 결과를 표기할지 지정
    */
    desc.OutputWindow = hwnd;

    /*
    * 전체화면 독점 모드가 아닌 창모드 사용
    */
    desc.Windowed = TRUE;

    /*
    * 표시 후 버퍼 처리 방식 지정
    * DISCARD의 경우, 이전 내용을 유지하지 않고 다음 화면을 그리도록 한다.
    */
    desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    /*
    * swapChain, device, renderer 한번에 생성됨
    */
    HRESULT result = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &desc,
        swapChain.GetAddressOf(),
        device.GetAddressOf(),
        nullptr,
        context.GetAddressOf()
    );

    if (FAILED(result))
    {
        return result;
    }

    return S_OK;
}

HRESULT Renderer::CreateRenderTarget()
{
    /*
    * COM에서는 인터페이스를 식별하기 위해 IID라는 것을 사용한다.
    * IID_PPV_ARGS  = ID3D11Texture2D 인터페이스로 받고 싶다.
    * 결과를 ID3D11Texture2D 타입으로 backBuffer에 넣어줘. 라는 의미
    * GetBuffer: swapChain이 화면 출력 용으로 가지고 있는 Buffer를 가져온다.
    * BackBuffer는 실제로 Texture2D 리소스이다.
    */
    HRESULT result = swapChain->GetBuffer(
        0,
        IID_PPV_ARGS(backBuffer.GetAddressOf())
    );

    if (FAILED(result))
    {
        return result;

    }

    /*-------------------------------------------------------------------*/

    /*
    * RTV를 만들어달라고 요청하는 함수
    * Get()이란 이미가지고 있는 객체 포인터를 꺼냄
    * GetAddressOf()는 생성 결과 포인터를 써 넣을 자리를 제공
    */
    result = device->CreateRenderTargetView(
        backBuffer.Get(),
        nullptr,
        renderTargetView.GetAddressOf()
    );

    /*
    * OM이란 Output Merger로 렌더링 결과를 출력 대상에 기록하는 단계이다.
    */  
    context->OMSetRenderTargets(
        1,
        renderTargetView.GetAddressOf(),
        nullptr
    );

    return result;
}

HRESULT Renderer::CreateViewport()
{
    // 뷰포트 생성
    D3D11_VIEWPORT viewport{};
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width = 800.0f;
    viewport.Height = 600.0f;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    //뷰포트 연결
    context->RSSetViewports(1, &viewport);

    return S_OK;
}
