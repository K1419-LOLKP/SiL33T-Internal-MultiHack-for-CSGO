#pragma once
#include "GlowStructure.h"
#include "Physics.h"
#include "SDK.h"

class Entites
{
public:
	int health = 0;
	int team = 0;
	int shotsFired;
	int compWins;
	int xID = 0;
	bool dormant = true;
	bool visible = false;
	bool scoped = true;
	bool special;
	Vector velocityVector;
	float netVel;
	float netDeg;
	Angle viewAngle;
	Angle aimAngle;
	vec3 headPos;
	intptr_t glowIndex = NULL;
	intptr_t boneIndex = NULL;
	intptr_t addressInMem = NULL;
	GlowStructure entityGs;
	Ray_t ray;
};

