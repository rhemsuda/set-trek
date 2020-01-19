/*
File Name:		planet.h
Description:	This file holds the definition for the planet struct and the methods associated with it
Programmer:		Kyle Jensen
Date:			March 24, 2016
*/

#pragma once

#include "Platform.h"
#include "Texture.h"
#include <D3Dcompiler.h>
#include <DirectXMath.h>

#define BLACK_HOLE_INDEX 10

class Planet
{
public:
	XMFLOAT3 position;
	XMFLOAT3 rotationAxis;

	float angle;
	float rotationSpeed;

	int tileX;
	int tileY;

	bool visited;

	int energy;
	int science;
	string name;

	TextureHandle texture;

	Planet(ID3D11Device* device, ID3D11DeviceContext* deviceContext, float speed, XMFLOAT3 position, TextureHandle texture);

	void Update(float timeElapsed);
	void SetPosition(MatrixBufferType* perspectiveMatrices);
};

