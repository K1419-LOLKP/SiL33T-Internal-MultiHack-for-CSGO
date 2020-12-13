#pragma once
#include <cstdint>

struct Angle
{
	float Pitch;
	float Yaw;
	float Roll;
};

struct Point
{
	float x;
	float y;
	float z;
};

struct Vector
{
	float x;
	float y;
	float z;
};

struct UserCmd_t
{
	virtual ~UserCmd_t() {};
	int command_number;
	int tick_count;
	Angle viewangles;
	Vector aimdirection;
	float forwardmove;
	float sidemove;
	float upmove;
	int buttons;
	unsigned char impulse;
	int weaponselect;
	int weaponsubtype;
	int random_seed;
	short mousedx;
	short mousedy;
	bool hasbeenpredicted;
	Vector headangles;
	Vector headoffset;
};



/*
Angle subtract(Angle a1, Angle a2)
{
	Angle trA;

	trA.Pitch = a1.Pitch - a2.Pitch;
	trA.Yaw = a1.Yaw - a2.Yaw;
	trA.Roll = a1.Roll - a2.Roll;

	return(trA);
}

Angle add(Angle a1, Angle a2)
{
	Angle trA;

	trA.Pitch = a1.Pitch + a2.Pitch;
	trA.Yaw = a1.Yaw + a2.Yaw;
	trA.Roll = a1.Roll + a2.Roll;

	return(trA);
}

Angle multiply(Angle a1, float x)
{
	Angle trA;

	trA.Pitch = a1.Pitch * x;
	trA.Yaw = a1.Yaw * x;
	trA.Roll = a1.Roll * x;

	return(trA);
}

Angle divide(Angle a1, float x)
{
	Angle trA;

	trA.Pitch = a1.Pitch / x;
	trA.Yaw = a1.Yaw / x;
	trA.Roll = a1.Roll / x;

	return(trA);
}

Point subtract(Point a1, Point a2)
{
	Point trA;

	trA.x = a1.x - a2.x;
	trA.y = a1.y - a2.y;
	trA.z = a1.z - a2.z;

	return(trA);
}

Point add(Point a1, Point a2)
{
	Point trA;

	trA.x = a1.x + a2.x;
	trA.y = a1.y + a2.y;
	trA.z = a1.z + a2.z;

	return(trA);
}

Velocity subtract(Velocity a1, Velocity a2)
{
	Velocity trA;

	trA.x = a1.x - a2.x;
	trA.y = a1.y - a2.y;
	trA.z = a1.z - a2.z;

	return(trA);
}

Velocity add(Velocity a1, Velocity a2)
{
	Velocity trA;

	trA.x = a1.x + a2.x;
	trA.y = a1.y + a2.y;
	trA.z = a1.z + a2.z;

	return(trA);
}

Velocity multiply(Velocity a1, float x)
{
	Velocity trA;

	trA.x = a1.x * x;
	trA.y = a1.y * x;
	trA.z = a1.z * x;

	return(trA);
}

Velocity divide(Velocity a1, float x)
{
	Velocity trA;

	trA.x = a1.x / x;
	trA.y = a1.y / x;
	trA.z = a1.z / x;

	return(trA);
}
*/