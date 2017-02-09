//#include "TextureClass.h"
//
//TextureClass::TextureClass()
//{
//	myTexture = nullptr; 
//}
//
//TextureClass::TextureClass(const TextureClass& aOther)
//{
//}
//
//TextureClass::~TextureClass()
//{
//}
//
//bool TextureClass::Initialize(ID3D11Device* aDevice, WCHAR* aFileName)
//{
//	HRESULT result;
//
//	result = D3DX11CreateShaderResourceViewFromFile(aDevice, aFileName, nullptr, nullptr, &myTexture, nullptr);
//	
//	if (FAILED(result))
//	{
//		return false;
//	}
//
//	return true;
//}
//
//void TextureClass::Shutdown()
//{
//	if (myTexture)
//	{
//		myTexture->Release();
//		myTexture = nullptr;
//	}
//}
//
//ID3D11ShaderResourceView* TextureClass::GetTexture()
//{
//	return myTexture;
//}