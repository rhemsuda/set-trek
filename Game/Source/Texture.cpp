/*
File Name:		Texture.cpp
Description:	This file handles all the loading of textures from TGA files. REFERENCES: http://www.rastertek.com/dx11tut05.html
Programmer:		Kyle Jensen
Date:			March 24, 2017
*/

#include "../Include/Texture.h"


//Function: LoadTextureFromTGA()
//Description: This method loads the texture into the TextureHandle by the fileName. TextureHandle is a typedef to ID3D11ShaderResourceView*.
//Returns: TextureHandle = A handle to the texture.
TextureHandle LoadTextureFromTGA(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* fileName)
{
	TextureHandle textureView;
	ID3D11Texture2D* texture;

	unsigned char* tgaData;
	int height = 0;
	int width = 0;
	int bpp = 0;
	int imageSize = 0;

	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT hResult;
	unsigned int rowPitch;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	FILE* ifp;
	TGAHeader tgaFileHeader;
	unsigned char* tgaImageData;

	//Open up the file for reading in binary
	int error = fopen_s(&ifp, fileName, "rb");
	if (error != 0)
		return false;

	//Read in the file header information
	unsigned int count = (unsigned int)fread(&tgaFileHeader, sizeof(TGAHeader), 1, ifp);
	if (count != 1)
		return false;

	//Get the important information from the header
	height = (int)tgaFileHeader.height;
	width = (int)tgaFileHeader.width;
	bpp = (int)tgaFileHeader.bpp;

	//Only support 32 bit densities
	if (bpp != 32)
		return false;

	//Calculate the size of the image data
	imageSize = width * height * 4;

	//Allocate memory for the image data based on the size of the image
	tgaImageData = new unsigned char[imageSize];
	if (!tgaImageData)
		return false;

	//Read the target image data into the buffer
	count = (unsigned int)fread(tgaImageData, 1, imageSize, ifp);
	if (count != imageSize)
		return false;

	error = fclose(ifp);
	if (error != 0)
		return false;

	// Allocate memory for the targa destination data.
	tgaData = new unsigned char[imageSize];
	if (!tgaData)
		return false;

	//Initialize the index into the targa image data
	int k = (width * height * 4) - (width * 4);

	//Initialize the index into the targa destination data array
	int index = 0;

	//Because the TGA image format stores images upside down, we need to load the image data into the tga destination in the correct order
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			//Set tga data
			tgaData[index + 0] = tgaImageData[k + 2];  // Red.
			tgaData[index + 1] = tgaImageData[k + 1];  // Green.
			tgaData[index + 2] = tgaImageData[k + 0];  // Blue
			tgaData[index + 3] = tgaImageData[k + 3];  // Alpha

													   // Increment the indexes into the targa data.
			k += 4;
			index += 4;
		}

		//Set the targa image data index back to the preceding row at the beginning of the column since its reading it in upside down
		k -= (width * 8);
	}

	delete[] tgaImageData;
	tgaImageData = 0;

	//Set up texture description
	textureDesc.Height = height;
	textureDesc.Width = width;
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	//Create empty texture so we can copy our TGA data into it
	hResult = device->CreateTexture2D(&textureDesc, NULL, &texture);

	//Set the row pitch of the TGA image data
	rowPitch = (width * 4) * sizeof(unsigned char);

	//Copy the image data from the TGA data array to the texture
	deviceContext->UpdateSubresource(texture, 0, NULL, tgaData, rowPitch, 0);

	//Set the shader resource view description
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	//Create a shader resource view for the given texture
	hResult = device->CreateShaderResourceView(texture, &srvDesc, &textureView);

	//Generate mipmaps
	deviceContext->GenerateMips(textureView);

	//Release the TGA data array as the texture is successfully loaded into memory
	delete[] tgaData;
	tgaData = NULL;

	return textureView;
}