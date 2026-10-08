#include "Texture.h"
#include <wrl/client.h>
#include <d3d11.h>
#include <vector>

#pragma comment(lib, "windowscodecs.lib")

using Microsoft::WRL::ComPtr;

HRESULT Texture::LoadFromFile(
    ID3D11Device* device,
    const wchar_t* filePath
)
{
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
    * Window의 Device같은 존재로, WIC 객체를 생성한다.
    */
    HRESULT hr = CoCreateInstance(
        CLSID_WICImagingFactory, // 어떤 객체를 생성할 것인가 = ImagingFactory
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(factory.GetAddressOf())
    );

    if (FAILED(hr))
    {
        return hr;
    }
        
    /*
    * 실제 이미지 파일을 해석하는 객체
    */
    ComPtr<IWICBitmapDecoder> decoder;

    /*
    * 실제 디코더 객체를 생성하고 위 포인터에 저장
    * 지정한 이미지 파일을 해석할 수 있는 객체
    */
    hr= factory->CreateDecoderFromFilename(
        filePath,
        nullptr,
        GENERIC_READ,
        WICDecodeMetadataCacheOnLoad,
        decoder.GetAddressOf()
    );

    if (FAILED(hr))
    {
        return hr;
    }

    /*
    * Decoder가 접근한 이미지의 특정 프레임에 접근하기 위한 객체
    * GIF와 같은 이미지파일은 여러장의 이미지를 가지고 있기 때문에 어떤 이미지를 가져올 지 지정해야한다.
    */
    ComPtr<IWICBitmapFrameDecode> frame;

    /*
    * 0번째 이미지를 가져와서 저장
    */
    hr= decoder->GetFrame(
        0,
        frame.GetAddressOf()
    );

    if (FAILED(hr))
    {
        return hr;
    }

    /*
    * 가져온 이미지의 크기를 저장한다.
    */
    UINT width;
    UINT height;

    hr = frame->GetSize(&width, &height);

    if (FAILED(hr))
    {
        return hr;
    }

    /*
    * 원본 이미지의 pixel format을 우리가 원하는 포맷으로 변환하는 객체
    * 예를 들어 원본이 B G R A일때, R G B A로 순서를 변환 할 수 있다.
    */
    ComPtr<IWICFormatConverter> converter;

    hr = factory->CreateFormatConverter(converter.GetAddressOf());

    if (FAILED(hr))
    {
        return hr;
    }

    hr = converter->Initialize(
        frame.Get(),
        GUID_WICPixelFormat32bppRGBA, // 어떤 Pixel Format으로 변환할 것인가.
        WICBitmapDitherTypeNone,
        nullptr,
        0.0,
        WICBitmapPaletteTypeCustom
    );

    if (FAILED(hr))
    {
        return hr;
    }

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

    hr = converter->CopyPixels(
        nullptr, // 어느 영역에서 복사할지, null인 경우 전부 가져와라
        stride,
        imageSize,
        pixels.data()
    );

    if (FAILED(hr))
    {
        return hr;
    }

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

    hr = device->CreateTexture2D(
        &textureDesc,
        &initialData,
        texture.GetAddressOf()
    );

    if (FAILED(hr))
    {
        return hr;
    }

    /*-------------------------------------여기부터 SRV 설정------------------------------*/

    hr = device->CreateShaderResourceView(
        texture.Get(),
        nullptr,
        srv.GetAddressOf()
    );

    if (FAILED(hr))
    {
        return hr;
    }

    return S_OK;
}

void Texture::Bind(
    ID3D11DeviceContext* context,
    UINT slot
)
{
    context->PSSetShaderResources(
        slot,
        1,
        srv.GetAddressOf()
    );
}