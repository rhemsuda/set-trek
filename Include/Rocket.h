/*
File Name:		Rocket.h
Description:	This file holds the definition for a rocket and the methods to move the rocket
Programmer:		Kyle Jensen
Date:			March 24, 2016
*/

#include "platform.h"
#include "Texture.h"
#include "Vector.h"

class Rocket
{
public:
	float speed;
	int damage;
	Vector2 position;
	Vector2 size;
	Vector2 direction;
	float angle;
	TextureHandle texture;

	bool exploded;
	time_t explosionTime;
	int shooter;

	Rocket(Vector2 position, Vector2 size, Vector2 direction, TextureHandle texture, float speed, int damage);
	void MoveInDirection(float timeElapsed);
};