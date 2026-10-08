#pragma once

#include <wrl/client.h>
#include <d3d11.h>

using Microsoft::WRL::ComPtr;

class VertexShader {

public:
	HRESULT Create(
		ID3D11Device* device,
		const wchar_t* filePath,
		const D3D11_INPUT_ELEMENT_DESC* layout,
		UINT layoutCount
	);

	void Bind(ID3D11DeviceContext* context);

private:
	ComPtr<ID3DBlob>  vertexShaderBlob;

	/*
	* 버텍스 셰이더 객체
	*/
	ComPtr<ID3D11VertexShader> vertexShader;

	/*
	* 입력 레이아웃이란 버텍스 버퍼의 데이터를 어떻게 나눠 읽어서 셰이더에 전달할지 설명서이다.
	*/
	ComPtr<ID3D11InputLayout> inputLayout;
};