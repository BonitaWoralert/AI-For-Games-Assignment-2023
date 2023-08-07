#pragma once
#include "Control.h"

class FSMState
{
public:
	FSMState(int type = FSM_STATE_NONE, Control* parent = nullptr)
	{
		m_type = type; m_parent = parent;
	}

	virtual void Enter() {} //entering state
	virtual void Exit() {} //exiting state
	virtual void Update(int t) {} //update each loop
	virtual void Init() {} //reset state
	virtual void CheckTransitions(int t) {} //how will the state end/what state will it go to when it finishes

	Control* m_parent;
	int m_type;
};

