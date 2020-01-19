/*
File Name:		Ship.h
Description:	This file holds the definition of a ship and the methods to modify them. This should be abstracted but
				due to time constraints enemy and player information is stored in a Ship, and used appropriately. Works great for purpose.
Programmer:		Kyle Jensen
Date:			April 14, 2017
*/

#pragma once
#pragma warning(disable:4244)

#include "Texture.h"
#include "Vector.h"
#include "Ability.h"
#include <time.h>
#include <DirectXMath.h>

#define SHIP_NEAR_THRESHOLD 2.0f
#define SHIP_PLAYER_SPEED 100.0f
#define SHIP_ENEMY_SPEED 40.0f
#define SHIP_ENEMY_SPEEDBOOST 1.65f;
#define NUM_ABILITIES 3

using namespace DirectX;

class Ship
{
public:
	Vector2 position;
	Vector2 size;
	Vector2 destination;
	TextureHandle texture;
	float speed;
	float maxspeed;
	float angle;

	int maxEnergy;
	int energy;
	int science;

	Ability abilities[NUM_ABILITIES];
	time_t abilityShotTime[NUM_ABILITIES];
	bool boss;
	bool spawnedMinions;
	int cooldown;
	time_t cooldown_time;
	time_t last_heal;

	Ship(float speed, Vector2 startPosition, Vector2 size, TextureHandle texture, int energy, Ability abilities[NUM_ABILITIES]);
	void MoveTowardDestination(float timeElapsed);
};