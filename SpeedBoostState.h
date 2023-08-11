#pragma once
#include "FSMState.h"

class SpeedBoostState : FSMState
{
	SpeedBoostState(int type = FSM_STATE_SPEEDBOOST)
	{
		m_type = type;
	}

	void Enter();
	void Exit();
	void Update();
	void Init();
};

