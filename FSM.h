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

    void Update()
    {
        if (m_activeState != 0)
        {
            //do stuff
        }
    }

private:
    int m_activeState;
};
