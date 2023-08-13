#pragma once

enum type
{
    FSM_STATE_NONE,
    FSM_STATE_FUEL,
    FSM_STATE_PASSENGER,
    FSM_STATE_SPEEDBOOST,
};

class FSM
{
public:

    FSM(int type = FSM_STATE_NONE)
	    {
            m_activeState = type;
	    }

    void SetState(type newState) { m_activeState = newState; }

    bool HasState() { 
        if (m_activeState != 0) 
            return true; 
        return false; }

    int GetState() { return m_activeState; }

private:
    int m_activeState;
};
