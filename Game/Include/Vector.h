/*
File Name:		Vector.h
Description:	This file contains the code definitions for the vector methods
Programmer:		Kyle Jensen
Date:			March 24, 2017
*/


#pragma once

#include <math.h>

struct Vector2
{
	float x;
	float y;

	Vector2 operator+(Vector2& b)
	{
		return Vector2{ x + b.x, y + b.y };
	}
	Vector2 operator-(Vector2& b)
	{
		return Vector2{ x - b.x, y - b.y };
	}
	Vector2 operator*(float s)
	{
		return Vector2{ x * s, y * s };
	}
	Vector2 operator/(float s)
	{
		return Vector2{ x / s, y / s };
	}
};

float DotProduct(Vector2& a, Vector2& b);
float Magnitude(Vector2& a);
Vector2 Normalize(Vector2& a);