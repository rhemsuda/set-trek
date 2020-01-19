/*
File Name:		Planet.cpp
Description:	This file holds the definition of a planet and the methods to modify them.
Programmer:		Kyle Jensen
Date:			April 14, 2017
*/

#include "../Include/Planet.h"


//Function: Planet(ID3D11Device* device, ID3D11DeviceContext* deviceContext, float speed, XMFLOAT3 position, TextureHandle texture)
//Description: This is the constructor for a planet. It initializes all values to their default states
//Returns: void.
Planet::Planet(ID3D11Device* device, ID3D11DeviceContext* deviceContext, float speed, XMFLOAT3 position, TextureHandle texture)
{
	this->texture = texture;
	this->rotationSpeed = speed;
	this->position = position;
	this->visited = false;
	this->energy = 0;
	this->science = 0;
	this->name = "Unidentified";
}



//Function: Update()
//Description: This method updates the planets rotation by their rotation speed * timeElapsed
//Returns: void.
void Planet::Update(float timeElapsed)
{
	this->angle += this->rotationSpeed * timeElapsed;
}

//Function: SetPosition()
//Description: This method positions the planet by first rotating, then scaling and finally translating
//Returns: void.
void Planet::SetPosition(MatrixBufferType * perspectiveMatrices)
{
	perspectiveMatrices->world = XMMatrixMultiply(XMMatrixRotationAxis(XMLoadFloat3(&this->rotationAxis), this->angle), XMMatrixRotationX(XM_PI / 2));
	perspectiveMatrices->world = XMMatrixMultiply(perspectiveMatrices->world, XMMatrixScaling(0.85f, 0.85f, 0.85f));
	perspectiveMatrices->world = XMMatrixMultiply(perspectiveMatrices->world, XMMatrixTranslation(this->position.x, this->position.y, this->position.z));
}
