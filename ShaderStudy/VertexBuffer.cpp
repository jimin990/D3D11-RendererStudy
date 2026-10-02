#include "VertexBuffer.h"

HRESULT VertexBuffer::Create(
    ID3D11Device* device, 
    const void* vertexData, // 문법 중요
    UINT dataSize, 
    UINT stride)
{
    /*
    * 버퍼를 어떻게 만들지 설정문
    */
    D3D11_BUFFER_DESC bufferDesc{};

    /*
    * 정점 배열 전체를 담을 크기
    * 배열 전체를 담을 공간을 할당
    */
    bufferDesc.ByteWidth = dataSize;

    /*
    * 생성할 때 데이터를 넣고, 이후에는 변경하지 않음
    */
    bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;

    /*
    * 정점 데이터를 사용하는 버퍼로 사용
    */
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    /*
    * 처음 넣을 데이터 지정
    */
    D3D11_SUBRESOURCE_DATA initialData1{};
    initialData1.pSysMem = vertexData;

    /*
    * 실제 버퍼를 생성
    */
    HRESULT result = device->CreateBuffer(
        &bufferDesc,    // 앞선 설정대로 버퍼를 생성
        &initialData1,   // 배열의 데이털 복사해서 넣음
        Buffer.GetAddressOf()
    );

    if (FAILED(result))
    {
        return -1;
    }

    this->stride = stride;
    offset = 0;

    return S_OK;
}

HRESULT VertexBuffer::Bind(ID3D11DeviceContext* context)
{
    ID3D11Buffer* buffer = Buffer.Get();

    context->IASetVertexBuffers(
        0,
        1,
        &buffer,
        &stride,
        &offset
    );

    return S_OK;
}
