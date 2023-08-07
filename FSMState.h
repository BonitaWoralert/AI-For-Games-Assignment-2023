#pragma once
#include "Control.h"

class FSMState
{
public:
	FSMState(int type = FSM_STATE_NONE, Control* parent = nullptr)
	{
		m_type = type; m_parent = parent;
	}

	virtual void Enter() {}
	virtual void Exit() {}
	virtual void Update(int t) {}
	virtual void Init() {}
	virtual void CheckTransitions(int t) {}

	Control* m_parent;
	int m_type;
};

