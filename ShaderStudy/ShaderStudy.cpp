#include <iostream>
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <cstring>
#include <vector>
#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexShader.h"
#include "PixelShader.h"

/* ComPtr을 사용하기 위한 헤더
*  Comptr은 DirectX 객체를 관리하는 스마트 포인터이다.
*/
#include <wrl/client.h>

/*
* 윈도우에서 제공하는 이미지 디코딩 라이브러리
*/
#include <wincodec.h>

#pragma comment(lib, "windowscodecs.lib")

/*
* 링커에게 d3d11.lib 라이브러리 연결을 지시한다.
* Direct3D 11 DLL의 함수를 사용하도록 링크를 연결
*/
#pragma comment(lib, "d3d11.lib")

#pragma comment(lib, "d3dcompiler.lib")

using Microsoft::WRL::ComPtr;

struct Vertex
{
    float x, y, z; // 위치
    //float r, g, b; // 색
    float u, v; // uv로 변경
};

/*
* 상수 버퍼
* D3D11의 버퍼는 16바이트여야하기 때문에 빈 자리를 배열로 채움
*/
struct TransformData
{
    float scale;
    float padding[3];
};

/*
* LRESULT: 메시지 처리 결과를 반환하는 자료형
* CALLBACK: Windows가 요구하는 함수 호출 규약
* 
*  HWND hwnd: 메시지가 발생한 창의 핸들
*  UINT message: 어떤 메시지인지 나타내는 번호 
*  WPARAM wParam: 메시지에 딸린 추가 정보
*  LPARAM lParam: 메시지에 딸린 추가 정보
*/
LRESULT CALLBACK WindowProc(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    switch (message) 
    {
    case WM_DESTROY:

        // 창이 파괴되면 메시지 루프에 종료를 알림
        PostQuitMessage(0);
        return 0;
    }

    /*
    * 따로 지정해주지 않은 메시지를 기본 처리 함수에 맡기는 함수
    * 이로서 창 이동, 크기 변경, 닫기 버튼 같은 기본 동작을 하나하나 직접 구현하지 않아도 된다.
    */
    return DefWindowProcW(hwnd, message, wParam, lParam);
}

int WINAPI wWinMain(
    HINSTANCE instance,
    HINSTANCE previousInstance,
    PWSTR commandLine,
    int showCommand)
{
    /*
    * 현재 프로세스의 실행 파일(EXE) 모듈 핸들을 가르킨다.
    * EXE 모듈이란, 현재 실행 중인 EXE의 코드와 데이터라고 생각하면 된다.
    * 프로세스랑 다른 점은 프로세스는 프로그램 전체, 모듈은 그 안에 로드된 구성 요소이다.
    * wWinMain으로 실행하게 되면 인자로 들어오기 때문에 생략한다.
    */
    //HINSTANCE instance = GetModuleHandleW(nullptr);

    /*
    * 창의 기본 설정을 담는 구조체{}는 모두 0으로 초기화 한다는것이다.
    * 여기서 클래스는 c++ 에서 말하는 클래스를 말하는 것이 아니다.
    * 어떤 설정으로 창을 만들지 정해 놓은 틀을 말한다.
    */
    WNDCLASSW wc{};

    /*
    * 창의 메시지를 처리할 함수의 주소를 저장하는 멤버. 즉, 함수 포인터
    * 이 종류의 창에서 메시지가 발생하면 설정한 함수를 호출한다.
    * 이때 함수 호출을 위한 주소를 저장하기 때문에 ()는 붙히지 않는다.
    */
    wc.lpfnWndProc = WindowProc;

    /*
    * 이 창이 속한 모듈을 설정한다.
    * 앞서 만든 모듈을 지정한다.
    */
    wc.hInstance = instance;

    /*
    * 추후 창을 만들때 사용할 이 설정의 이름을 지정한다.
    * 창 제목이랑 다른 개념이다.
    * L 이란 Window의 w버전 함수에서 사용하는 와이드 문자열이라는 표시이다.
    */
    wc.lpszClassName = L"MyWindowClass";

    /*
    * 이 창에서 사용할 마우스 커서를 지정한다.
    * hCursor: 커서 핸들을 저장하는 멤버
    * LoadCursorW: 커서 리소스를 불러오는 함수
    * nullptr을 지정하면 윈도우의 기본 커서를 사용한다.
    * MAKEINTRESOUTCEW: 숫자 리소스 ID를 API에 전달할 수 있는 형태로 바꾼다.
    * 32512: 기본 화살표 커서의 리소스 번호
    */
    wc.hCursor = LoadCursorW(nullptr, MAKEINTRESOURCEW(32512));

    /*
    * 창 내부의 기본 배경을 지정한다.
    * hbrBackcround: 배경을 칠할 브러시의 핸들
    * GetSysColorBrush: Windows 시스템 색상에 해당하는 브러시를 가져온다.
    * COLOR_WINDOW: 창 내부 배경에 사용하는 시스템 색상
    */
    wc.hbrBackground = GetSysColorBrush(COLOR_WINDOW);

    /*
    * 작성한 설정을 Windows에 등록한다.
    */
    RegisterClassW(&wc);

    // 2. 실제 창 생성
    HWND hwnd = CreateWindowExW(
        0,                      // 추가 스타일
        L"MyWindowClass",       // 등록한 창 클래스 이름
        L"첫 번째 창",          // 제목
        WS_OVERLAPPEDWINDOW,    // 일반적인 데스크톱 창 스타일
        CW_USEDEFAULT,          // 시작 X 위치
        CW_USEDEFAULT,          // 시작 Y 위치
        800,                    // 창 전체 너비
        600,                    // 창 전체 높이
        nullptr,                // 부모 창 없음
        nullptr,                // 메뉴 없음
        instance,               // 프로그램 모듈 핸들
        nullptr                 // 추가 전달 데이터 없음
    );

    if (!hwnd)
    {
        // 1은 오류를 나태나는 값
        return 1;
    }

    /*
    * SW_SHOW: 창을 표시하라는 명령
    * 숨기기, 최소화, 최대화 등 여러 명령을 할 수 있다.
    */
    ShowWindow(hwnd, SW_SHOW);

    /*-------------------------------------여기부터 렌더링 파이프 라인------------------------------*/

    /*-------------------------------------IA(Input Assembler) 과정------------------------------*/

    Renderer renderer{};

    renderer.Initialize(hwnd);

    /*-------------------------------------여기부터 WIC 설정------------------------------*/
    /*
    * WIC(Window Imaging Component)란
    * Windows에서 제공하는 WIC관련 선언들이 들어 있는 헤더이다.
    * WIC란 PNG, JPG 같은 이미지 파일을 읽어서 실제 픽셀 데이터로 변환하는 Windows 기능이다.
    * PNG나 JPG는 압축된 이미지 포맷이기 때문에 압축을 해제하고, 해석하는 과정이 필요한데
    * 이 과정을 처리해주는 기능을 제공해주는 라이브러리이다.
    */

    /*
    * 우선 이 부분은 패스, WIC는 Component Object Model 기반이라 Com을 초기화한다고 기억
    */
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    /*
    * WIC 객체들을 만들어주는 Factory 인터페이스
    */
    ComPtr<IWICImagingFactory> factory;

    /*
    * 실제 팩토리를 생성해서 위에 만든 포인터에 저장
    * 나머지는 추후 더 공부
    */
    CoCreateInstance(
        CLSID_WICImagingFactory, // 어떤 객체를 생성할 것인가 = ImagingFactory
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(factory.GetAddressOf())
    );

    /*
    * 실제 이미지 파일을 해석하는 객체
    */
    ComPtr<IWICBitmapDecoder> decoder;

    /*
    * 실제 디코더 객체를 생성하고 위 포인터에 저장
    * 나머지는 추후 공부
    */
    factory->CreateDecoderFromFilename(
        L"man.jpg",
        nullptr,
        GENERIC_READ,
        WICDecodeMetadataCacheOnLoad,
        decoder.GetAddressOf()
    );
    
    /*
    * Decoder가 접근한 이미지의 특정 프레임에 접근하기 위한 객체
    * GIF와 같은 이미지파일은 여러장의 이미지를 가지고 있기 때문에 어떤 이미지를 가져올 지 지정해야한다.
    */
    ComPtr<IWICBitmapFrameDecode> frame;

    /*
    * 0번째 이미지를 가져와서 저장
    */
    decoder->GetFrame(
        0,
        frame.GetAddressOf()
    );

    /*
    * 가져온 이미지의 크기를 저장한다.
    */
    UINT width;
    UINT height;

    frame->GetSize(&width, &height);

    /*
    * 원본 이미지의 pixel format을 우리가 원하는 포맷으로 변환하는 객체
    * 예를 들어 원본이 B G R A일때, R G B A로 순서를 변환 할 수 있다.
    */
    ComPtr<IWICFormatConverter> converter;

    factory->CreateFormatConverter(converter.GetAddressOf());

    converter->Initialize(
        frame.Get(),
        GUID_WICPixelFormat32bppRGBA, // 어떤 Pixel Format으로 변환할 것인가.
        WICBitmapDitherTypeNone,
        nullptr,
        0.0,
        WICBitmapPaletteTypeCustom
    );

    /*
    * stride 는 이미지 한줄이 메모리에 차지하는 바이트 수
    * width = 3
    * height = 2 일때
    * 
    * RGBA 4바이트로 4 * 3 은 12 바이트
    */
    UINT stride = width * 4;
    UINT imageSize = stride * height;

    /*
    * 픽셀을 저장할 vector
    */
    std::vector<BYTE> pixels(imageSize);

    converter->CopyPixels(
        nullptr, // 어느 영역에서 복사할지, null인 경우 전부 가져와라
        stride,
        imageSize,
        pixels.data()
    );
    /*-------------------------------------여기부터 taxture 설정------------------------------*/
    D3D11_TEXTURE2D_DESC textureDesc = {};

    textureDesc.Width = width;
    textureDesc.Height = height;

    textureDesc.MipLevels = 1;

    textureDesc.ArraySize = 1;

    textureDesc.Format =
        DXGI_FORMAT_R8G8B8A8_UNORM;

    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;

    textureDesc.Usage = D3D11_USAGE_DEFAULT;

    textureDesc.BindFlags =
        D3D11_BIND_SHADER_RESOURCE;

    /*
    * 초기 데이터
    */
    D3D11_SUBRESOURCE_DATA initialData = {};

    initialData.pSysMem = pixels.data();

    initialData.SysMemPitch = stride;

    initialData.SysMemSlicePitch = 0;

    ComPtr<ID3D11Texture2D> texture;

    HRESULT hr = renderer.device->CreateTexture2D(
        &textureDesc,
        &initialData,
        texture.GetAddressOf()
    );
    /*-------------------------------------여기부터 SRV 설정------------------------------*/

    /*
    * SRV (
    */
    ComPtr<ID3D11ShaderResourceView> textureSRV;

    hr = renderer.device->CreateShaderResourceView(
        texture.Get(),
        nullptr,
        textureSRV.GetAddressOf()
    );

    renderer.context->PSSetShaderResources(
        0,
        1,
        textureSRV.GetAddressOf()
    );
    /*-------------------------------------여기부터 샘플러 설정------------------------------*/

    D3D11_SAMPLER_DESC samplerDesc = {};

    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

    ComPtr<ID3D11SamplerState> samplerState;

    hr = renderer.device->CreateSamplerState(
        &samplerDesc,
        samplerState.GetAddressOf()
    );

    renderer.context->PSSetSamplers(
        0,
        1,
        samplerState.GetAddressOf()
    );

    /*-------------------------------------여기부터 버텍스 버퍼 설정------------------------------*/
    /*
    * 버텍스의 위치값
    * 버텍스 4개로 변경, 점 3개씩 삼각형을 이룬다.
    * 버텍스 색상까지 함께 들어있다.
    * 버텍스 버퍼 초기값
    */
    Vertex vertices[] =
    {
        // Position              // UV
        { 0.0f,  1.0f, 0.0f,    0.0f, 0.0f }, // 왼쪽 위
        {  1.0f,  1.0f, 0.0f,    1.0f, 0.0f }, // 오른쪽 위
        { 0.0f, 0.0f, 0.0f,    0.0f, 1.0f }, // 왼쪽 아래
        {  1.0f, 0.0f, 0.0f,    1.0f, 1.0f }  // 오른쪽 아래
    };

    VertexBuffer vertexBuffer;

    vertexBuffer.Create(
        renderer.device.Get(),
        vertices,
        sizeof(vertices),
        sizeof(Vertex)
        );

    vertexBuffer.Bind(renderer.context.Get());

    /*-------------------------------------여기부터 InputLayout 설정------------------------------*/
    // 정점 안에 있는 정보 한 항목을 어떻게 읽을지 설명하는 구조체 변수
    // 위치와 색상, 두 항목이 존재하므로 배열의 크기를 2로 지정

    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        {
            "POSITION",                     // 셰이더의 지정된 이름으로 입력 전달
            0,
            DXGI_FORMAT_R32G32B32_FLOAT,    // 32비트 실수 3개로 읽는다.
            0,
            0,                              // 정점의 처음부터 읽는다. 따라서 x,y,z를 읽는다.
            D3D11_INPUT_PER_VERTEX_DATA,
            0
        },

        {
            "TEXCOORD",
            0,
            DXGI_FORMAT_R32G32_FLOAT,
            0,
            12,                             // 앞선 x,y,z가 12바이트를 차지하기 때문에 그 뒤 부터 읽게 설정한다.
            D3D11_INPUT_PER_VERTEX_DATA,
            0
        }
    };

    VertexShader vertexShader{};

    vertexShader.Create(
        renderer.device.Get(),
        L"BasicShader.hlsl",
        layout,
        2
    );

    vertexShader.Bind(renderer.context.Get());

    /*-------------------------------------여기부터 인덱스 버퍼 설정------------------------------*/
   /*
   * 버텍스의 인덱스 값
   * 두개의 점이 중복되기 때문에, 삼각형을 이루는 점 index를 저장한다.
   */
    UINT indices[] =
    {
        0, 1, 2,
        2, 1, 3
    };

    IndexBuffer indexBuffer;

    indexBuffer.Create(
        renderer.device.Get(),
        indices,
        sizeof(indices),
        0
    );

    indexBuffer.Bind(renderer.context.Get());

    /*-------------------------------------여기부터 픽셀 셰이더 설정------------------------------*/

    PixelShader pixelShader{};

    pixelShader.Create(
        renderer.device.Get(),
        L"BasicShader.hlsl"
    );

    pixelShader.Bind(renderer.context.Get());

    /*-------------------------------------여기부터 버텍스 설정------------------------------*/

     // 버퍼 크기 지정
    TransformData transformData{};
    transformData.scale = 1.0f;

    D3D11_BUFFER_DESC constantDesc{};
    constantDesc.ByteWidth = sizeof(TransformData);
    constantDesc.Usage = D3D11_USAGE_DEFAULT;
    constantDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    D3D11_SUBRESOURCE_DATA constantInitialData{};
    constantInitialData.pSysMem = &transformData;

    ComPtr<ID3D11Buffer> constantBuffer;

    HRESULT result = renderer.device->CreateBuffer(
        &constantDesc,
        &constantInitialData,
        constantBuffer.GetAddressOf()
    );

    if (FAILED(result))
    {
        return -1;
    }

    ID3D11Buffer* bufferForVS = constantBuffer.Get();

    renderer.context->VSSetConstantBuffers(
        0,
        1,
        &bufferForVS
    );

    if (FAILED(result))
    {
        return result;
    }

    /*-------------------------------------여기부터 메시지 루프 설정------------------------------*/

    /*
    * 메시지 정보를 보관할 구조체
    */
    MSG msg{};

    int num{};

    /*
    * 프로그램이 실행되는 동안 메시지를 계속 받아야하기 때문에 반복문을 사용한다.
    */
    while (true)
    {
        wchar_t text[128]{};

        swprintf_s(text, L"%d : 렌더링 시작\n", num);
        OutputDebugStringW(text);

        ++num;

        /*
        * 현재 스레드의 메시지 큐에서 메시지를 가져온다.
        * 메시지는 총 3가지 종류로
        * 양수: 일반 메시지
        * 0: 종료 메시지 WM_QUIT
        * -1: 오류 발생
        * GetMessage는 메시지가 올때 까지 계속 기다리는 상태이다.
        
        BOOL result = GetMessageW(&msg, nullptr, 0, 0);

        if (result == -1) // 오류
        {
            return -1;
        }

        if (result == 0) // WM_QUIT 수신
        {
            break;
        }

        */

        if (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                break;

            /*
            * 메시지값을 실제 문자 메시지 값으로 처리 될 수 있도록 변환한다.
            * 원래의 메시지는 어떤 키를 눌렸다는 처리만 가능하지만, 변환을 통해서
            * 어떤 키를 입력받았는지 알 수 있다.
            * 예를 들어 Shift + a 라면 A키가, 그냥 a 하면 a 키로 입력 받도록 할 수 있는 것이다.
            *
            */
            TranslateMessage(&msg);

            /*
            * 가져온 창 메시지를 해당 창의 처리 함수에 전달
            * 아까 설정한 wc.lpfnWndProc = WindowProc; 이곳에 설정
            */
            DispatchMessageW(&msg);
        }
        else
        {
            const float backgroundColor[4] =
            {
                0.1f, 0.2f, 0.5f, 1.0f
            };

            // 1. 백 버퍼를 배경색으로 채움
            renderer.context->ClearRenderTargetView(
                renderer.renderTargetView.Get(),
                backgroundColor
            );

            /*
            * 2. 현재 연결된 버퍼와 셰이더로 삼각형을 그림
            * 버텍스 기준으로 만들기 때문에 인덱스로 만들려면 아래 코드 처럼 해야한다.
            */ 
            //context->Draw(6, 0);

            renderer.context->DrawIndexed(6, 0, 0);
            renderer.swapChain->Present(1, 0);
        }
    }

    return static_cast<int>(msg.wParam);
}
