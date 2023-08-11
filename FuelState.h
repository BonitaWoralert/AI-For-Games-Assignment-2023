#pragma once
#include "FSMState.h"
class FuelState : FSMState
{
	FuelState(int type = FSM_STATE_FUEL)
	{
		m_type = type;
	}

	void Enter();
	void Exit();
	void Update();
	void Init();
	
};

