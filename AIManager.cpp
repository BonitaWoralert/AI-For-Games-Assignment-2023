#include "AIManager.h"
#include "Vehicle.h"
#include "DrawableGameObject.h"
#include "PickupItem.h"
#include "Waypoint.h"
#include "main.h"
#include "constants.h"
#include "ForceHelper.h"



// AI Manager

AIManager::AIManager()
{
    m_pRedCar = nullptr;
    std::srand(std::time(nullptr)); //use current time as seed for rand
}

AIManager::~AIManager()
{
	release();
}

void AIManager::release()
{
	clearDrawList();

	for (PickupItem* pu : m_pickups)
	{
		delete pu;
	}
	m_pickups.clear();

    //delete both cars
	delete m_pRedCar;
    m_pRedCar = nullptr;

    delete m_pBlueCar;
    m_pBlueCar = nullptr;
}

HRESULT AIManager::initialise(ID3D11Device* pd3dDevice)
{
    // create red vehicle 
    float xPos = 0;
    float yPos = 0;

    m_pRedCar = new Vehicle();
    HRESULT hr = m_pRedCar->initMesh(pd3dDevice, carColour::redCar);
    m_pRedCar->setPosition(Vector2D(xPos, yPos));
    if (FAILED(hr))
        return hr;

    //create blue vehicle
    m_pBlueCar = new Vehicle();
    hr = m_pBlueCar->initMesh(pd3dDevice, carColour::blueCar);
    m_pBlueCar->setPosition(Vector2D(xPos, yPos));
    if (FAILED(hr))
        return hr;
    
    // setup the waypoints
    m_waypointManager.createWaypoints(pd3dDevice);
    m_pRedCar->setWaypointManager(&m_waypointManager);

    // create a passenger pickup item
    PickupItem* pPickupPassenger = new PickupItem();
    hr = pPickupPassenger->initMesh(pd3dDevice, pickuptype::Passenger);
    m_pickups.push_back(pPickupPassenger);

    setRandomPickupPosition(pPickupPassenger);

    return hr;
}


void AIManager::update(const float fDeltaTime)
{
    for (unsigned int i = 0; i < m_waypointManager.getWaypointCount(); i++) {
        m_waypointManager.getWaypoint(i)->update(fDeltaTime);
        //AddItemToDrawList(m_waypointManager.getWaypoint(i)); // if you uncomment this, it will display the waypoints
    }

    for (int i = 0; i < m_waypointManager.getQuadpointCount(); i++)
    {
        Waypoint* qp = m_waypointManager.getQuadpoint(i);
        qp->update(fDeltaTime);
        //AddItemToDrawList(qp); // if you uncomment this, it will display the quad waypoints
    }

    // update and display the pickups
    for (unsigned int i = 0; i < m_pickups.size(); i++) {
        m_pickups[i]->update(fDeltaTime);
        AddItemToDrawList(m_pickups[i]);
    }

	// draw the waypoints nearest to the blue car
    
    Waypoint* wp = m_waypointManager.getNearestWaypoint(m_pBlueCar->getPosition());
	if (wp != nullptr)
	{
		vecWaypoints vwps = m_waypointManager.getNeighbouringWaypoints(wp);
		for (Waypoint* wp : vwps)
		{
			AddItemToDrawList(wp);
		}
	}
    
    if (m_seek)
    {
        Seek(m_pRedCar, m_pBlueCar);
    }
    
    if (m_flee)
    {
        Flee(m_pRedCar, m_pBlueCar);
    }

    // update and draw the red car (and check for pickup collisions)
	if (m_pRedCar != nullptr)
	{
        m_pRedCar->update(fDeltaTime);
		checkForCollisions();
		AddItemToDrawList(m_pRedCar);
	}

    //update and draw blue car
    if (m_pBlueCar != nullptr)
    {
        m_pBlueCar->update(fDeltaTime);
        Wander(m_pBlueCar);
        AddItemToDrawList(m_pBlueCar);
    }
}

void AIManager::mouseUp(int x, int y)
{
    //m_pRedCar->forceTemp(Vector2D(x, y), SEEK_MESSAGE);
    m_pRedCar->Seek(Vector2D(x, y), SEEK_MESSAGE);
    //m_pRedCar->Flee(Vector2D(x, y), FLEE_MESSAGE);
}

void AIManager::keyUp(WPARAM param)
{
    const WPARAM key_a = 65;
    switch (param)
    {
    case key_a:
    {
        OutputDebugStringA("a Up \n");
        break;
    }
    }
}

void AIManager::keyDown(WPARAM param)
{
    // hint 65-90 are a-z
    const WPARAM key_a = 65;
    const WPARAM key_s = 83;
    const WPARAM key_t = 84;
    const WPARAM key_space = 32;

    switch (param)
    {
    case key_space:
    {
        break;
    }
    case VK_NUMPAD0:
    {
        OutputDebugStringA("seeking");
        m_seek = !m_seek;
        break;
    }
    case VK_NUMPAD1:
    {
        //m_pRedCar->Flee(m_pBlueCar->getPosition(), FLEE_MESSAGE);
        
        OutputDebugStringA("fleeing");
        m_flee = !m_flee;
        break;
        
    }
    case VK_NUMPAD2:
    {
        OutputDebugStringA("2 pressed \n");
        break;
    }
    case key_a:
    {
        //go to random waypoint
        
        Waypoint* randWp = m_waypointManager.getWaypoint(std::rand() % m_waypointManager.getWaypointCount());
        m_pRedCar->forceTemp(randWp->getPosition(), SEEK_MESSAGE);

        OutputDebugStringA("a Down \n");
        break;
    }
    case key_s:
    {
        break;
    }
    case key_t:
    {
        break;
    }
    // etc
    default:
        break;
    }
}

void AIManager::setRandomPickupPosition(PickupItem* pickup)
{
    if (pickup == nullptr)
        return;

    int x = (rand() % SCREEN_WIDTH) - (SCREEN_WIDTH / 2);
    int y = (rand() % SCREEN_HEIGHT) - (SCREEN_HEIGHT / 2);

    Waypoint* wp = m_waypointManager.getNearestWaypoint(Vector2D(x, y));
    if (wp) {
        pickup->setPosition(wp->getPosition());
    }
}

void AIManager::Wander(Vehicle* car)
{
    int x = (rand() % SCREEN_WIDTH) - (SCREEN_WIDTH / 2);
    int y = (rand() % SCREEN_HEIGHT) - (SCREEN_HEIGHT / 2);

    Waypoint* wp = m_waypointManager.getNearestWaypoint(Vector2D(x, y));
    if (wp) {
        car->forceTemp(wp->getPosition(), SEEK_MESSAGE);
    }
}

void AIManager::Seek(Vehicle* seeker, Vehicle* target)
{
    seeker->forceTemp(target->getPosition(), SEEK_MESSAGE); //go towards target
}

void AIManager::Flee(Vehicle* flee, Vehicle* target)
{
    /*
    if (flee->getPosition().Distance(target->getPosition()) < 350.0f) // check distance between fleeing car and target
    {
        Vector2D location = target->getPosition().GetReverse(); //reverse the position of the target
        flee->forceTemp(location, SEEK_MESSAGE);
    }
    */
    flee->Flee(target->getPosition(), FLEE_MESSAGE);
}

/*
// hello. This is hopefully the only time you may need to use and alter directx code 
// the relevant #includes are already in place, but if you create your own collision class (or use this code anywhere else) 
// make sure you have the following:

#include <d3d11_1.h> // this has the appropriate directx structures / objects
#include <DirectXCollision.h> // this is the dx collision class helper
using namespace DirectX; // this means you don't need to put DirectX:: in front of objects like XMVECTOR and so on. 
*/

bool AIManager::checkForCollisions()
{
    if (m_pickups.size() == 0)
        return false;

    XMVECTOR dummy;

    // get the position and scale of the car and store in dx friendly xmvectors
    XMVECTOR carPos;
    XMVECTOR carScale;
    XMMatrixDecompose(
        &carScale,
        &dummy,
        &carPos,
        XMLoadFloat4x4(m_pRedCar->getTransform())
    );

    // create a bounding sphere for the car
    XMFLOAT3 scale;
    XMStoreFloat3(&scale, carScale);
    BoundingSphere boundingSphereCar;
    XMStoreFloat3(&boundingSphereCar.Center, carPos);
    boundingSphereCar.Radius = scale.x;

    // do the same for a pickup item
    // a pickup - !! NOTE it is only referring the first one in the list !!
    // to get the passenger, fuel or speedboost specifically you will need to iterate the pickups and test their type (getType()) - see the pickup class
    XMVECTOR puPos;
    XMVECTOR puScale;
    XMMatrixDecompose(
        &puScale,
        &dummy,
        &puPos,
        XMLoadFloat4x4(m_pickups[0]->getTransform())
    );

    // bounding sphere for pickup item
    XMStoreFloat3(&scale, puScale);
    BoundingSphere boundingSpherePU;
    XMStoreFloat3(&boundingSpherePU.Center, puPos);
    boundingSpherePU.Radius = scale.x;

    // does the car bounding sphere collide with the pickup bounding sphere?
    if (boundingSphereCar.Intersects(boundingSpherePU))
    {
        OutputDebugStringA("Pickup passenger collision\n");
        m_pickups[0]->hasCollided();
        setRandomPickupPosition(m_pickups[0]);

        // you will need to test the type of the pickup to decide on the behaviour
        // m_pRedCar->dosomething(); ...

        return true;
    }

    return false;
}





