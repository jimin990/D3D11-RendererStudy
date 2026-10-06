#pragma once

#include <wrl/client.h>
#include <d3d11.h>

using Microsoft::WRL::ComPtr;

class Renderer {

public:
    HRESULT Initialize(HWND hwnd);

    //변경해여야함
public:

    HRESULT CreateDeviceAndSwapChain(HWND hwnd);

    HRESULT CreateRenderTarget();

    HRESULT CreateViewport();
    /*
    * 필요한 자원을 만든다. 예) 버퍼, 셰이더, RTV 생성
    */
    ComPtr<ID3D11Device> device;

    /*
    * 그린 화면을 창에 표시하기 위한 버퍼들을 관리하는 객체
    * 백버퍼를 관리하고, 완성된 화면을 표시하도록 처리하는 객체
    * 화면에 표시할 taxture들을 관리
    * swapchain은 화면 출력을 위한 taxture을 관리하고 있으며, 이것이 backbuffer이다.
    */
    ComPtr<IDXGISwapChain> swapChain;

    /*
    * 자원을 연결하고 작업을 요청한다.
    */
    ComPtr<ID3D11DeviceContext> context;

    /*
    * 백버퍼를 가르킬 스마트 포인터, 백버퍼는 2차원 이미지 자원이다.
    * 백버퍼는 swapchain이 관리하는 화면 출력용 화면으로, 이후 RTV를 이용해서 GPU가 화면을 그리는 대상으로 지정한다.
    */
    ComPtr<ID3D11Texture2D> backBuffer;

    ComPtr<ID3DBlob> pixelShaderCode;

    /*
    * RTV를 가르킬 스마트 포인터
    * RTV란 taxture를 RenderTarget으로 사용하겠다고 지정하는 View이다.
    * 그리고 RenderTarget이란 실제로 GPU가 렌더링 결과를 써 넣는 대상이다.
    * 즉, Texture를 렌더링 출력 대상으로 사용하기 위한 View
    */
    ComPtr<ID3D11RenderTargetView> renderTargetView;
};
