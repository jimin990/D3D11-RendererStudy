#pragma once

#include <wrl/client.h>
#include <d3d11.h>

using Microsoft::WRL::ComPtr;

class VertexBuffer {

public:
    /*
    * vertexData: 사용할 정점 데이터
    * dataSize: 전체 정점 데이터의 크기
    * stride: 한 정점에서 다음 정점까지의 바이트 간격
    */
	HRESULT Create(
        ID3D11Device* device,
        const void* vertexData,
        UINT dataSize,
        UINT stride
    );

    HRESULT Bind(ID3D11DeviceContext* context);

private:
    /*
    * 실제 버퍼를 담을 스마트 포인터
    */
    ComPtr<ID3D11Buffer> Buffer;

    /*
    * stride: 한 정점에서 다음 정점까지의 바이트 간격
    * offset: 버퍼의 어디서 부터 읽기 시작할지
    * 현재 위치 + 컬러 = 24바이트, 그러므로 현재 간격은 24바이트이다.
    */
    UINT stride = 0;
    UINT offset = 0;
};
