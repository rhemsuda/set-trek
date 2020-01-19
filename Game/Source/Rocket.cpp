/*
File Name:		Rocket.cpp
Description:	This file holds the definition of a rocket class, and the methods to fire them
Programmer:		Kyle Jensen
Date:			April 14, 2017
*/

#include "..\Include\Rocket.h"


//Function: Rocket(Vector2 position, Vector2 size, Vector2 direction, TextureHandle texture, float speed, int damage)
//Description: This is the constructor for a rocket. It initializes all values to their default states
//Returns: void.
Rocket::Rocket(Vector2 position, Vector2 size, Vector2 direction, TextureHandle texture, float speed, int damage)
{
	this->position = position;
	this->size = size;
	this->direction = direction;
	this->texture = texture;
	this->speed = speed;
	this->damage = damage;
	this->exploded = false;
	this->explosionTime = time(0);
	this->shooter = 0;
}


//Function: MoveInDirection(float timeElapsed)
//Description: This method moves the rocket in the direction it is given, and rotates it in that direction.
//Returns: void.
void Rocket::MoveInDirection(float timeElapsed)
{
	this->position = this->position + this->direction * this->speed * timeElapsed;
	this->angle = acos(DotProduct(Vector2{ 1.0f, 0.0f }, this->direction));

	// Dot product is always the smallest angle between 2 vectors, so when we want a value greater than PI
	// we must subtract the smaller angle from 2*PI to get it's reflection
	if (this->direction.y < 0)
	{
		this->angle = 2 * XM_PI - this->angle;
	}
}