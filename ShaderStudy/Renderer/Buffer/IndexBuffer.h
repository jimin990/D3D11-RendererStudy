#pragma once

#include <wrl/client.h>
#include <d3d11.h>

using Microsoft::WRL::ComPtr;

class IndexBuffer {

public:
    HRESULT Create(
        ID3D11Device* device,
        const void* indexData,
        UINT dataSize,
        UINT indexCount
    );

    void Bind(ID3D11DeviceContext* context);

private:
    /*
    * 실제 버퍼를 담을 스마트 포인터
    */
    ComPtr<ID3D11Buffer> Buffer;

    UINT indexCount = 0;
};
