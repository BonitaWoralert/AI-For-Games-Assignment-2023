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

    FSM(int type)
	    {
            m_activeState = type;
	    }

    void SetState(int newState) { m_activeState = newState; }

    bool HasState() { 
        if (m_activeState != 0) 
            return true; 
        return false; }

    int GetState() { return m_activeState; }

    int m_activeState;
private:
};
