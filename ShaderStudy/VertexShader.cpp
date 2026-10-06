#include "VertexShader.h"
#include <d3d11.h>
#include <d3dcompiler.h>

HRESULT VertexShader::Create(
    ID3D11Device* device,
    const wchar_t* filePath,
    const D3D11_INPUT_ELEMENT_DESC* layout,
    UINT layoutCount)
{
    ComPtr<ID3DBlob> errorBlob;

    HRESULT result = D3DCompileFromFile(
        filePath,          // HLSL 파일
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "VSMain",                     // 실행 시작 함수
        "vs_5_0",                     // Vertex Shader 5.0
        D3DCOMPILE_DEBUG,
        0,
        vertexShaderBlob.GetAddressOf(),
        &errorBlob
    );

    if (FAILED(result))
    {
        return result;
    }

    result = device->CreateInputLayout(
        layout,
        layoutCount,
        vertexShaderBlob->GetBufferPointer(),
        vertexShaderBlob->GetBufferSize(),
        inputLayout.GetAddressOf()
    );

    if (FAILED(result))
    {
        return result;
    }

    result = device->CreateVertexShader(
        vertexShaderBlob->GetBufferPointer(),
        vertexShaderBlob->GetBufferSize(),
        nullptr,
        vertexShader.GetAddressOf()
    );

    if (FAILED(result))
    {
        return result;
    }

    return S_OK;
}

void VertexShader::Bind(ID3D11DeviceContext* context)
{
    /*
    * 앞으로 정점데이터를 읽을 때 사용할 레이아웃 지정
    * IA는 Input Assembler
    */
    context->IASetInputLayout(inputLayout.Get());

    // 삼각형으로 연결하도록 지정한다.
    context->IASetPrimitiveTopology(
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
    );

    // 정점 처리는 이 버텍스 셰이더를 사용해
    context->VSSetShader(vertexShader.Get(), nullptr, 0);
}
