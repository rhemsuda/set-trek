/*
File Name:		Ship.cpp
Description:	This file holds the definition of a ship and the methods to modify them. This should be abstracted but
				due to time constraints enemy and player information is stored in a Ship, and used appropriately. Works great for purpose.
Programmer:		Kyle Jensen
Date:			April 14, 2017
*/

#include "../Include/Ship.h"


//Function: Ship(float speed, Vector2 startPosition, Vector2 size, TextureHandle texture, int energy, Ability abilities[NUM_ABILITIES])
//Description: This is the constructor for a ship. It initializes all information including texture energy and abilities.
//Returns: void.
Ship::Ship(float speed, Vector2 startPosition, Vector2 size, TextureHandle texture, int energy, Ability abilities[NUM_ABILITIES])
{
	this->speed = speed;
	this->maxspeed = speed;
	this->position = startPosition;
	this->size = size;
	this->destination = startPosition;
	this->texture = texture;
	this->angle = 0.0f;
	this->energy = energy;
	this->maxEnergy = energy;
	this->science = 0;
	this->cooldown = 2;
	this->cooldown_time = time(0) - cooldown;
	this->last_heal = time(0) - 1;
	this->spawnedMinions = false;

	this->abilities[0] = abilities[0];
	this->abilities[1] = abilities[1];
	this->abilities[2] = abilities[2];

	this->abilityShotTime[0] = time(0) - abilities[0].cooldown;
	this->abilityShotTime[1] = time(0) - abilities[1].cooldown;
	this->abilityShotTime[2] = time(0) - abilities[2].cooldown;

	this->boss = false;
}


//Function: MoveTowardDestination(float timeElapsed)
//Description: This method moves the ship in the in the direction of a specific destination.
//Returns: void.
void Ship::MoveTowardDestination(float timeElapsed)
{
	Vector2 diff = destination - this->position;

	//If we are at our destination we dont want to move the ship
	if (Magnitude(diff) > SHIP_NEAR_THRESHOLD)
	{
		diff = Normalize(diff);
		this->position = this->position + diff * this->speed * timeElapsed;

		this->angle = acos(DotProduct(Vector2{ 1.0f, 0.0f }, diff));

		// Dot product is always the smallest angle between 2 vectors, so when we want a value greater than PI
		// we must subtract the smaller angle from 2*PI to get it's reflection
		if (diff.y < 0)
		{
			this->angle = 2 * XM_PI - this->angle;
		}
	}
}
