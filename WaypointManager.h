#pragma once

struct ID3D11Device;


#include <d3d11_1.h> // this has the appropriate directx structures / objects
#include <DirectXCollision.h> // this is the dx collision class helper
using namespace DirectX; // this means you don't need to put DirectX:: in front of objects like XMVECTOR and so on. 


#include <vector>
using namespace std;

class Waypoint;

#include "Vector2D.h"
#define WAYPOINT_RESOLUTION 20

typedef vector<Waypoint*> vecWaypoints;
typedef vector <BoundingBox> vecBoundingBox;

class WaypointManager
{
public:
	WaypointManager() { m_numCheckpoints = 0; };
	~WaypointManager();

	HRESULT createWaypoints(ID3D11Device* pd3dDevice);
	void destroyWaypoints();

	Waypoint* getWaypoint(const unsigned int index);
	size_t getWaypointCount() { return m_waypoints.size(); }

	// get the points which make a building on the map
	Waypoint* getQuadpoint(const unsigned int index);
	size_t getQuadpointCount() { return m_quadpoints.size(); }

	vecWaypoints getNeighbouringWaypoints(Waypoint* waypoint);
	Waypoint* getNearestWaypoint(Vector2D position);

protected: // methods
	// given two waypoints, if a line were to be drawn between them, would that cross a building?
	bool	doWaypointsCrossBuilding(Waypoint* wp1, Waypoint* wp2);


protected: // properties
	vecWaypoints	m_waypoints;
	vecWaypoints	m_quadpoints;
	int				m_numCheckpoints;
	vecBoundingBox	m_boundingBoxes;

};

