#pragma once
#include <cmath>

class GlowStructure
{
public:
	void set(float rA, float gA, float bA, float aA) {
		r = rA;
		g = gA;
		b = bA;
		alpha = aA;
		render_when_occluded = 1;
		render_when_unoccluded = 0;
		bloom_amount = 255;
	}
	bool unused() {
		return next_free_slot != -2;
	}

	void* entity;
	float r;
	float g;
	float b;
	float alpha;
	char unknown0[8];
	float bloom_amount;
	char unknown1[4];
	bool render_when_occluded;
	bool render_when_unoccluded;
	bool full_bloom_render;
	char unknown2[13];
	int next_free_slot;
};