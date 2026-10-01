#pragma once

#include <wrl/client.h>
#include <d3d11.h>

using Microsoft::WRL::ComPtr;

class VertexBuffer {

public:
	HRESULT CreateVertexBufferCreate(
        ID3D11Device* device,
        const void* vertexData,
        UINT dataSize,
        UINT stride);

private:
    /*
    * 실제 버퍼를 담을 스마트 포인터
    */
    ComPtr<ID3D11Buffer> vertexBuffer;

    UINT stride = 0;
    UINT offset = 0;
};
