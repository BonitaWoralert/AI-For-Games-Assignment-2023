#include "FSMMachine.h"

void FSMMachine::UpdateMachine(float deltaTime)
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

	//switch states if there is a transition

	m_currentState->Exit();
	m_currentState = m_nextState;
	m_currentState->Enter();

	m_currentState->Update(deltaTime);
}


