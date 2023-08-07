#pragma once
#include "FSMState.h"
#include "Control.h"
#include <iostream>
#include <vector>

class FSMMachine : public FSMState
{
public:
	FSMMachine(int type = FSM_STATE_NONE)
	{m_type = type;}

	virtual void UpdateMachine(int t);
	virtual void AddState(FSMState* state);
	virtual void SetDefaultState(FSMState* state) { m_defaultState = state; }
	//virtual void SetGoalID(int goal) { m_goalID = goal; }
	//virtual TransitionState(int goal);
	//virtual Reset();

	int m_type;

private:
	std::vector<FSMState*> m_states;
	FSMState* m_currentState;
	FSMState* m_defaultState;
	FSMState* m_goalState;
	FSMState* m_goalID;
};

