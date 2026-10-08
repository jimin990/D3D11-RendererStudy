#pragma once

#include <d3d11.h>
/*
* 윈도우에서 제공하는 이미지 디코딩 라이브러리
*/
#include <wincodec.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

class Texture
{
public:
	HRESULT LoadFromFile(
		ID3D11Device* device,
		const wchar_t* filePath
	);

	void Bind(
		ID3D11DeviceContext* context,
		UINT slot
	);

	UINT GetWidth() const { return width; }
	UINT GetHeight() const { return height; }

private:
	UINT width = 0;
	UINT height = 0;

	ComPtr<ID3D11Texture2D> texture;

	/*
	* SRV(Shader Resource View)
	*/
	ComPtr<ID3D11ShaderResourceView> srv;
};

