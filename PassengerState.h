#pragma once
#include "FSMState.h"

class PassengerState : FSMState
{
	PassengerState(int type = FSM_STATE_PASSENGER)
	{
		m_type = type;
	}

	void Enter();
	void Exit();
	void Update();
	void Init();
};

