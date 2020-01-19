/*
File Name:		game.h
Description:	This header file contains all the information for a game, from mouse input to game state to our
				DLL code recompilation methods to our game logic prototypes.
Programmer:		Kyle Jensen
Date:			April 14, 2017
*/

#pragma once

#pragma warning(disable: 4244)

//Define a macro to easily get the count of a static array
#define ArrayCount(array) sizeof(array)/sizeof(array[0])

#define TILE_SIZE 10
#define NUM_PLANET_TYPES 11
#define NUM_BACKGROUNDS 6
#define MAX_PLANETS 10

#include "Platform.h"
#include "ObjLoader.h"
#include "Texture.h"
#include "Ship.h"
#include "Planet.h"
#include "Rocket.h"
#include "Sound.h"

#include "DirectXTK\Inc\SpriteFont.h"
#include "DirectXTK\Inc\SimpleMath.h"
struct MouseInput
{
	bool downL;
	bool downR;
	bool clickedL;
	bool clickedR;
	int x;
	int y;
};

struct Input
{
	MouseInput mouse;

	bool key1;
	bool key2;
	bool key3;
	bool key4;
	bool keyE;
	bool enter;
};

enum LevelState
{
	Start,
	Exploration,
	Discovery,
	GameOver
};


struct GameState
{
	//Buffers
	ID3D11Buffer* matrixBuffer;
	MatrixBufferType* shaderMatrices;
	IDXGISwapChain* swapChain;
	DXBuffer sphereVertexBuffer;
	DXBuffer quadVertexBuffer;
	MatrixBufferType perspectiveMatrices;
	MatrixBufferType orthoMatrices;

	//Fonts
	SpriteBatch* spriteBatch;
	SpriteFont* spriteFontLucida24;
	SpriteFont* spriteFontLucida56;
	
	//Drawing stuff
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* layout;
	ID3D11RasterizerState* rasterState;
	ID3D11SamplerState* sampleState;
	ID3D11BlendState* blendState;

	//Sound related items
	IDirectSound8* directSound;
	IDirectSoundBuffer* primaryBuffer;
	SoundHandle backgroundMusic[NUM_BACKGROUNDS];
	SoundHandle introMusic;
	SoundHandle spaceShipMoveSound;
	SoundHandle missileFireSound;
	SoundHandle missileHitSound;

	LevelState levelState;

	string planetNames[NUM_PLANET_TYPES] = { "Flarvis 5OW", "Sporia QR5", "Anides", "Saturn", "Earth", "Zumia", "Neptune", "Pluto", "Kestoia", "Xaglara" };
	TextureHandle backgrounds[NUM_BACKGROUNDS];
	TextureHandle planetTextures[NUM_PLANET_TYPES];
	size_t backgroundIndex;

	//Ship textures
	TextureHandle playerTexture;
	TextureHandle enemyTextures[2];
	TextureHandle bossTextures[3];

	//Rocket textures
	TextureHandle playerRocketTextures[3]; 
	TextureHandle enemyRocketTextures[4];
	TextureHandle explosionTexture;

	std::vector<Planet*> planets;
	std::vector<Rocket*> instantiatedRockets;

	int scienceGathered;

	Ship* player;
	std::vector<Ship*> enemies;

	Vector2 enemySpawnpoints[3];

	//Planet info
	Planet* currentPlanet;
	XMFLOAT3 currentPlanetLastPos;
	bool nearPlanet;
	bool visitingPlanet;
	int currentSector;
	
	int screenWidth;
	int screenHeight;
	int tileWidth;
	int tileHeight;

	//Whether this game state has been initialized
	bool started;
	bool initialized;
};

//Game related prototypes
void RunStartGame(Input input, GameState* gameState, ID3D11DeviceContext* deviceContext, ID3D11Device* device);
void RunExplorationScene(Input input, GameState* gameState, ID3D11DeviceContext* deviceContext, ID3D11Device* device);
void RunDiscoveryScene(Input input, GameState* gameState, ID3D11DeviceContext* deviceContext);
void RunGameOverScene(Input input, GameState* gameState);
bool CheckCollision(int x1, int y1, int width1, int height1, int x2, int y2, int width2, int height2);

//Level related prototypes
void InitializeSectorBattle(GameState* gameState);
void GenerateLevel(GameState* gameState, ID3D11DeviceContext* deviceContext, ID3D11Device* device);

//Drawing related prototypes
void OverwriteGPUShaderMatrices(ID3D11DeviceContext* deviceContext, ID3D11Buffer* matrixBuffer, MatrixBufferType* matrices);
void DrawTexture2D(ID3D11DeviceContext* deviceContext, GameState* gameState, TextureHandle texture, int x, int y, int width, int height, int zOrder, float angle = 0.0f);
void DrawModel(ID3D11DeviceContext* deviceContext, DXBuffer* vertexBuffer, TextureHandle texture);

//1. Define a macro for the definition of the GameUpdateAndRender function pointer.
//2. Create an extern "C" variable for the GameUpdateAndRender function pointer.
//3. Define the type as game_update_and_render
#define GAME_UPDATE_AND_RENDER(name) void name(GameState* gameState, ID3D11DeviceContext* deviceContext, ID3D11Device* device, ID3D11RenderTargetView* renderTargetView, ID3D11DepthStencilView* depthStencilView, int bufferWidth, int bufferHeight, Input input)
extern "C" GAME_UPDATE_AND_RENDER(GameUpdateAndRender);
typedef GAME_UPDATE_AND_RENDER(_GameUpdateAndRender);