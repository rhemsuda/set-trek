/*
File Name:		ObjLoader.h
Description:	This file handles loading vertex buffers from obj files. It parses the obj and returns the vertex buffer.
Programmer:		Kyle Jensen
Date:			April 14, 2017
*/

#pragma once
#include "Platform.h"

class ObjLoader
{
public:

	//Function: VertexBufferFromObj()
	//Description: This method gives us a vertex buffer from an Obj file. It uses the Assimp library to load Obj data into
	//				structures that we can load into our vertices array. It takes the vertex point and texture coordinate for each
	//				vertice and adds them to the vertex buffer.
	//Returns: DXBuffer = The vertex buffer.
	static DXBuffer VertexBufferFromObj(ID3D11Device* device, char* filename)
	{
		DXBuffer vertexBuffer;

		// TODO: just mesh not spheremesh
		DXBuffer* sphereVertexBuffer = &vertexBuffer;

		VertexType* vertices;
		D3D11_BUFFER_DESC vertexBufferDesc;
		D3D11_SUBRESOURCE_DATA vertexData;

		Importer importer;
		const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate);
		aiMesh* sphereMesh = scene->mMeshes[0];

		sphereVertexBuffer->size = sphereMesh->mNumVertices;

		//Loop through all vertices and copy over the vertex and texture coord information to the vertex buffer
		vertices = new VertexType[sphereVertexBuffer->size];
		for (int i = 0; i < sphereVertexBuffer->size; i++)
		{
			XMFLOAT3 vertex = XMFLOAT3{ sphereMesh->mVertices[i].x, sphereMesh->mVertices[i].y, sphereMesh->mVertices[i].z };
			XMFLOAT2 textureCoord = XMFLOAT2{ sphereMesh->mTextureCoords[0][i].x, sphereMesh->mTextureCoords[0][i].y };
			vertices[i].position = vertex;
			vertices[i].texture = textureCoord;
		}

		// Set up the description of the static vertex buffer.
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = sizeof(VertexType) * sphereVertexBuffer->size;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;
		vertexBufferDesc.StructureByteStride = 0;

		// Give the subresource structure a pointer to the vertex data.
		vertexData.pSysMem = vertices;
		vertexData.SysMemPitch = 0;
		vertexData.SysMemSlicePitch = 0;

		// Now create the vertex buffer.
		device->CreateBuffer(&vertexBufferDesc, &vertexData, &sphereVertexBuffer->data);

		//End model definition

		// Release the arrays now that the vertex and index buffers have been created and loaded.
		delete[] vertices;
		vertices = 0;

		return vertexBuffer;
	}

};