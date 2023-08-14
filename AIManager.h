#pragma once

#include "WaypointManager.h"

#include "FSM.h"

using namespace std;

class Vehicle;
class PickupItem;
typedef vector<PickupItem*> vecPickups;

enum activeSteeringBehaviour
{
	STEERING_NONE,
	STEERING_WANDER,
	STEERING_PURSUIT,
	STEERING_FLEE,
};

class AIManager
{
public:
	AIManager();
	virtual  ~AIManager();
	void	release();
	HRESULT initialise(ID3D11Device* pd3dDevice);
	void	update(const float fDeltaTime);
	void	mouseUp(int x, int y);
	void	keyDown(WPARAM param);
	void	keyUp(WPARAM param);

protected:
	bool	checkForCollisions();
	void	setRandomPickupPosition(PickupItem* pickup);

	//FSM
	void StateManager(int activeState);

	void PassengerState();
	void FuelState();
	void SpeedBoostState();

	//steering behaviours
	void SteeringBehaviourManager(int activeBehaviour);

	void	ArriveAtRandomWaypoint(Vehicle* car);
	void	SeekRandomWaypoint(Vehicle* car);
	void	Pursuit(Vehicle* pursuit, Vehicle* target);
	void	Wander(Vehicle* car);
	void	Flee(Vehicle* flee, Vehicle* target);

private:
	vecPickups              m_pickups;
	Vehicle*				m_pRedCar = nullptr;
	Vehicle*				m_pBlueCar = nullptr;
	WaypointManager			m_waypointManager;

	FSM						brain;

	bool					m_seek = false;
	bool					m_flee = false;
	bool					FSMToggle = true;
	int						activeBehaviour = 0;
};

