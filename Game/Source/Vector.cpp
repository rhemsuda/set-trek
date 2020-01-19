/*
File Name:		Vector.cpp
Description:	This file contains the code definitions for the vector methods
Programmer:		Kyle Jensen
Date:			March 24, 2017
*/

#include "../Include/Vector.h"

//This method calculates the dot product of two vectors
float DotProduct(Vector2& a, Vector2& b)
{
	return a.x * b.x + a.y * b.y;
}

//This method calculates the magnitude of the vector
float Magnitude(Vector2& a)
{
	return sqrt(DotProduct(a, a));
}

//This method normalizes a vector
Vector2 Normalize(Vector2& a)
{
	return a / Magnitude(a);
}