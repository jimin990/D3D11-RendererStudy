#pragma once

#include <wrl/client.h>
#include <d3d11.h>

using Microsoft::WRL::ComPtr;

class PixelShader
{
public:
    HRESULT Create(
        ID3D11Device* device,
        const wchar_t* filePath
    );

    void Bind(ID3D11DeviceContext* context);

private:
    /*
    * 픽셀 셰이더란 삼각형 안에 어떤 색으로 표현을 할지 지정
    */
    ComPtr<ID3D11PixelShader> pixelShader;

    ComPtr<ID3DBlob> pixelShaderBlob;
};

