#pragma once
#include "FSM.h"

class FSMState
{
public:
	FSMState(int type = FSM_STATE_NONE)
	{
		m_type = type;
	}

	virtual void Enter() {} //entering state
	virtual void Exit() {} //exiting state
	virtual void Update(float deltaTime) {} //update each loop
	virtual void Init() {} //reset state

	int m_type;
};

