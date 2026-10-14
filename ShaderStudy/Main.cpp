#include <iostream>
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <cstring>
#include <DirectXMath.h>
#include "Renderer/Renderer.h"
#include "Renderer/Buffer/VertexBuffer.h"
#include "Renderer/Buffer/IndexBuffer.h"
#include "Renderer/Shader/VertexShader.h"
#include "Renderer/Shader/PixelShader.h"
#include "Platform/Window.h"
#include "Renderer/Texture/Texture.h"
#include <vector>

/* ComPtr을 사용하기 위한 헤더
*  Comptr은 DirectX 객체를 관리하는 스마트 포인터이다.
*/
#include <wrl/client.h>

/*
* FBX Import를 위한 Assimp 라이브러리 추가
*/
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

/*
* 링커에게 d3d11.lib 라이브러리 연결을 지시한다.
* Direct3D 11 DLL의 함수를 사용하도록 링크를 연결
*/
#pragma comment(lib, "d3d11.lib")

#pragma comment(lib, "d3dcompiler.lib")

using Microsoft::WRL::ComPtr;

/*
struct Vertex
{
    float x, y, z; // 위치
    //float r, g, b; // 색
    float u, v; // uv로 변경
};
*/

using namespace DirectX;

/*
* XMFLOAT3는 DirectX에서 행렬 계산에 적합하게 float3형을 저장하도록 만들어놓은 자료형이다.
*/
struct Vertex
{
    XMFLOAT3 position;
    XMFLOAT2 uv;
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

// 버퍼 크기 지정
TransformData transformData{};

struct TransformData1
{
    XMFLOAT4X4 worldViewProjection;
};

/*
void LoadFBX(const char* filePath, std::vector<Vertex>& InVector)
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(
        filePath,
        aiProcess_Triangulate |
        aiProcess_ConvertToLeftHanded
    );

    wchar_t text[128]{};

    if (!scene || !scene->mRootNode)
    {
        swprintf_s(text, L"FBX Load Failed\n");
        OutputDebugStringW(text);

        return;
    }

    swprintf_s(text, L"FBX Load Success!\n");

    //swprintf_s(text, L"Mesh Count: %d\n", scene->mNumMeshes);

    OutputDebugStringW(text);

    const aiMesh* mesh = scene->mMeshes[0];


    for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
    {
        Vertex vertex{};

        vertex.position = {
            mesh->mVertices[i].x,
            mesh->mVertices[i].y,
            mesh->mVertices[i].z
        };

        if (mesh->HasTextureCoords(0))
        {
            vertex.uv = {
                mesh->mTextureCoords[0][i].x,
                mesh->mTextureCoords[0][i].y
            };
        }

        InVector.push_back(vertex);
    }
}*/

void LoadFBX(
    const char* filePath,
    std::vector<Vertex>& vertices,
    std::vector<UINT>& indices)
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(
        filePath,
        aiProcess_Triangulate |
        aiProcess_ConvertToLeftHanded |
        aiProcess_PreTransformVertices
    );

    vertices.clear();
    indices.clear();

    if (!scene || !scene->mRootNode)
    {
        OutputDebugStringA(importer.GetErrorString());
        return;
    }

    for (unsigned int m = 0; m < scene->mNumMeshes; ++m)
    {
        const aiMesh* mesh = scene->mMeshes[m];
        const UINT vertexOffset = static_cast<UINT>(vertices.size());

        for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
        {
            Vertex vertex{};

            vertex.position = {
                mesh->mVertices[i].x,
                mesh->mVertices[i].y,
                mesh->mVertices[i].z
            };

            if (mesh->HasTextureCoords(0))
            {
                vertex.uv = {
                    mesh->mTextureCoords[0][i].x,
                    mesh->mTextureCoords[0][i].y
                };
            }

            vertices.push_back(vertex);
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
        {
            const aiFace& face = mesh->mFaces[i];

            if (face.mNumIndices != 3)
                continue;

            for (unsigned int j = 0; j < 3; ++j)
                indices.push_back(vertexOffset + face.mIndices[j]);
        }
    }
}

int WINAPI wWinMain(
    HINSTANCE instance,
    HINSTANCE previousInstance,
    PWSTR commandLine,
    int showCommand)
{
    Window window;

    if (!window.Create(
        instance,
        1000,
        1000,
        L"렌더러"
    ))
    {
        return -1;
    }

    /*-------------------------------------여기부터 렌더링 파이프 라인------------------------------*/

    /*-------------------------------------FBX 파일 Importer 파이프 라인------------------------------*/
    std::vector<Vertex> vertices;
    std::vector<UINT> indices;

    LoadFBX(
        "C:/Users/wlals/Desktop/FBX/FBX/HollowBody.fbx",
        vertices,
        indices
    );

    if (vertices.empty() || indices.empty())
        return -1;

    /*-------------------------------------IA(Input Assembler) 과정------------------------------*/

    Renderer renderer{};

    renderer.Initialize(window.GetHwnd());

    /*-------------------------------------여기부터 Texture 설정------------------------------*/

    Texture texture{};

    texture.LoadFromFile(
        renderer.device.Get(),
        L"man.jpg"
    );

    texture.Bind(
        renderer.context.Get(),
        0
    );

    /*-------------------------------------여기부터 샘플러 설정------------------------------*/

    D3D11_SAMPLER_DESC samplerDesc = {};

    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

    ComPtr<ID3D11SamplerState> samplerState;

    HRESULT hr = renderer.device->CreateSamplerState(
        &samplerDesc,
        samplerState.GetAddressOf()
    );

    renderer.context->PSSetSamplers(
        0,
        1,
        samplerState.GetAddressOf()
    );

    /*-------------------------------------여기부터 버텍스 버퍼 설정------------------------------*/
    
    VertexBuffer vertexBuffer{};

    vertexBuffer.Create(
        renderer.device.Get(),
        vertices.data(),
        static_cast<UINT>(vertices.size() * sizeof(Vertex)),
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
        L"Shaders/BasicShader.hlsl",
        layout,
        2
    );

    vertexShader.Bind(renderer.context.Get());

    /*-------------------------------------여기부터 인덱스 버퍼 설정------------------------------*/
   /*
   * 버텍스의 인덱스 값
   * 두개의 점이 중복되기 때문에, 삼각형을 이루는 점 index를 저장한다.   
   * 시계 방향       → 앞면 또는 뒷면
   * 반시계 방향     → 반대쪽 면
   */
    IndexBuffer indexBuffer;

    indexBuffer.Create(
        renderer.device.Get(),
        indices.data(),
        static_cast<UINT>(indices.size() * sizeof(UINT)),
        0
    );

    indexBuffer.Bind(renderer.context.Get());

    /*-------------------------------------여기부터 픽셀 셰이더 설정------------------------------*/

    PixelShader pixelShader{};

    pixelShader.Create(
        renderer.device.Get(),
        L"Shaders/BasicShader.hlsl"
    );

    pixelShader.Bind(renderer.context.Get());

    /*-------------------------------------여기부터 Constant 버텍스 설정------------------------------*/

    //// 버퍼 크기 지정
    //TransformData transformData{};
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

    /*-------------------------------------여기부터 View (카메라) Constant 버텍스 설정------------------------------*/
    XMFLOAT3 minPos = vertices.front().position;
    XMFLOAT3 maxPos = minPos;

    for (const Vertex& vertex : vertices)
    {
        const XMFLOAT3& p = vertex.position;

        if (p.x < minPos.x) minPos.x = p.x;
        if (p.y < minPos.y) minPos.y = p.y;
        if (p.z < minPos.z) minPos.z = p.z;

        if (p.x > maxPos.x) maxPos.x = p.x;
        if (p.y > maxPos.y) maxPos.y = p.y;
        if (p.z > maxPos.z) maxPos.z = p.z;
    }

    XMVECTOR center = XMVectorSet(
        (minPos.x + maxPos.x) * 0.5f,
        (minPos.y + maxPos.y) * 0.5f,
        (minPos.z + maxPos.z) * 0.5f,
        1.0f
    );

    const float halfWidth = (maxPos.x - minPos.x) * 0.5f;
    const float halfHeight = (maxPos.y - minPos.y) * 0.5f;
    const float halfDepth = (maxPos.z - minPos.z) * 0.5f;

    // 현재 Renderer의 뷰포트 크기
    const float aspect = 800.0f / 600.0f;
    const float fovY = XMConvertToRadians(60.0f);
    const float tanHalfFov = tanf(fovY * 0.5f);

    const float distanceForHeight = halfHeight / tanHalfFov;
    const float distanceForWidth = halfWidth / (tanHalfFov * aspect);

    float fitDistance = distanceForHeight > distanceForWidth
        ? distanceForHeight
        : distanceForWidth;

    if (fitDistance < 0.1f)
        fitDistance = 0.1f;

    const float distance = halfDepth + fitDistance * 1.2f;

    XMVECTOR cameraPosition = XMVectorSubtract(
        center,
        XMVectorSet(0.0f, 0.0f, distance, 0.0f)
    );

    XMMATRIX world = XMMatrixIdentity();

    XMMATRIX view = XMMatrixLookAtLH(
        cameraPosition,
        center,
        XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
    );

    XMMATRIX projection = XMMatrixPerspectiveFovLH(
        fovY,
        aspect,
        fitDistance * 0.01f,
        distance + halfDepth + fitDistance
    );

    TransformData1 transform1{};

    XMStoreFloat4x4(
        &transform1.worldViewProjection,
        XMMatrixTranspose(world * view * projection)
    );

    // 행렬 상수 버퍼 생성
    D3D11_BUFFER_DESC constantDesc1{};
    constantDesc1.ByteWidth = sizeof(TransformData1);
    constantDesc1.Usage = D3D11_USAGE_DEFAULT;
    constantDesc1.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    D3D11_SUBRESOURCE_DATA constantInitialData1{};
    constantInitialData1.pSysMem = &transform1;

    ComPtr<ID3D11Buffer> constantBuffer1;

    result = renderer.device->CreateBuffer(
        &constantDesc1,
        &constantInitialData1,
        constantBuffer1.GetAddressOf()
    );

    if (FAILED(result))
        return -1;

    // HLSL의 register(b1)에 연결
    ID3D11Buffer* bufferForVS1 = constantBuffer1.Get();

    renderer.context->VSSetConstantBuffers(
        1,
        1,
        &bufferForVS1
    );
    /*-------------------------------------여기부터 메시지 루프 설정------------------------------*/
    int num{};

    /*
    * 프로그램이 실행되는 동안 메시지를 계속 받아야하기 때문에 반복문을 사용한다.
    */
    while (window.ProcessMessages())
    {
        wchar_t text[128]{};

        swprintf_s(text, L"%d : 렌더링 시작\n", num);
        OutputDebugStringW(text);

        ++num;

        // Constant Buffer 업데이트
        renderer.context->UpdateSubresource(
            constantBuffer.Get(),
            0,
            nullptr,
            &transformData,
            0,
            0
        );

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

        /*renderer.context->DrawIndexed(36, 0, 0);*/

        renderer.context->DrawIndexed(
            static_cast<UINT>(indices.size()),
            0,
            0
        );
        renderer.swapChain->Present(1, 0);
    }

    return 0;
}
