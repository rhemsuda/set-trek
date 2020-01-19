/*
File Name:		Ability.h
Description:	This file contains the definition of the ability struct, along with the static ability variables
				for all the types of abilities.
Programmer:		Kyle Jensen
Date:			March 24, 2017
*/


#include "Vector.h"
struct Ability
{
	int cooldown;
	int damage;
	int speed;
	int rocketIndex;
	int scienceCost;
	Vector2 rocketSize;
};

static Ability playerRocket1 = { 1, 100, 100, 0, 50, Vector2{ 35.0f, 35.0f } };
static Ability playerRocket2 = { 2, 250, 180, 1, 200, Vector2{ 40.0f, 40.0f } };
static Ability playerRocket3 = { 8, 600, 65, 2, 400, Vector2{ 60.0f, 60.0f } };

static Ability enemy1Rocket1 = { 2, 200, 120, 0, 0, Vector2{ 35.0f, 35.0f } };
static Ability enemy1Rocket2 = { 7, 300, 180, 1, 0, Vector2{ 40.0f, 40.0f } };
static Ability enemy1Rocket3 = { 10, 500, 100, 2, 0, Vector2{ 55.0f, 55.0f } };

static Ability enemy2Laser = { 1, 100, 220, 3, 0, Vector2{ 30.0f, 30.0f } };

static Ability bossRocket1 = { 1, 100, 180, 0, 0, Vector2{ 50.0f, 50.0f } };
static Ability bossRocket2 = { 4, 200, 200, 1, 0, Vector2{ 50.0f, 50.0f } };
static Ability bossRocket3 = { 10, 300, 120, 2, 0, Vector2{ 70.0f, 70.0f } };