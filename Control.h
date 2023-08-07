#pragma once
#include "FSM.h"
#include "Vehicle.h"
class Control
{
public:
	Control(Vehicle* car = nullptr);
	void Update(int t);
	void Init();
	void UpdatePickups();
};

