#pragma once

#include "WaypointManager.h"

using namespace std;

class Vehicle;
class PickupItem;
typedef vector<PickupItem*> vecPickups;

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
	void	Wander(Vehicle* car);
	void	Seek(Vehicle* seeker, Vehicle* target);
	void	Flee(Vehicle* flee, Vehicle* target);


private:
	vecPickups              m_pickups;
	Vehicle*				m_pRedCar = nullptr;
	Vehicle*				m_pBlueCar = nullptr;
	WaypointManager			m_waypointManager;
	bool					m_seek = false;
	bool					m_flee = false;
};

