#include "PixelShader.h"
#include <d3dcompiler.h>

HRESULT PixelShader::Create(ID3D11Device* device, const wchar_t* filePath)
{
    ComPtr<ID3DBlob> errorMessage;

    HRESULT result = D3DCompileFromFile(
        filePath,          // HLSL 파일
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "PSMain",                     // 실행 시작 함수
        "ps_5_0",                     // Vertex Shader 5.0
        D3DCOMPILE_DEBUG,
        0,
        pixelShaderBlob.GetAddressOf(),
        &errorMessage
    );

    if (FAILED(result))
    {
        return result;
    }

    result = device->CreatePixelShader(
        pixelShaderBlob->GetBufferPointer(),
        pixelShaderBlob->GetBufferSize(),
        nullptr,
        pixelShader.GetAddressOf()
    );

    if (FAILED(result))
    {
        return result;
    }

	return S_OK;
}

void PixelShader::Bind(ID3D11DeviceContext* context)
{
    // 색상 계산은 이 픽셀 셰이더를 사용해
    context->PSSetShader(pixelShader.Get(), nullptr, 0);
}