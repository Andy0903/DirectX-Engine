#ifndef TEXTUREARRAYCLASS_H
#define TEXTUREARRAYCLASS_H

#include <d3d11.h>
#include <d3dx11tex.h>

class TextureArrayClass
{
public:
	TextureArrayClass();
	TextureArrayClass(const TextureArrayClass&);
	~TextureArrayClass();

	bool Initialize(ID3D11Device*, WCHAR*, WCHAR*);
	void Shutdown();

	ID3D11ShaderResourceView **GetTextureArray();

private:
	ID3D11ShaderResourceView* myTextures[2];
};

#endif