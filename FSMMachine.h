#pragma once
#include "FSMState.h"
#include <iostream>
#include <vector>

class FSMMachine : public FSMState
{
public:
	FSMMachine(int type = FSM_STATE_NONE)
		{m_type = type;}

	virtual void UpdateMachine(float deltaTime);
	virtual void AddState(FSMState* state);
	virtual void SetDefaultState(FSMState* state) { m_defaultState = state; }

	int m_type;

private:
	std::vector<FSMState*> m_states;
	FSMState* m_currentState;
	FSMState* m_defaultState;
	FSMState* m_nextState;
};

