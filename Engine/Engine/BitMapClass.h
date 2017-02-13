//#ifndef BITMAPCLASS_H
//#define BITMAPCLASS_H
//
//#include <d3d11.h>
//#include <d3dx10math.h>
//#include "textureclass.h"
//
//class BitmapClass
//{
//public:
//	BitmapClass();
//	BitmapClass(const BitmapClass&);
//	~BitmapClass();
//
//	bool Initialize(ID3D11Device*, int, int, WCHAR*, int, int);
//	void Shutdown();
//	bool Render(ID3D11DeviceContext*, int, int);
//
//	int GetIndexCount();
//	ID3D11ShaderResourceView* GetTexture();
//
//private:
//	struct VertexType
//	{
//		D3DXVECTOR3 position;
//		D3DXVECTOR2 texture;
//	};
//
//	bool InitializeBuffers(ID3D11Device*);
//	void ShutdownBuffers();
//	bool UpdateBuffers(ID3D11DeviceContext*, int, int);
//	void RenderBuffers(ID3D11DeviceContext*);
//
//	bool LoadTexture(ID3D11Device*, WCHAR*);
//	void ReleaseTexture();
//
//	ID3D11Buffer *myVertexBuffer, *myIndexBuffer;
//	int myVertexCount, myIndexCount;
//	TextureClass *myTexture;
//
//	int myScreenWidth, myScreenHeight;
//	int myBitmapWidth, myBitmapHeight;
//	int myPreviousPosX, myPreviousPosY;
//};
//
//
//#endif