/*
File Name:		Texture.h
Description:	This file handles all the loading of textures from TGA files. REFERENCES: http://www.rastertek.com/dx11tut05.html
Programmer:		Kyle Jensen
Date:			March 24, 2016
*/


#pragma once

#include <d3d11.h>
#include <stdio.h>

//Typedef this as a TextureHandle because who wants to type this garbage 100x
typedef ID3D11ShaderResourceView* TextureHandle;

//Defined header for a tga file
struct TGAHeader
{
	unsigned char data1[12];
	unsigned short width;
	unsigned short height;
	unsigned char bpp;
	unsigned char data2;
};

TextureHandle LoadTextureFromTGA(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* fileName);