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
    : brain(FSM_STATE_PASSENGER)
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

    //create fuel
    PickupItem* pPickupFuel = new PickupItem();
    hr = pPickupFuel->initMesh(pd3dDevice, pickuptype::Fuel);
    m_pickups.push_back(pPickupFuel);

    //create speedboost
    PickupItem* pPickupBoost = new PickupItem();
    hr = pPickupBoost->initMesh(pd3dDevice, pickuptype::SpeedBoost);
    m_pickups.push_back(pPickupBoost);
  
    // create a passenger pickup item
    PickupItem* pPickupPassenger = new PickupItem();
    hr = pPickupPassenger->initMesh(pd3dDevice, pickuptype::Passenger);
    m_pickups.push_back(pPickupPassenger);

    setRandomPickupPosition(pPickupPassenger);
    setRandomPickupPosition(pPickupFuel);
    setRandomPickupPosition(pPickupBoost);

    //create FSM and set default state
    FSM brain(FSM_STATE_PASSENGER);

    return hr;
}

void AIManager::update(const float fDeltaTime)
{

    //update fsm
    if (brain.HasState() && FSMToggle == true) //if there is a state
    {
        //state manager will switch to it
        StateManager(brain.m_activeState);
    }

    //update steering behaviours
    SteeringBehaviourManager(activeBehaviour);
    
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
    
    if (m_flee)
    {
        //m_pRedCar->Flee(m_pBlueCar->getPosition(), FLEE_MESSAGE);
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
        //m_pBlueCar->Seek(m_pRedCar->getPosition(), SEEK_MESSAGE);
        AddItemToDrawList(m_pBlueCar);
    }
}

#pragma region	FSM STATES

void AIManager::StateManager(int activeState)
{
    switch (activeState)
    {
    case 1:
        FuelState();
        break;
    case 2:
        PassengerState();
        break;
    case 3:
        SpeedBoostState();
        break;
    default:
        break;
    }
}

void AIManager::FuelState()
{
    OutputDebugStringA("entered fuel state");
    if (m_pRedCar->GetFuel() < 0.25)
    {
        m_pRedCar->Seek(m_pickups[0]->getPosition(), SEEK_MESSAGE); //seek fuel
    }
    //if boost near
    else if ((m_pRedCar->getPosition() - m_pickups[1]->getPosition()).Length() < 250)
    {
        brain.SetState(FSM_STATE_SPEEDBOOST); //boost state
    }
    else
    {
        brain.SetState(FSM_STATE_PASSENGER); //pickup passengers
    }
}

void AIManager::PassengerState()
{
    OutputDebugStringA("entered passenger state");
    //if low fuel
    if (m_pRedCar->GetFuel() < 0.25)
    {
        brain.SetState(FSM_STATE_FUEL); //fuel state
    }
    //if boost near
    else if ((m_pRedCar->getPosition() - m_pickups[1]->getPosition()).Length() < 250)
    {
        brain.SetState(FSM_STATE_SPEEDBOOST); //boost state
    }

    //seek passenger
    m_pRedCar->Seek(m_pickups[2]->getPosition(), SEEK_MESSAGE);
}

void AIManager::SpeedBoostState()
{
    OutputDebugStringA("entered speedboost state");

    //seek speedboost
    if ((m_pRedCar->getPosition() - m_pickups[1]->getPosition()).Length() < 250)
    {
        m_pRedCar->Seek(m_pickups[1]->getPosition(), SEEK_MESSAGE);
    }
    //if low fuel
    else if (m_pRedCar->GetFuel() < 0.25)
    {
        brain.SetState(FSM_STATE_FUEL); //fuel state
    }
    else
    {
        //time for passengers
        brain.SetState(FSM_STATE_PASSENGER);
    }
}

#pragma endregion

void AIManager::mouseUp(int x, int y)
{
    m_pRedCar->Seek(Vector2D(x, y), SEEK_MESSAGE);
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
    const WPARAM key_1 = 49;

    const WPARAM key_a = 65;
    const WPARAM key_s = 83;
    const WPARAM key_w = 87;
    const WPARAM key_p = 80;
    const WPARAM key_f = 70;
    const WPARAM key_space = 32;

    switch (param)
    {
    //toggle between Steering / FSM AI
    case key_1:
    {
        FSMToggle = !FSMToggle;
        break;
    }
    //debug stuff
    case VK_NUMPAD0:
    {
        //seek fuel
        m_pRedCar->Seek(m_pickups[0]->getPosition(), SEEK_MESSAGE);
        break;
    }
    case VK_NUMPAD1:
    {
        //seek speedboost
        m_pRedCar->Seek(m_pickups[1]->getPosition(), SEEK_MESSAGE);
        break;
        
    }
    case VK_NUMPAD2:
    {
        //seek passenger
        m_pRedCar->Seek(m_pickups[2]->getPosition(), SEEK_MESSAGE);
        break;
    }
    case VK_NUMPAD3:
    {
        //manual fuel refill
        m_pBlueCar->FuelRefill();
        m_pRedCar->FuelRefill();
        break;
    }
    //steering behaviours
    case key_a:
    {
        ArriveAtRandomWaypoint(m_pBlueCar);
        break;
    }
    case key_s:
    {
        SeekRandomWaypoint(m_pBlueCar);
        break;
    }
    case key_w:
    {
        activeBehaviour = STEERING_WANDER;
        break;
    }
    case key_p:
    {
        activeBehaviour = STEERING_PURSUIT;
        break;
    }
    case key_f:
    {
        activeBehaviour = STEERING_FLEE;
        break;
    }
    case key_space:
    {
        activeBehaviour = STEERING_NONE;
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

void AIManager::SteeringBehaviourManager(int activeBehaviour)
{
    switch (activeBehaviour)
    {
        case 1:
            Wander(m_pBlueCar);
            break;
        case 2:
            Pursuit(m_pBlueCar, m_pRedCar);
            break;
        case 3:
            Flee(m_pBlueCar, m_pRedCar);
            break;
        default:
            break;
    }
}

void AIManager::Wander(Vehicle* car)
{
    int x = (rand() % SCREEN_WIDTH) - (SCREEN_WIDTH / 2);
    int y = (rand() % SCREEN_HEIGHT) - (SCREEN_HEIGHT / 2);

    Waypoint* wp = m_waypointManager.getNearestWaypoint(Vector2D(x, y));
    if (wp) 
    {
        car->Seek(wp->getPosition() , SEEK_MESSAGE);
    }
}

void AIManager::Flee(Vehicle* flee, Vehicle* target)
{
    if (flee->getPosition().Distance(target->getPosition()) < 350.0f) // check distance between fleeing car and target
    {
        flee->Flee(target->getPosition(), FLEE_MESSAGE);
    }   
}

void AIManager::ArriveAtRandomWaypoint(Vehicle* car)
{
    //Arrive at random waypoint
    Waypoint* randWp = m_waypointManager.getWaypoint(std::rand() % m_waypointManager.getWaypointCount());
    car->Arrive(randWp->getPosition(), SEEK_MESSAGE);
}

void AIManager::SeekRandomWaypoint(Vehicle* car)
{
    //Go to random waypoint
    Waypoint* randWp = m_waypointManager.getWaypoint(std::rand() % m_waypointManager.getWaypointCount());
    m_pBlueCar->Seek(randWp->getPosition(), SEEK_MESSAGE);
}

void AIManager::Pursuit(Vehicle* pursuit, Vehicle* target)
{
    //pursuit
    m_pBlueCar->Seek(m_pRedCar->getPosition(), SEEK_MESSAGE);
}

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
    vector<BoundingSphere> PU;
    BoundingSphere boundingSpherePU_1;
    BoundingSphere boundingSpherePU_2;
    BoundingSphere boundingSpherePU_3;
    PU.push_back(boundingSpherePU_1);
    PU.push_back(boundingSpherePU_2);
    PU.push_back(boundingSpherePU_3);
    
    for (int i = 0; i < 3; i++)
    {
        XMMatrixDecompose(
            &puScale,
            &dummy,
            &puPos,
            XMLoadFloat4x4(m_pickups[i]->getTransform())
        );

        // bounding sphere for pickup item
        XMStoreFloat3(&scale, puScale);
        XMStoreFloat3(&PU[i].Center, puPos);
        PU[i].Radius = scale.x;
        
        // does the car bounding sphere collide with the pickup bounding sphere?
    
        if (boundingSphereCar.Intersects(PU[i]))
        {
            pickuptype type = m_pickups[i]->getType();
            m_pickups[i]->hasCollided();
            setRandomPickupPosition(m_pickups[i]);

            switch (type)
            {
            case pickuptype::Fuel:
                m_pRedCar->FuelRefill();
                OutputDebugStringA("fuel\n");
                break;
            case pickuptype::SpeedBoost:
                m_pRedCar->SpeedBoost();
                OutputDebugStringA("speed boost\n");
                break;
            case pickuptype::Passenger:
                OutputDebugStringA("passenger pickup\n");
                break;
            default:
                break;
            }

            return true;
        }
    }


    return false;
}





