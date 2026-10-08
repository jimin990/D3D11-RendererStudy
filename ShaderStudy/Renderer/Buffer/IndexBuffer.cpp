#include "IndexBuffer.h"
#include <d3d11.h>

HRESULT IndexBuffer::Create(
    ID3D11Device* device,
    const void* indexData,
    UINT dataSize,
    UINT indexCount) 
{
    D3D11_BUFFER_DESC indexBufferDesc{};

    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = dataSize;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA data{};
    data.pSysMem = indexData;

    HRESULT result = device->CreateBuffer(
        &indexBufferDesc,
        &data,
        Buffer.GetAddressOf()
    );
    
    if (FAILED(result))
    {
        return result;
    }

    this->indexCount = indexCount;

    return S_OK;
}

void IndexBuffer::Bind(ID3D11DeviceContext* context)
{
    context->IASetIndexBuffer(
        Buffer.Get(),
        DXGI_FORMAT_R32_UINT,
        0
    );
}