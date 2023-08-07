#include "FSMMachine.h"
/*
void FSMMachine::UpdateMachine(int t)
{
	//dont do anything with no state
	if (m_states.size() == 0)
		return;

	//don't do anything with no current state/default state
	if (!m_currentState)
		m_currentState = m_defaultState;
	if (!m_currentState)
		return;

	//update current state
	int oldStateID = m_currentState->m_type;
	//check for a transition
	m_goalID = m_currentState->CheckTransitions();

	//switch states if there is a transition
	if (m_goalID != oldStateID)
	{
		if (TransitionState(m_goalID))
		{
			m_currentState->Exit();
			m_currentState = m_goalState;
			m_currentState->Enter();
		}
	}
	m_currentState->Update(t);
}
*/
