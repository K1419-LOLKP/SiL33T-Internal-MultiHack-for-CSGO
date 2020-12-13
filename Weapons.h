#pragma once
#include <string>

class Weapons
{
public:
	std::string custName = "";
	bool doTrigger = false;
	bool doAim = false;
	bool doRCS = false;
	bool fireWhenMoving = true;
	bool fireWhenUnscoped = true;
	bool hasScope = false;
	bool isKnife = false;
	int viewModel = 0;
	int seed = 0;
	bool holdScope = false;
	int skinNumber = 0;
	float skinCondition = 0.001f;
	int statTrak = -1;
};

