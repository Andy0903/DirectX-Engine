#include "TextureArrayClass.h"

TextureArrayClass::TextureArrayClass()
{
	myTextures[0] = nullptr;
	myTextures[1] = nullptr;
}

TextureArrayClass::~TextureArrayClass()
{
}

bool TextureArrayClass::Initialize(ID3D11Device* aDevice, WCHAR* aFileName1, WCHAR* aFileName2)
{
	HRESULT result;

	result = D3DX11CreateShaderResourceViewFromFile(aDevice, aFileName1, nullptr, nullptr, &myTextures[0], nullptr);
	if (FAILED(result)) { return false; }

	result = D3DX11CreateShaderResourceViewFromFile(aDevice, aFileName2, nullptr, nullptr, &myTextures[1], nullptr);
	if (FAILED(result)) { return false; }

	return true;
}

void TextureArrayClass::Shutdown()
{
	if (myTextures[0])
	{
		myTextures[0]->Release();
		myTextures[0] = nullptr;
	}

	if (myTextures[1])
	{
		myTextures[1]->Release();
		myTextures[1] = nullptr;
	}
}

ID3D11ShaderResourceView** TextureArrayClass::GetTextureArray()
{
	return myTextures;
}