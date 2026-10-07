# Direct3D 11
DirectX에 포함된 3D그래픽 API이다.

DirectX는 그래픽만을 담당하는 것이 아니라, 멀티미디어 기술을 초함하는 큰 묶음이고

그 중 그래픽을 담당하는 것이 Direct3D이다.

## Device
```
ComPtr<ID3D11Device> device;

HRESULT result = D3D11CreateDeviceAndSwapChain(
    nullptr,
    D3D_DRIVER_TYPE_HARDWARE,
    nullptr,
    0,
    nullptr,
    0,
    D3D11_SDK_VERSION,
    &desc,
    swapChain.GetAddressOf(),
    device.GetAddressOf(),
    nullptr,
    context.GetAddressOf()
);
```

Device는 Direct3D의 객체와 GPU 리소스를 생성하는 객체로 그래픽 연산에 필요한 객체들을 생성할 때 사용된다.

앞에 I는 인터페이스를 나타낸다.

```
Vertex Buffer
Index Buffer
Texture
Vertex Shader
Pixel Shader
RenderTargetView
InputLayout
```
와 같은 객체들이 생성된다.

## context

```
ComPtr<ID3D11DeviceContext> context;
```

Context는 앞서 만든 객체들을 실제 렌더링 파이프에 연결, 설정하고 렌더링 명령을 내린다.

# D3D11 렌더링 파이프 라인
D3D11의 렌더링 파이프라인을 핵심 부분은 아래와 같은 순서로 진행된다.
```
IA → VS → Rasterizer → PS → OM
```

IA (Input Assembler) 렌더링 파이프라인의 입력 조립 단계
VS (Vertex Shader): 

렌더링 과정을 간단하게 요약하면, 

1. IA가 읽어올 버텍스 버퍼를 지정한다.
2. 이때 Input layout을 통해서 이 버퍼를 어떻게 읽을지 지정한다.
3. hsls파일을 컴파일하고, 바이트 코드로 변환된 값을 버텍스 셰이더로 지정을 한다.
4. 버텍스 버퍼로 부터 IA가 값을 읽어서 버텍스 셰이더의 입력값으로 넣는다.
5. 만약 constant buffer와 같은 값이 있다면 register()는 GPU 파이프라인의 어느 리소스 슬롯을 사용할 것인가를 지정하고 입력값으로 넣어준다.
6. 버텍스 셰이더에서 SV_POSITION 시스템 시맨틱으로 지정된 반환값은 레스터라이저로 전달되어, 이 위치를 이용해 삼각형을 만들고 어떤 픽셀들이 삼각형에 포함되는지 결정한다.
7. 보간된 값은 픽셀 셰이더로 이동하여 입력 값으로 들어간다.
8. 픽셀 셰이더에서 SV_TARGET 시맨틱으로 지정하여, 타겟으로 지정된 버퍼로 출력이 작성된다.
9. 이때 출력을 지정하는 것은 OM(Output Merger)이다.
10. OM에게 "앞으로 출력 결과는 이 RTV(Render Target View)가 가리키는 Render Target에 기록해"라고 지정하는 것이다.
12. 또한 RTV가 지정하는 것을 RT(Render Target) 이며, 이 값은 Taxture2D값으로 지정을 할 수 있으며, 보통은 BackBuffer을 사용한다.
---수정필요---


## IA (Input Assembler)
렌더링 파이프 라인에서 입력을 조립하는 단계이다.

입력을 조립하는게 추상적이라, 애매할 수 있지만

입력에 필요한 데이터를 지정하고, 입력된 데이터를 어떻게 읽을 지 설정을 한 뒤

어떤 토폴리지로 만들지 지정을 한다.

```
context->IASetVertexBuffers(
    0,
    1,
    &buffer,
    &stride,
    &offset
);
```
buffer를 버텍스버퍼로 지정한다.

이때 버텍스 버퍼를 여러가지로 지정이 될 수 있다.

이때는 buffer을 리스트로 만들어서 값으로 넣으면 된다.

```
context->IASetInputLayout(inputLayout.Get());
```
버퍼 안에 데이터를 어떻게 읽고, 각 값을 어떤 의미로 버텍스 셰이더에 전달할지 정의하는 규칙이다.

이때 사용하는것이 Input Layout으로 버텍스를 어떻게 읽을 지 지정한다.

### Input Layout
```
{
    "POSITION",                     // SemanticName
    0,                              // SemanticIndex
    DXGI_FORMAT_R32G32B32_FLOAT,    // Format
    0,                              // InputSlot
    0,                              // AlignedByteOffset
    D3D11_INPUT_PER_VERTEX_DATA,    // InputSlotClass
    0                               // InstanceDataStepRate
}
```
1. "POSITION" — SemanticName

2. 0 — SemanticIndex
같은 Semantic을 여러 개 사용할 때 구분하는 번호

3. ③ DXGI_FORMAT_R32G32B32_FLOAT — Format
버퍼에서 몇 바이트를 어떤 자료형으로 읽을지 정한다.
```
R32  → float 1개
G32  → float 1개
B32  → float 1개
= 32 × 3 = 96bit = 12byte
```
4. 0 — InputSlot
몇 번 Vertex Buffer에서 읽을 것인가를 지정한다.

5. 0 — AlignedByteOffset
해당 Vertex 안에서 몇 번째 바이트부터 읽을 것인가.
```
struct Vertex
{
    float x, y, z; // 12 byte
    float u, v;    // 8 byte
};
```
6. D3D11_INPUT_PER_VERTEX_DATA — InputSlotClass
이 데이터가 정점마다 바뀌는 데이터인지, 인스턴스마다 바뀌는 데이터인지 정한다.

추후 인스턴싱을 사용한다면, D3D11_INPUT_PER_INSTANCE_DATA를 사용한다.

7. 마지막 0 — InstanceDataStepRate
인스턴싱할 때 사용하는 값
현재는 인스턴싱을 사용하지 않기 때문에 0으로 둔다.

# 버텍스 버퍼란?
버텍스 버퍼는 이름 때문에 점의 위치만을 저장하는 버퍼처럼 느껴지지만 실제로는 각 버텍스에 필요한 속성을 저장하는 버퍼로,

다양한 값이 들어 갈 수 있다.

위치는 속성 중 하나로 UV, normal값등 여러 값들이 버텍스 버퍼로 사용될 수 있다.

버퍼는 GPU가 데이터를 읽을 수 있는 메모리 공간이다.

버퍼를 만들기 위해서는, 우선 4가지가 필요하다.

1. 버퍼 설정

BufferDesc로 버퍼의 종류, 용도, 크기 등을 지정할 수 있다.

2. 버퍼에 담을 데이터

버퍼에 담을 데이터가 필요하다.

버텍스 버퍼라면 버텍스가, 인덱스 버퍼라면 인덱스들이 해당된다.

3. 버퍼 초기 데이터

버퍼에 초기에 담을 데이터를 지정 할 수 있다.

앞서 만든 데이터를 여기에 지정한다.

D3D11_SUBRESOURCE_DATA에 버퍼에 담을 초기 데이터를 지정 할 수 있다.

4. 만들어진 버퍼를 담을 포인터

버퍼가 만들어지면 그 버퍼를 담을 포인터가 필요하다.

## 실제 버퍼 만들기

앞선 준비가 되었다면, 실제로 버퍼를 만들 수 있다.

``
device->CreateBuffer(
    &indexBufferDesc,
    &indexData,
    &indexBuffer
);
``
순서대로, 

버퍼 설정, 버퍼 초기 데이터, 만들어진 버퍼를 저장할 포인터이다.

## 버퍼 사용하기
이렇게 버퍼가 만들어지면, Context의 IA를 통해서 버퍼와 레이아웃을 연결해서

실제로 버퍼를 읽을 수 있도록 설정해준다.

여기서 

```
context->IASetVertexBuffers(
    0,
    1,
    &vertexBuffer,
    &stride,
    &offset
);
```

는 버퍼가 버텍스 버퍼라는 것을 지정을 하며, 0번 슬롯을 사용한다고 지정을 한다.

또한 버퍼 안에 데이터 간격과 크기를 지정할 수 있다.

```
context->IASetInputLayout(inputLayout);
```

이후 레이아웃을 지정하여, 이 버퍼를 실제로 GPU가 어떻게 읽는지 규칙을 지정해 준다.

## IA (Input Assembler)
IA 는 정점 입력을 관리하고 조립한다.

정점 입력에는 버텍스 버퍼, 인덱스 버퍼, 인덱스 레이아웃, 토폴리지가 있다.

## backBuffer
backbuffer는 2d image textue이다.

화면을 출력하는 과정이 출력된다면 부자연스럽기 때문에, 출력하는 화면이 다 그려지고 난 후 화면으로 출력한다.
이때 화면으로 쓰일 화면을 미리 만들어 둔 곳을 backbuffer라고 한다.

backbuffer는 앞서 말한 것 처럼 2d texture기 때문에 이 이미지에 이전 렌더링 결과를 가르켜 화면을 구성한다.
이때 이것을 지정하는게 targerView 이다.

```
context->OMSetRenderTargets(
    1,
    renderTargetView.GetAddressOf(),
    nullptr
);
```
앞으로 렌더링 결과를 이 RTV가 가리키는 Texture에 넣으라는 코드.

이때도 아직 실제 사용자가 보는 화면에 보여지고 있는 상태가 아니다.

## Constant Buffer
Constant Buffer는 CPU값을 GPU로 전달하기 위해 사용되는 Buffer이다.

이때 중요한것은 전달한 값과 HLSL에서 사용할 값을 매칭하는 것이다.
```
struct TransformData
{
    float scale;
    float padding[3];
};
```
로 C++ 코드로 전달할 값을 작성했다면
```
cbuffer TransformBuffer : register(b0)
{
    float scale;
    float3 padding;
};
```
이렇게 사용할 값도 같은 값으로 매칭을 해줘야한다.

cbuffer은 constant buffer에서 이 값으로 선언을 한다는 의미이다.

*추가적으로 constant buffer의 값을 c++ 코드에서 수정했더라도, 이 값이 렌더링에서 적용이 바로 되지 않는다.

그 이유는 CPU와 GPU의 메모리 값이 다르기 때문이다.

처음 초기화를 했을 때, CPU값이 GPU로 이동이 되고 아무리 C++값, 즉 CPU에서 수정을 한다고 해도

이 값은 GPU로 전달이 되지 않는다.

그렇기 때문에 update를 통해서 이 값을 다시 GPU로 전달해야 렌더링에 적용이 된다.
