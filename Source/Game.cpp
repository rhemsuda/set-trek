/*
File Name:		Game.cpp
Description:	This file manages the game portion of the application. The player is spawned in with 100 health in the first sector.
				The goal of the game is to fly to planets and collect energy and science while fighting off the enemies. The game
				includes rockets shooting from players and enemies, along with boss fights every 10 rounds.
Programmer:		Kyle Jensen
Date:			April 14, 2017
*/

#include "../Include/Game.h"


#define TimeElapsed 0.016667f


//HUD texture globals
TextureHandle energyIcon;
TextureHandle scienceIcon;
TextureHandle abilityIcons[4];
TextureHandle introBackground;
TextureHandle introLogo;

bool displayLevelNotClear = false;

//Function: GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
//Description: This method gets dynamically compiled into the main.cpp file and can be swapped for real-time debugging performance gains.
//It runs the main game loop and all the game functionality for this application.
//Returns: void.
GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
	//If the game has not been initialized yet, do so
	if (!gameState->initialized)
	{
		srand(time(0));

		//Set screen and tile sizes
		gameState->screenWidth = bufferWidth;
		gameState->screenHeight = bufferHeight;
		gameState->tileWidth = gameState->screenWidth / TILE_SIZE;
		gameState->tileHeight = gameState->screenHeight / TILE_SIZE;

		//Setup the projection matrix
		float fieldOfView = (float)XM_PI / 2.0f;
		float screenAspect = (float)gameState->screenWidth / (float)gameState->screenHeight;

		//Initialize perspective matrices
		MatrixBufferType* perspectiveMatrices = &gameState->perspectiveMatrices;
		perspectiveMatrices->view = XMMatrixIdentity();
		perspectiveMatrices->projection = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, 0.1f, 100.f);
		perspectiveMatrices->world = XMMatrixIdentity();

		//TODO: May not need this here as it is called before drawing
		OverwriteGPUShaderMatrices(deviceContext, gameState->matrixBuffer, perspectiveMatrices);

		//Initialize orthographic matrices
		MatrixBufferType* orthoMatrices = &gameState->orthoMatrices;
		orthoMatrices->view = XMMatrixIdentity();
		orthoMatrices->projection = XMMatrixMultiply(XMMatrixOrthographicLH(gameState->screenWidth, gameState->screenHeight, 0.1f, 100.f), XMMatrixTranslation(-1, -1, 0));
		orthoMatrices->world = XMMatrixIdentity();

		//Set the constant buffer in the vertex shader with the updated values
		deviceContext->VSSetConstantBuffers(0, 1, &gameState->matrixBuffer);

		//Set the vertex buffers to the loaded obj models
		gameState->sphereVertexBuffer = ObjLoader::VertexBufferFromObj(device, "Assets//Models//sphere.obj");
		gameState->quadVertexBuffer = ObjLoader::VertexBufferFromObj(device, "Assets//Models//quad.obj");

		//Initialize planet textures
		TextureHandle* planetTextures = gameState->planetTextures;
		planetTextures[0] = LoadTextureFromTGA(device, deviceContext, "Assets//Textures//planet1.tga");
		planetTextures[1] = LoadTextureFromTGA(device, deviceContext, "Assets//Textures//planet2.tga");
		planetTextures[2] = LoadTextureFromTGA(device, deviceContext, "Assets//Textures//planet3.tga");
		planetTextures[3] = LoadTextureFromTGA(device, deviceContext, "Assets//Textures//planet4.tga");
		planetTextures[4] = LoadTextureFromTGA(device, deviceContext, "Assets//Textures//planet5.tga");
		planetTextures[5] = LoadTextureFromTGA(device, deviceContext, "Assets//Textures//planet6.tga");
		planetTextures[6] = LoadTextureFromTGA(device, deviceContext, "Assets//Textures//planet7.tga");
		planetTextures[7] = LoadTextureFromTGA(device, deviceContext, "Assets//Textures//planet8.tga");
		planetTextures[8] = LoadTextureFromTGA(device, deviceContext, "Assets//Textures//planet9.tga");
		planetTextures[9] = LoadTextureFromTGA(device, deviceContext, "Assets//Textures//planet10.tga");
		planetTextures[10] = LoadTextureFromTGA(device, deviceContext, "Assets//Textures//blackhole.tga");

		//Initialize background textures
		TextureHandle* backgrounds = gameState->backgrounds;
		backgrounds[0] = LoadTextureFromTGA(device, deviceContext, "Assets//Textures//universe1.tga");
		backgrounds[1] = LoadTextureFromTGA(device, deviceContext, "Assets//Textures//universe2.tga");
		backgrounds[2] = LoadTextureFromTGA(device, deviceContext, "Assets//Textures//universe3.tga");
		backgrounds[3] = LoadTextureFromTGA(device, deviceContext, "Assets//Textures//universe4.tga");
		backgrounds[4] = LoadTextureFromTGA(device, deviceContext, "Assets//Textures//universe5.tga");
		backgrounds[5] = LoadTextureFromTGA(device, deviceContext, "Assets//Textures//universe6.tga");

		introBackground = LoadTextureFromTGA(device, deviceContext, "Assets//Textures//introbackground.tga");
		introLogo = LoadTextureFromTGA(device, deviceContext, "Assets//Textures//logo.tga");

		energyIcon = LoadTextureFromTGA(device, deviceContext, "Assets//Textures//energy.tga");
		scienceIcon = LoadTextureFromTGA(device, deviceContext, "Assets//Textures//science.tga");

		abilityIcons[0] = LoadTextureFromTGA(device, deviceContext, "Assets//Textures//ability1icon.tga");
		abilityIcons[1] = LoadTextureFromTGA(device, deviceContext, "Assets//Textures//ability2icon.tga");
		abilityIcons[2] = LoadTextureFromTGA(device, deviceContext, "Assets//Textures//ability3icon.tga");
		abilityIcons[3] = LoadTextureFromTGA(device, deviceContext, "Assets//Textures//ability4icon.tga");

		gameState->playerRocketTextures[0] = LoadTextureFromTGA(device, deviceContext, "Assets//Textures//rocket1_y.tga");
		gameState->playerRocketTextures[1] = LoadTextureFromTGA(device, deviceContext, "Assets//Textures//rocket2_y.tga");
		gameState->playerRocketTextures[2] = LoadTextureFromTGA(device, deviceContext, "Assets//Textures//rocket3_y.tga");
		gameState->enemyRocketTextures[0] = LoadTextureFromTGA(device, deviceContext, "Assets//Textures//rocket1_r.tga");
		gameState->enemyRocketTextures[1] = LoadTextureFromTGA(device, deviceContext, "Assets//Textures//rocket2_r.tga");
		gameState->enemyRocketTextures[2] = LoadTextureFromTGA(device, deviceContext, "Assets//Textures//rocket3_r.tga");
		gameState->enemyRocketTextures[3] = LoadTextureFromTGA(device, deviceContext, "Assets//Textures//laser_beam.tga");

		gameState->explosionTexture = LoadTextureFromTGA(device, deviceContext, "Assets//Textures//explosion.tga");

		gameState->playerTexture = LoadTextureFromTGA(device, deviceContext, "Assets//Textures//ship.tga");
		gameState->enemyTextures[0] = LoadTextureFromTGA(device, deviceContext, "Assets//Textures//enemy1.tga");
		gameState->enemyTextures[1] = LoadTextureFromTGA(device, deviceContext, "Assets//Textures//enemy2.tga");
		gameState->bossTextures[0] = LoadTextureFromTGA(device, deviceContext, "Assets//Textures//enemyboss1.tga");
		gameState->bossTextures[1] = LoadTextureFromTGA(device, deviceContext, "Assets//Textures//enemyboss2.tga");
		gameState->bossTextures[2] = LoadTextureFromTGA(device, deviceContext, "Assets//Textures//enemyboss3.tga");

		//Sound	
		LoadWaveFile("Assets//Audio//spaceship_move.wav", gameState->directSound, &gameState->spaceShipMoveSound);
		LoadWaveFile("Assets//Audio//missile_fire.wav", gameState->directSound, &gameState->missileFireSound);
		LoadWaveFile("Assets//Audio//missile_hit.wav", gameState->directSound, &gameState->missileHitSound);
		LoadWaveFile("Assets//Audio//intro.wav", gameState->directSound, &gameState->introMusic);
		LoadWaveFile("Assets//Audio//background01.wav", gameState->directSound, &gameState->backgroundMusic[0]);
		LoadWaveFile("Assets//Audio//background02.wav", gameState->directSound, &gameState->backgroundMusic[1]);
		LoadWaveFile("Assets//Audio//background03.wav", gameState->directSound, &gameState->backgroundMusic[2]);
		gameState->backgroundMusic[3] = gameState->backgroundMusic[0];
		gameState->backgroundMusic[4] = gameState->backgroundMusic[1];
		gameState->backgroundMusic[5] = gameState->backgroundMusic[2];

		//Initialize player ship
		Vector2 playerStartPos = Vector2{ (float)gameState->tileWidth, (float)gameState->screenHeight / 2.0f };
		Vector2 playerSize = Vector2{ (float)gameState->tileWidth, (float)gameState->tileHeight };
		Ability abilities[NUM_ABILITIES] = { playerRocket1, playerRocket2, playerRocket3 };
		gameState->player = new Ship(SHIP_PLAYER_SPEED, playerStartPos, playerSize, gameState->playerTexture, 2000, abilities);
		gameState->player->energy = 100;

		//Initialize enemy spawn points
		gameState->enemySpawnpoints[0] = Vector2{ (float)gameState->screenWidth - (float)gameState->tileWidth, (float)gameState->screenHeight / 2.f };
		gameState->enemySpawnpoints[1] = Vector2{ (float)gameState->screenWidth - (float)gameState->tileWidth, (float)gameState->screenHeight };
		gameState->enemySpawnpoints[2] = Vector2{ (float)gameState->screenWidth - (float)gameState->tileWidth,  (float)gameState->tileHeight };
		gameState->currentSector = 1;

		//If the game has been started, generate the level
		if (gameState->started)
		{
			GenerateLevel(gameState, deviceContext, device);
		}

		gameState->scienceGathered = 0;
		
        gameState->initialized = true;
    }

	//Prepare draw	
	UINT stride = sizeof(VertexType);
	UINT offset = 0;
	UINT sampleMask = 0xffffffff;
	float clearColor[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    deviceContext->ClearRenderTargetView(renderTargetView, clearColor);
    deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &gameState->matrixBuffer);
	deviceContext->VSSetShader(gameState->vertexShader, 0, 0);
	deviceContext->PSSetShader(gameState->pixelShader, 0, 0);
	deviceContext->OMSetBlendState(gameState->blendState, 0, sampleMask);
	deviceContext->IASetInputLayout(gameState->layout);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deviceContext->RSSetState(gameState->rasterState);

	//Get input mouse x and y relative to the screen width and height
	input.mouse.x = (float)input.mouse.x / (float)bufferWidth * gameState->screenWidth;
	input.mouse.y = (float)input.mouse.y / (float)bufferHeight * gameState->screenHeight;

	//Draw the background universe object
	TextureHandle background = (gameState->levelState == LevelState::Start) ? introBackground : gameState->backgrounds[gameState->backgroundIndex];
	if (gameState->levelState != LevelState::GameOver)
	{
		DrawTexture2D(deviceContext, gameState, background, 0, 0, gameState->screenWidth, gameState->screenHeight, 99, XM_PI);
	}
	
	//Switch between level states
	switch (gameState->levelState)
	{
	case LevelState::Start:
		RunStartGame(input, gameState, deviceContext, device);
		break;
	case LevelState::Discovery:
		RunDiscoveryScene(input, gameState, deviceContext);
		break;
	case LevelState::Exploration:
		RunExplorationScene(input, gameState, deviceContext, device);
		break;
	case LevelState::GameOver:
		RunGameOverScene(input, gameState);
	}	

	// HUD //
	
	//If we are in the playing states, draw icons for energy and abilities
	if (gameState->levelState == LevelState::Discovery || gameState->levelState == LevelState::Exploration)
	{
		DrawTexture2D(deviceContext, gameState, energyIcon, 10, gameState->screenHeight - 50, 40, 40, 1);
		DrawTexture2D(deviceContext, gameState, scienceIcon, 10, gameState->screenHeight - 100, 40, 40, 1, XM_PI);
		DrawTexture2D(deviceContext, gameState, abilityIcons[0], 10, 10, 60, 60, 1, XM_PI);
		DrawTexture2D(deviceContext, gameState, abilityIcons[1], 80, 10, 60, 60, 1, XM_PI);
		DrawTexture2D(deviceContext, gameState, abilityIcons[2], 150, 10, 60, 60, 1, XM_PI);
		DrawTexture2D(deviceContext, gameState, abilityIcons[3], 220, 10, 60, 60, 1, XM_PI);
	}
	else if (gameState->levelState == LevelState::Start)
	{
		DrawTexture2D(deviceContext, gameState, introLogo, (gameState->screenWidth / 2) - 200, gameState->screenHeight - 150, 400, 100, 1, XM_PI);
	}

	//Get counter information as WSTRING
	wstring energy = to_wstring(gameState->player->energy);
	wstring science = to_wstring(gameState->player->science);
	wstring sector = to_wstring(gameState->currentSector);
	wstring notClearMessage = L"You must clear all enemies to move on";

	//Begin drawing with spritebatch
	gameState->spriteBatch->Begin();

	//Draw the discovery scene HUD
	if (gameState->levelState == LevelState::Discovery)
	{
		wstring planetName;
		wstring energyLabel = L"1. Gather energy (";
		wstring scienceLabel = L"2. Gather science (";
		wstring continueLabel = L"3. Continue";
		planetName.assign(gameState->currentPlanet->name.begin(), gameState->currentPlanet->name.end());
		energyLabel.append(to_wstring(gameState->currentPlanet->energy)).append(L")");
		scienceLabel.append(to_wstring(gameState->currentPlanet->science)).append(L")");	
		
		gameState->spriteFontLucida24->DrawString(gameState->spriteBatch, planetName.data(), SimpleMath::Vector2((gameState->screenWidth / 2) - ((planetName.length() * 16) / 2), 50.0f));
		gameState->spriteFontLucida24->DrawString(gameState->spriteBatch, energyLabel.data(), SimpleMath::Vector2((gameState->screenWidth / 2) - ((energyLabel.length() * 16) / 2), gameState->screenHeight - 170.0f));
		gameState->spriteFontLucida24->DrawString(gameState->spriteBatch, scienceLabel.data(), SimpleMath::Vector2((gameState->screenWidth / 2) - ((scienceLabel.length() * 16) / 2), gameState->screenHeight - 120.0f));
		gameState->spriteFontLucida24->DrawString(gameState->spriteBatch, continueLabel.data(), SimpleMath::Vector2((gameState->screenWidth / 2) - ((continueLabel.length() * 16) / 2), gameState->screenHeight - 70.0f));
		
		gameState->spriteFontLucida24->DrawString(gameState->spriteBatch, energy.data(), SimpleMath::Vector2(60, 20));
		gameState->spriteFontLucida24->DrawString(gameState->spriteBatch, science.data(), SimpleMath::Vector2(60, 70));
	}
	else if (gameState->levelState == LevelState::Exploration)
	{
		//If we are near a planet, draw the prompt to press E to warp to it
		if (gameState->nearPlanet)
		{
			wstring interactLabel = L"Press [E] to warp";
			gameState->spriteFontLucida24->DrawString(gameState->spriteBatch, interactLabel.data(), SimpleMath::Vector2((gameState->screenWidth / 2) - ((interactLabel.length() * 16) / 2), gameState->screenHeight - 120.0f));
		}

		gameState->spriteFontLucida24->DrawString(gameState->spriteBatch, energy.data(), SimpleMath::Vector2(60, 20));
		gameState->spriteFontLucida24->DrawString(gameState->spriteBatch, science.data(), SimpleMath::Vector2(60, 70));
		gameState->spriteFontLucida24->DrawString(gameState->spriteBatch, sector.data(), SimpleMath::Vector2(gameState->screenWidth - (sector.length() * 24) - 25, 20));
	
		//If there are still enemies in the sector and we try to leave it, display message
		if (displayLevelNotClear)
			gameState->spriteFontLucida24->DrawString(gameState->spriteBatch, notClearMessage.data(), SimpleMath::Vector2((gameState->screenWidth / 2) - ((notClearMessage.length() * 16) / 2) , 100));
	}
	else if (gameState->levelState == LevelState::Start)
	{
		wstring playLabel = L"Press Enter to Play";
		gameState->spriteFontLucida24->DrawString(gameState->spriteBatch, playLabel.data(), SimpleMath::Vector2((gameState->screenWidth / 2) - ((playLabel.length() * 16) / 2), gameState->screenHeight - 70.0f));
	}
	else if (gameState->levelState == LevelState::GameOver)
	{
		wstring gameOverLabel = L"Game Over";
		wstring sectorLabel = L"Sector ";
		wstring scienceLabel = L"Science Gathered: ";
		wstring menuLabel = L"Press Enter to return to menu";
		sectorLabel.append(to_wstring(gameState->currentSector));
		scienceLabel.append(to_wstring(gameState->scienceGathered));
		gameState->spriteFontLucida56->DrawString(gameState->spriteBatch, gameOverLabel.data(), SimpleMath::Vector2((gameState->screenWidth / 2) - ((gameOverLabel.length() * 50) / 2), 100.0f));
		gameState->spriteFontLucida24->DrawString(gameState->spriteBatch, sectorLabel.data(), SimpleMath::Vector2((gameState->screenWidth / 2) - ((sectorLabel.length() * 16) / 2), gameState->screenHeight / 2));
		gameState->spriteFontLucida24->DrawString(gameState->spriteBatch, scienceLabel.data(), SimpleMath::Vector2((gameState->screenWidth / 2) - ((scienceLabel.length() * 16) / 2), (gameState->screenHeight / 2) + 70.0f));
		gameState->spriteFontLucida24->DrawString(gameState->spriteBatch, menuLabel.data(), SimpleMath::Vector2((gameState->screenWidth / 2) - ((menuLabel.length() * 16) / 2), gameState->screenHeight - 70.0f));
	}

	gameState->spriteBatch->End();
    gameState->swapChain->Present(1, 0);
}



//Function: RunStartGame
//Description: This method runs the logic for the start screen. It starts and stops the intro music, and starts the game
//when the user presses enter.
//Returns: void.
void RunStartGame(Input input, GameState* gameState, ID3D11DeviceContext* deviceContext, ID3D11Device* device)
{
	DWORD status;
	gameState->introMusic->GetStatus(&status);
	if (!(status & DSBSTATUS_PLAYING) && !(status & DSBSTATUS_LOOPING))
	{
		PlayWaveFile(gameState->introMusic, -1000, true);
	}

	gameState->spaceShipMoveSound->Stop();
	gameState->backgroundMusic[gameState->backgroundIndex]->Stop();

	if (input.enter)
	{
		gameState->initialized = false;
		gameState->started = true;
		gameState->introMusic->Stop();
	}
}


//Function: RunExplorationScene(Input input, GameState* gameState, ID3D11DeviceContext* deviceContext, ID3D11Device* device)
//Description: This method runs the exploration scene. This is where the bulk of the action happens. From moving player and
//enemy ships, to shooting different rockets and abilities to collision detection and sound effects.
//Returns: void.
void RunExplorationScene(Input input, GameState* gameState, ID3D11DeviceContext* deviceContext, ID3D11Device* device)
{
	MatrixBufferType* perspectiveMatrices = &gameState->perspectiveMatrices;

	Ship* player = gameState->player;
	vector<Ship*>::iterator enemyIterator = gameState->enemies.begin();

	//Get rid of ships that have been destroyed
	while (enemyIterator != gameState->enemies.end())
	{
		Ship* enemy = (*enemyIterator);

		if (enemy->energy <= 0)
		{
			enemyIterator = gameState->enemies.erase(enemyIterator);
		}
		else
		{
			enemyIterator++;
		}
	}
	enemyIterator = gameState->enemies.begin();

	//If our energy reaches zero we lose.
	if (player->energy <= 0)
	{
		gameState->levelState = LevelState::GameOver;
		return;
	}

	//If it is a boss level, we want to check the boss' health and spawn minions every third of health.
	if (gameState->currentSector % 10 == 0)
	{
		for (size_t i = 0, size = gameState->enemies.size(); i != size; i++)
		{
			Ship* enemy = gameState->enemies.at(i);
			if (enemy->boss && enemy->energy < (enemy->maxEnergy / 3))
			{
				if (!enemy->spawnedMinions)
				{
					enemy->spawnedMinions = true;

					//Ship information for a minion
					Vector2 shipSize = Vector2{ (float)gameState->tileWidth, (float)gameState->tileHeight };
					int minionEnergy = 100 + (100 * (gameState->currentSector / 10));
					Ability minionAbilities[NUM_ABILITIES] = { enemy2Laser, enemy2Laser, enemy2Laser };

					//Spawn minions
					Ship* minion1 = new Ship(SHIP_ENEMY_SPEED, gameState->enemySpawnpoints[1], shipSize, gameState->enemyTextures[1], minionEnergy, minionAbilities);
					Ship* minion2 = new Ship(SHIP_ENEMY_SPEED, gameState->enemySpawnpoints[2], shipSize, gameState->enemyTextures[1], minionEnergy, minionAbilities);

					gameState->enemies.push_back(minion1);
					gameState->enemies.push_back(minion2);
				}
			}
		}
	}

	//If we click the left mouse button we set the players destination to the mouse click position
	if (input.mouse.clickedL)
	{
		//If click is on the window, set the destination
		if ((float)input.mouse.x >= 0 && (float)input.mouse.x <= gameState->screenWidth &&
			(float)input.mouse.y >= 0 && (float)input.mouse.y <= gameState->screenHeight)
		{
			Vector2 newDestination = Vector2{ (float)input.mouse.x, (float)(gameState->screenHeight - input.mouse.y) };
			player->destination = newDestination;
		}
	}

	//If we press 4, we want to check the difference between the last heal and heal the player for 500 energy for a price of 500 science
	if (input.key4)
	{
		if (difftime(time(0), player->last_heal) > 1)
		{
			if (player->science >= 500 && player->energy < player->maxEnergy)
			{
				player->science -= 500;		
				player->energy += 500;
				if (player->energy > player->maxEnergy)
					player->energy = player->maxEnergy;
			}
		}
	}

	//Set the ability index to the key that is pressed and -1 if none is pressed.
	int abilityIndex = (input.key1) ? 0 : (input.key2) ? 1 : (input.key3) ? 2 : -1;

	//If we fired an ability, instantiate a rocket of the ability type and fire it
	if (abilityIndex > -1 && abilityIndex < NUM_ABILITIES)
	{
		Ability ability = player->abilities[abilityIndex];

		//If we have enough science, we can use the ability
		if (player->science >= ability.scienceCost)
		{
			if (difftime(time(0), player->abilityShotTime[abilityIndex]) > ability.cooldown)
			{
				//Set the ability shot time
				player->abilityShotTime[abilityIndex] = time(0);
				
				//Rotate the rocket and fire it out of the front of the player ship
				XMFLOAT3 tempVec;
				XMStoreFloat3(&tempVec, XMVector3Transform(XMVECTOR{ 1.0f, 0.0f, 0.0f }, XMMatrixRotationZ(player->angle)));
				Vector2 direction = Vector2{ tempVec.x, tempVec.y };
				direction = Normalize(direction);

				//Create a rocket
				Rocket* rocket = new Rocket(player->position, ability.rocketSize, direction, gameState->playerRocketTextures[ability.rocketIndex], ability.speed, ability.damage);

				//Add rocket to list of instantiated rockets
				gameState->instantiatedRockets.push_back(rocket);
				player->science -= ability.scienceCost;

				//Play rocket sound
				PlayWaveFile(gameState->missileFireSound, -1500);
			}
		}
	}
		
	//Enemy shooting at player
	for (size_t i = 0, size = gameState->enemies.size(); i != size; i++)
	{
		Ship* enemy = gameState->enemies.at(i);
		
		time_t now = time(0);

		//Enemy needs to cooldown after shooting any rocket (stops from double shooting between rocket types)
		if (difftime(now, enemy->cooldown_time) > enemy->cooldown)
		{	
			enemy->cooldown_time = now;

			for (int i = (gameState->currentSector / 10); i >= 0; i--)
			{
				//If the time between the last shot and now is greater than the shoot rate, shoot again.
				if (difftime(now, enemy->abilityShotTime[i]) > enemy->abilities[i].cooldown)
				{
					Ability ability = enemy->abilities[i];
					enemy->abilityShotTime[i] = now;

					//If the enemy is a boss, we shoot double rockets offset to appear. Otherwise shoot single bullets
					if (enemy->boss)
					{
						Vector2 offset = { 0.0f, 30.0f };
						Vector2 rocket1Direction = Normalize((player->position + offset) - (enemy->position + offset));
						Vector2 rocket2Direction = Normalize((player->position - offset) - (enemy->position - offset));
						Rocket* rocket1 = new Rocket(enemy->position + offset, ability.rocketSize, rocket1Direction, gameState->enemyRocketTextures[ability.rocketIndex], ability.speed, ability.damage);
						Rocket* rocket2 = new Rocket(enemy->position - offset, ability.rocketSize, rocket2Direction, gameState->enemyRocketTextures[ability.rocketIndex], ability.speed, ability.damage);
						rocket1->shooter = 1;
						rocket2->shooter = 1;
						gameState->instantiatedRockets.push_back(rocket1);
						gameState->instantiatedRockets.push_back(rocket2);
					}
					else
					{
						Vector2 newDirection = Normalize(player->position - enemy->position);
						Rocket* rocket = new Rocket(enemy->position, ability.rocketSize, newDirection, gameState->enemyRocketTextures[ability.rocketIndex], ability.speed, ability.damage);
						rocket->shooter = 1;
						gameState->instantiatedRockets.push_back(rocket);
					}

					//Play rocket sound
					PlayWaveFile(gameState->missileFireSound, -1500);

					break;
				}
			}
		}
	}

	//Get the distances between the player and enemy and player to target
	float playerDistanceToTarget = Magnitude(player->destination - player->position);

	//Move the player towards the target
	gameState->player->MoveTowardDestination(TimeElapsed);

	DWORD status;
	gameState->spaceShipMoveSound->GetStatus(&status);

	//If the player is moving (not at their destination) the enemy will approach them
	if (playerDistanceToTarget > SHIP_NEAR_THRESHOLD)
	{
		if (!(status & DSBSTATUS_PLAYING) && !(status & DSBSTATUS_LOOPING))
		{
			PlayWaveFile(gameState->spaceShipMoveSound, 0, true);
		}
	}
	else if ((status & DSBSTATUS_PLAYING) || (status & DSBSTATUS_LOOPING))
	{
		gameState->spaceShipMoveSound->Stop();
	}

	for (size_t i = 0, size = gameState->enemies.size(); i != size; i++)
	{
		Ship* enemy = gameState->enemies.at(i);

		//Move the enemy towards the player
		enemy->destination = player->position;
		enemy->MoveTowardDestination(TimeElapsed);

		float enemyDistFromPlayer = Magnitude(player->position - enemy->position);

		//If the enemy ship gets close enough, TURBOFIRE ROCKETS!
		if (enemyDistFromPlayer < 100)
		{
			enemy->speed = enemy->maxspeed * SHIP_ENEMY_SPEEDBOOST;
		}
		else
		{
			enemy->speed = enemy->maxspeed;
		}

		//If the enemy is close enough to the player, deduct 300 energy and generate a new level
		if (enemyDistFromPlayer < 10)
		{
			player->energy -= 300;
			if (player->energy < 0)
				player->energy = 0;

			GenerateLevel(gameState, deviceContext, device);
		}

	}

	displayLevelNotClear = false;
	//Generate a new level if we reach the end and continue
	if (player->position.x >= gameState->screenWidth - (player->size.x / 2))
	{
		if (gameState->enemies.size() > 0)
		{
			displayLevelNotClear = true;
		}
		else
		{
			gameState->currentSector++;
			GenerateLevel(gameState, deviceContext, device);
		}	
	}

	//Set near planet to false before every time we check if we are near a planet
	gameState->nearPlanet = false;

	//Draw all planets generated by the level
	for (int planetIndex = 0; planetIndex != gameState->planets.size(); planetIndex++)
	{
		Planet* planet = gameState->planets[planetIndex];
		if (!planet->visited)
		{
			//If player collides with a planet
			if (CheckCollision(player->position.x - (player->size.x / 2), player->position.y - (player->size.y / 2), player->size.x, player->size.y,
				planet->tileX, planet->tileY, gameState->tileWidth, gameState->tileHeight))
			{
				gameState->nearPlanet = true;
				if (input.keyE)
				{
					//Load the discovery scene and set planet information for recovery
					gameState->levelState = LevelState::Discovery;
					gameState->currentPlanet = planet;
					gameState->currentPlanetLastPos = planet->position;
					gameState->currentPlanet->position.x = 0;
					gameState->currentPlanet->position.y = 0.2f;
					gameState->currentPlanet->position.z = 2;
					player->destination = player->position;
					gameState->spaceShipMoveSound->Stop();
					return;
				}
			}
		}

		//Update and set position of planet
		planet->SetPosition(perspectiveMatrices);
		planet->Update(TimeElapsed);

		//Draw planet
		OverwriteGPUShaderMatrices(deviceContext, gameState->matrixBuffer, perspectiveMatrices);
		DrawModel(deviceContext, &gameState->sphereVertexBuffer, planet->texture);
	}

	//Iterate through all rockets. We use an iterator because we want to be able to delete rockets when they explode, and continue iterating.
	vector<Rocket*>::iterator it = gameState->instantiatedRockets.begin();
	while (it != gameState->instantiatedRockets.end())
	{
		Rocket* rocket = *it;
		
		//If the rocket hasnt exploded, we can move it and check collisions with the appropriate targets
		if (!rocket->exploded)
		{
			rocket->MoveInDirection(TimeElapsed);

			//If the rocket comes from shooter 0 (player) then we check collisions with enemies, otherwise vice versa
			if (rocket->shooter == 0)
			{
				for (size_t i = 0, size = gameState->enemies.size(); i != size; i++)
				{
					Ship* enemy = gameState->enemies.at(i);

					//Check collisions with rockets, explode and deal damage if a collision is met
					if (CheckCollision(rocket->position.x - (rocket->size.x / 2), rocket->position.y - (rocket->size.y / 2), rocket->size.x, rocket->size.y,
						enemy->position.x - (enemy->size.x / 2), enemy->position.y - (enemy->size.y / 2), enemy->size.x, enemy->size.y))
					{
						rocket->exploded = true;
						enemy->energy -= rocket->damage;

						rocket->explosionTime = time(0);
						rocket->texture = gameState->explosionTexture;
						gameState->missileFireSound->Stop();
						PlayWaveFile(gameState->missileHitSound, -1000);
					}
				}			
			}
			else
			{
				//Check collisions with rockets, explode and deal damage if a collision is met
				if (CheckCollision(rocket->position.x - (rocket->size.x / 2), rocket->position.y - (rocket->size.y / 2), rocket->size.x, rocket->size.y,
					player->position.x - (player->size.x / 2), player->position.y - (player->size.y / 2), player->size.x, player->size.y))
				{
					rocket->exploded = true;
					player->energy -= rocket->damage;

					rocket->explosionTime = time(0);
					rocket->texture = gameState->explosionTexture;
					gameState->missileFireSound->Stop();
					PlayWaveFile(gameState->missileHitSound, -1000);
				}
			}			
			
			//If the rocket goes out of the map, we want to delete it as well
			if (!(rocket->position.x >= 0 - rocket->size.x && rocket->position.x <= gameState->screenWidth + rocket->size.x &&
				rocket->position.y >= 0 - rocket->size.y && rocket->position.y <= gameState->screenHeight + rocket->size.y))
			{
				rocket->exploded = true;
				rocket->explosionTime = time(0);
			}		
		}
		//Wait 1 second after exploded to delete the rocket from the iterator (shows explosion for 1sec)
		else if (difftime(time(0), rocket->explosionTime) > 1)
		{
			it = gameState->instantiatedRockets.erase(it);
			continue;
		}

		//Draw the rocket
		DrawTexture2D(deviceContext, gameState, rocket->texture, rocket->position.x - (rocket->size.x / 2), rocket->position.y - (rocket->size.y / 2), rocket->size.x, rocket->size.y, 10, rocket->angle);
		it++;
	}

	//Iterate through enemies and draw them at their positions
	for (size_t i = 0, size = gameState->enemies.size(); i != size; i++)
	{
		Ship* enemy = gameState->enemies.at(i);
		DrawTexture2D(deviceContext, gameState, enemy->texture, enemy->position.x - (enemy->size.x / 2), enemy->position.y - (enemy->size.y / 2), enemy->size.x, enemy->size.y, 10, enemy->angle);
	}

	//Draw the player ship
	DrawTexture2D(deviceContext, gameState, player->texture, player->position.x - (player->size.x / 2), player->position.y - (player->size.y / 2), player->size.x, player->size.y, 20, player->angle);
}


//Function: RunDiscoveryScene(Input input, GameState* gameState, ID3D11DeviceContext* deviceContext)
//Description: This method runs the discovery scene which allows the player to view a spinning 3d representation of the model,
//along with allowing them to gather a random amount of energy from the planet.
//Returns: void.
void RunDiscoveryScene(Input input, GameState* gameState, ID3D11DeviceContext* deviceContext)
{
	MatrixBufferType* perspectiveMatrices = &gameState->perspectiveMatrices;

	Planet* planet = gameState->currentPlanet;

	//If we press 3 or enter we want to go back to the exploration scene,
	//if the planet is out of resources it becomes a black hole and is no longer interactable
	if (input.key3 || input.enter)
	{
		gameState->levelState = LevelState::Exploration;		
		planet->position = gameState->currentPlanetLastPos;
		if (planet->science == 0 && planet->energy == 0)
		{
			planet->visited = true;
			planet->texture = gameState->planetTextures[BLACK_HOLE_INDEX];
		}
		return;
	}
	else if (input.key1)
	{
		int newEnergyCount = gameState->player->energy + planet->energy;
		if (newEnergyCount > 2000)
		{
			newEnergyCount = 2000;
		}
		gameState->player->energy = newEnergyCount;
		planet->energy = 0;
	}
	else if (input.key2)
	{
		gameState->player->science += planet->science;
		gameState->scienceGathered += planet->science;
		planet->science = 0;
	}

	planet->Update(TimeElapsed);

	//Position and draw the planet in the middle of the screen
	planet->SetPosition(perspectiveMatrices);
	OverwriteGPUShaderMatrices(deviceContext, gameState->matrixBuffer, perspectiveMatrices);
	DrawModel(deviceContext, &gameState->sphereVertexBuffer, planet->texture);

	//Draw the ship beside the planet
	DrawTexture2D(deviceContext, gameState, gameState->player->texture, (gameState->screenWidth - gameState->tileWidth) / 4.0f, (gameState->screenHeight - gameState->tileHeight) / 2.0f, gameState->tileWidth * 2, gameState->tileHeight * 2, 20, 0);
}


//Function: RunGameOverScene(Input input, GameState* gameState)
//Description: This method runs the game over scene which just checks for an enter and brings the player to the start screen.
//Returns: void.
void RunGameOverScene(Input input, GameState* gameState)
{
	if (input.enter)
	{
		gameState->levelState = LevelState::Start;
	}
}


//Function: CheckCollision(int x1, int y1, int width1, int height1, int x2, int y2, int width2, int height2)
//Description: This method uses standard AABB collision detection in order to detect collisions using the position and size of the vectors
//Returns: void.
bool CheckCollision(int x1, int y1, int width1, int height1, int x2, int y2, int width2, int height2)
{
	if (x1 + width1 < x2 || x1 > x2 + width2)
		return false;
	if (y1 + height1 < y2 || y1 > y2 + height2)
		return false;

	return true;
}


#pragma region Level Generation


//Function: InitializeSectorBattle(GameState* gameState)
//Description: This method initializes the ships in the sector depending on what sector it is. It uses an algorithm to get the
//number of enemies, their shooting rates, whether it is a boss level, and the rockets they use. This is my first shot at "Procedural" level design,
//but works more like a pattern than anything :)
//Returns: void.
void InitializeSectorBattle(GameState* gameState)
{
	Vector2 shipSize = Vector2{ (float)gameState->tileWidth, (float)gameState->tileHeight };

	int sector = gameState->currentSector;
	int numberOfEnemies = 1 + (((sector % 10) - 1) / 3);

	//Reposition player
	Vector2 playerStartPos = Vector2{ (float)gameState->tileWidth, (float)gameState->screenHeight / 2.0f };
	gameState->player->position = playerStartPos;
	gameState->player->destination = playerStartPos;
	gameState->player->speed = SHIP_PLAYER_SPEED;

	gameState->enemies.clear();

	//Every 10 levels there is a boss phase that gets increasingly harder
	if (sector % 10 == 0)
	{
		//Boss index and energy
		int bossIndex = (sector / 10 > 2) ? 2 : (sector / 10) - 1;
		int bossEnergy = 2000 + (2000 * (sector / 10));

		//Set the boss abilities to use the boss rockets
		Ability bossAbilities[NUM_ABILITIES] = { bossRocket1, bossRocket2, bossRocket3 };
		
		//Initialize the boss
		Ship* boss = new Ship(0.0f, gameState->enemySpawnpoints[0], shipSize * 2, gameState->bossTextures[bossIndex], bossEnergy, bossAbilities);
		boss->cooldown = 1;
		boss->boss = true;

		//Get minion ability and energy info
		Ability minionAbilities[NUM_ABILITIES] = { enemy2Laser, enemy2Laser, enemy2Laser };
		int minionEnergy = 100 + (100 * (sector / 10));

		//Initialize minions
		Ship* minion1 = new Ship(SHIP_ENEMY_SPEED, gameState->enemySpawnpoints[1], shipSize, gameState->enemyTextures[1], minionEnergy, minionAbilities);
		Ship* minion2 = new Ship(SHIP_ENEMY_SPEED, gameState->enemySpawnpoints[2], shipSize, gameState->enemyTextures[1], minionEnergy, minionAbilities);

		//Add enemies to the list of enemies
		gameState->enemies.push_back(boss);
		gameState->enemies.push_back(minion1);
		gameState->enemies.push_back(minion2);
	}
	else
	{
		//Loop through the number of enemies that we are spawning and get their information based on algorithms
		for (int i = 0; i < numberOfEnemies; i++)
		{
			//Get the speed multiplier. Alternates every 3 for boost of 1, 1.25 and 1.5 then repeated. 
			//Every 10th is disregarded and starts again at the 1
			float speedMultiplier = 0.75f + ((float)((((sector % 10) - 1) % 3) + 1) / 4);
			
			//Get the starting information for the enemy
			Vector2 enemyStartPos = gameState->enemySpawnpoints[i];
			float enemySpeed = SHIP_ENEMY_SPEED * speedMultiplier;
			TextureHandle texture = gameState->enemyTextures[0];
			int energy = 100 + (sector * 25);
			
			//Get enemy abilities
			Ability abilities[3] = { enemy1Rocket1, enemy1Rocket2, enemy1Rocket3 };
			
			//If there are 3 enemies, we want the middle one to be the second type of rocket, with some different traits.
			if (numberOfEnemies == 3 && i == 0)
			{
				enemySpeed += 10;
				texture = gameState->enemyTextures[1];
				energy += 100;
				abilities[0] = enemy2Laser;
				abilities[1] = enemy2Laser;
				abilities[2] = enemy2Laser;
			}

			//Initialize the enemy object and add it to enemies
			Ship* enemy = new Ship(enemySpeed, enemyStartPos, shipSize, texture, energy, abilities);
			gameState->enemies.push_back(enemy);
		}
	}
}


//Function: GenerateLevel()
//Description: This method generates a random level by choosing a random universe background, placing a random number of planets
//and positioning them with randomized values.
//Returns: void.
void GenerateLevel(GameState* gameState, ID3D11DeviceContext* deviceContext, ID3D11Device* device)
{
	//The last background index to stop the previous music that was playing
	int lastBackgroundIndex = gameState->backgroundIndex;
	gameState->backgroundIndex = rand() % NUM_BACKGROUNDS;

	//Clear planets
	vector<Planet*>::iterator it1 = gameState->planets.begin();
	while (it1 != gameState->planets.end())
	{
		delete (*it1);
		gameState->planets.erase(it1);
	}

	//Clear rockets
	vector<Rocket*>::iterator it2 = gameState->instantiatedRockets.begin();
	while (it2 != gameState->instantiatedRockets.end())
	{
		delete (*it2);
		gameState->instantiatedRockets.erase(it2);
	}

	//Loop through all tiles and randomly assign planets to their positions
	for (int tileY = 0; tileY != TILE_SIZE; tileY++)
	{
		for (int tileX = 0; tileX != TILE_SIZE; tileX++)
		{
			if (rand() % 20 == 0)
			{
				if (gameState->planets.size() < MAX_PLANETS)
				{
					XMFLOAT3 newPosition = {};

					//Set the planets position. I toyed around with a lot of these values to get the look and feel we want (THANK YOU REAL-TIME CODE RECOMPILATION)
					newPosition.z = 9;
					newPosition.x = (((float)tileX / (float)TILE_SIZE) * 23.0f) - 10.3f;
					newPosition.y = (((float)tileY / (float)TILE_SIZE) * 17.5f) - 7.9f;

					XMFLOAT3 newRotationAxis = XMFLOAT3{ (float)(rand() % 100), (float)(rand() % 100) , (float)(rand() % 100) };
					float newRotationSpeed = ((float)(rand() % 100) / 100.f) * 0.5f + 0.5f;

					int planetIndex = rand() % (NUM_PLANET_TYPES - 1);

					//Set the texture to a new random planet texture and set its position (Our last planet type is the black hole, dont use it for normal planets (subtract 1))
					TextureHandle newTexture = gameState->planetTextures[planetIndex];
					string newName = gameState->planetNames[planetIndex];

					int newTileX = tileX * gameState->tileWidth;
					int newTileY = tileY * gameState->tileHeight;

					//Initialize energy and science, science goes up per 10 sectors
					int energy = (rand() % 180) + 20;
					int science = (rand() % 350) + 100 + (100 * (gameState->currentSector / 10));

					//Create new planet
					Planet* newPlanet = new Planet(device, deviceContext, newRotationSpeed, newPosition, newTexture);
					newPlanet->rotationAxis = newRotationAxis;
					newPlanet->angle = 0.0f;
					newPlanet->tileX = newTileX;
					newPlanet->tileY = newTileY;
					newPlanet->energy = energy;
					newPlanet->science = science;
					newPlanet->name = newName;

					gameState->planets.push_back(newPlanet);
				}
			}
		}
	}

	//Initialize the sector
	InitializeSectorBattle(gameState);

	//Set start to exploration to start the exploration
	gameState->levelState = LevelState::Exploration;

	//Switch music tracks to the appropriate background music
	gameState->backgroundMusic[lastBackgroundIndex]->Stop();
	PlayWaveFile(gameState->backgroundMusic[gameState->backgroundIndex], -1000, true);
}

#pragma endregion


#pragma region Drawing

// Function: OverwriteGPUShaderMatrices()
// Description: This method overwrites the shader matrices(world, view, projection) with the new mapped resource every time we wish to draw
// a new texture. It also transposes the matrices to prepare them to be rendered.
// Returns: void.
void OverwriteGPUShaderMatrices(ID3D11DeviceContext* deviceContext, ID3D11Buffer* matrixBuffer, MatrixBufferType* matrices)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource = {};
	deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	//Loop through the matrices and set their information from the mapped resource
	MatrixBufferType* shaderMatrices = (MatrixBufferType*)mappedResource.pData;
	for (int i = 0; i < ArrayCount(matrices->array); i++)
	{
		shaderMatrices->array[i] = XMMatrixTranspose(matrices->array[i]);
	}

	deviceContext->Unmap(matrixBuffer, 0);
}

//Function: DrawTexture2D(ID3D11DeviceContext* deviceContext, GameState* gameState, TextureHandle texture, int x, int y, int width, int height, int zOrder, float angle)
//Description: This method initializes the orthographic matrices for the positions of the objects and draws them to a quad.
//Returns: void.
void DrawTexture2D(ID3D11DeviceContext* deviceContext, GameState* gameState, TextureHandle texture, int x, int y, int width, int height, int zOrder, float angle)
{
	gameState->orthoMatrices.world = XMMatrixRotationZ(angle);
	gameState->orthoMatrices.world = XMMatrixMultiply(gameState->orthoMatrices.world, XMMatrixScaling(width / 2, height / 2, 1.0));
	gameState->orthoMatrices.world = XMMatrixMultiply(gameState->orthoMatrices.world, XMMatrixTranslation(x + width / 2, y + height / 2, zOrder));
	OverwriteGPUShaderMatrices(deviceContext, gameState->matrixBuffer, &gameState->orthoMatrices);

	deviceContext->PSSetShaderResources(0, 1, &texture);
	gameState->quadVertexBuffer.Draw(deviceContext);
}


//Function: DrawModel()
//Description: This method draws a planet by setting the shader resources and then drawing the vertex buffer
//Returns: void.
void DrawModel(ID3D11DeviceContext* deviceContext, DXBuffer* vertexBuffer, TextureHandle texture)
{
	deviceContext->PSSetShaderResources(0, 1, &texture);
	vertexBuffer->Draw(deviceContext);
}

#pragma endregion