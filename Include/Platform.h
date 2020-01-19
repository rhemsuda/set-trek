/*
File Name:		Platform.h
Description:	This file holds all the definitions for platform-wide includes and types
Programmer:		Kyle Jensen
Date:			March 24, 2016
*/

#pragma once

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>
#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <d3d11_1.h>
#include <DirectXMath.h>
#include <D3Dcompiler.h>

#include <vector>
#include <assert.h>
#include <random>
#include <time.h>
#include <string>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

//#include "SpriteBatch.h"
//#include "SpriteFont.h"
//#include "SimpleMath.h"

using namespace std;
using namespace DirectX;
using namespace Assimp;



//Vertex type for shaders
struct VertexType
{
	XMFLOAT3 position;
	XMFLOAT2 texture;
};

//Matrix buffer type for shaders
union MatrixBufferType
{
	struct
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

	XMMATRIX array[3];
};


//Drawing buffer
struct DXBuffer
{
	unsigned int STRIDE = sizeof(VertexType);
	unsigned int OFFSET = 0;

	ID3D11Buffer* data;
	int size;

	void Draw(ID3D11DeviceContext* deviceContext)
	{
		deviceContext->IASetVertexBuffers(0, 1, &data, &STRIDE, &OFFSET);
		deviceContext->Draw(size, 0);
	}
};