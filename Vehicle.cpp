#include "Vehicle.h"
#include <iostream>

#define VEHICLE_MASS 0.00005f
#define SEEK_MESSAGE "SEEK"
#define FLEE_MESSAGE "FLEE"
#define ARRIVE_MESSAGE "ARRIVE"
#define SEEK_RADIUS 10
#define MAX_SPEED 1

Vehicle::Vehicle() : m_forceMotion(VEHICLE_MASS, getPositionAddress())
{
	m_currentPosition = Vector2D(0,0);
	m_lastPosition = Vector2D(0, 0);
	m_waypointManager = nullptr;
}

HRESULT	Vehicle::initMesh(ID3D11Device* pd3dDevice, carColour colour)
{
	m_scale = XMFLOAT3(30, 20, 1);

	if (colour == carColour::redCar)
	{
		setTextureName(L"Resources\\car_red.dds");
	}
	else if (colour == carColour::blueCar)
	{
		setTextureName(L"Resources\\car_blue.dds");
	}

	HRESULT hr = DrawableGameObject::initMesh(pd3dDevice);

	setPosition(Vector2D(0, 0));

	m_lastPosition = Vector2D(0, 0);

	return hr;
}

void Vehicle::update(const float deltaTime)
{
	updateMessages(deltaTime);

	m_forceMotion.update(deltaTime);

	// rotate the object based on its last & current position
	Vector2D diff = m_currentPosition - m_lastPosition;
	whisker = m_currentPosition + diff;
	
	if (diff.Length() > 0) { // if zero then don't update rotation
		diff.Normalize();
		m_radianRotation = atan2f((float)diff.y, (float)diff.x); // this is used by DrawableGameObject to set the rotation
	}
	m_lastPosition = m_currentPosition;

	//bb = CollisionHelper::createBoundingBoxFromPoints()

	// set the current poistion for the drawablegameobject
	setPosition(m_currentPosition);

	DrawableGameObject::update(deltaTime);
}

// set the current position
void Vehicle::setPosition(Vector2D position)
{
	m_currentPosition = position;
	DrawableGameObject::setPosition(position);
}

void Vehicle::setWaypointManager(WaypointManager* wpm)
{
	m_waypointManager = wpm;
}

#pragma region OLD MOVEMENT CODE

void Vehicle::applyForceToPosition(const Vector2D& positionTo, string name)
{

	// create a vector from the position to, and the current car position
	Vector2D posFrom = getPosition();
	Vector2D force = positionTo - posFrom;

	// normalise this (make it length 1)
	force.Normalize();

	getForceMotion()->applyForce(force);

	// Tutorial todo
	// create a message called 'SEEK' which detects when the car has reached a certain point
	// note: this has been done for you in the updateMessages function.
	MessagePosition message;
	message.name = name;
	message.position = positionTo;
	addMessage(message);

	//addMessage(message);
}

void Vehicle::forceTemp(Vector2D positionTo, string name)
{

	// create a vector from the position to, and the current car position
	Vector2D posFrom = getPosition();
	force = positionTo - posFrom;

	// normalise this (make it length 1)
	force.Normalize();

	if (getForceMotion()->getVelocity().Length() < MAX_SPEED)
	{
	//getForceMotion()->applyForce(force);
	getForceMotion()->accummulateForce(force);
	//getForceMotion()->accummulateForce(brakingForce);
	}

	// Tutorial todo
	// create a message called 'SEEK' which detects when the car has reached a certain point
	// note: this has been done for you in the updateMessages function.
	MessagePosition message;
	message.name = name;
	message.position = positionTo;
	addMessage(message);

}

void Vehicle::arrive(Vector2D positionTo, string name)
{
	float deceleration = 0.5;
	Vector2D posFrom = getPosition();
	Vector2D desiredvelocity = positionTo - posFrom;
	desiredvelocity.Normalize();
	desiredvelocity*deceleration;

}


#pragma endregion

#pragma region NEW FORCE BASED MOVEMENT (REFERRAL)

void Vehicle::Seek(Vector2D targetPos, string name)
{

	Vector2D currentPos = getPosition();
	Vector2D unitVec = targetPos - currentPos;
	unitVec.Normalize();

	Vector2D desiredVelo = unitVec * MAX_SPEED;

	Vector2D currentVelo = getForceMotion()->getVelocity();
	currentVelo.Normalize();

	Vector2D seekForce = desiredVelo - currentVelo;

	m_forceMotion.accummulateForce(seekForce);

	MessagePosition message;
	message.name = name;
	message.position = targetPos;
	addMessage(message);
}

void Vehicle::Arrive(Vector2D targetPos, string name)
{
	float deceleration = 0.5;
	Vector2D currentPos = getPosition();
	Vector2D arrivePos = targetPos - currentPos;
	arrivePos.Normalize();

	Vector2D arriveSpeed = arrivePos * deceleration;

	Vector2D desiredVelo = arrivePos * arriveSpeed;

	Vector2D currentVelo = getForceMotion()->getVelocity();
	currentVelo.Normalize();

	Vector2D arriveForce = desiredVelo - currentVelo;

	m_forceMotion.accummulateForce(arriveForce);

	MessagePosition message;
	message.name = name;
	message.position = targetPos;
	addMessage(message);
}

void Vehicle::Wander()
{
	float fRandomDot = (static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 2.0)))-1;
	float radian = acos(fRandomDot);

	int direction = rand() % 2;
	if (direction == 1)
	{
		radian += 3.14159;
	}
	float xPos = cos(radian) - sin(radian);
	float yPos = sin(radian) + cos(radian);

	Vector2D wanderDirection = Vector2D(xPos, yPos);
	wanderDirection.Normalize();

	Vector2D wanderPosition = getPosition();
	wanderPosition += wanderDirection * MAX_SPEED;

	Seek(wanderPosition, SEEK_MESSAGE);
}

void Vehicle::Flee(Vector2D targetPos, string name)
{

	Vector2D currentPos = getPosition();
	Vector2D unitVec = currentPos - targetPos;
	unitVec.Normalize();

	Vector2D desiredVelo = unitVec * MAX_SPEED;

	Vector2D currentVelo = getForceMotion()->getVelocity();
	currentVelo.Normalize();

	Vector2D fleeForce = desiredVelo - currentVelo;

	getForceMotion()->accummulateForce(fleeForce);

	MessagePosition message;
	message.name = name;
	message.position = targetPos;
	addMessage(message);
}


#pragma endregion

#pragma region MESSAGING SYSTEM

// -------------------------------------------------------------------------------
// a really rubbish messaging system.. there is clearly a better way to do this...


void Vehicle::addMessage(MessagePosition message)
{
	m_vecMessages.push_back(message);
}

void Vehicle::updateMessages(const float deltaTime)
{
	// create an iterator to iterate the message list
	list<MessagePosition>::iterator messageIterator = m_vecMessages.begin();

	// loop while the iterator is not at the end
	while (messageIterator != m_vecMessages.end())
	{
		MessagePosition msg = *messageIterator;
		
		if (msg.name.compare(SEEK_MESSAGE) == 0)
		{
			Vector2D differenceVector = getPosition() - msg.position;

				// WARNING - when testing distances, make sure they are large enough to be detected. Ask a lecturer if you don't understand why. 10 *should* be about right
				if (differenceVector.Length() < SEEK_RADIUS)
				{
					messageReceived(msg);

					// delete the message. This will also assign(increment) the iterator to be the next item in the list
					messageIterator = m_vecMessages.erase(messageIterator);
					continue; // continue the next loop (we don't want to increment below as this will skip an item)
				}
			
		}
		if (msg.name.compare(FLEE_MESSAGE) == 0)
		{
			Vector2D differenceVector = getPosition() - msg.position;
			if (differenceVector.Length() > 200)
			{
				messageReceived(msg);

				messageIterator = m_vecMessages.erase(messageIterator);
				continue;
			}
		}
		if (msg.name.compare(ARRIVE_MESSAGE) == 0)
		{
			Vector2D differenceVector = getPosition() - msg.position;

			// WARNING - when testing distances, make sure they are large enough to be detected. Ask a lecturer if you don't understand why. 10 *should* be about right
			if (differenceVector.Length() < SEEK_RADIUS)
			{
				messageReceived(msg);

				// delete the message. This will also assign(increment) the iterator to be the next item in the list
				messageIterator = m_vecMessages.erase(messageIterator);
				continue; // continue the next loop (we don't want to increment below as this will skip an item)
			}

		}
		
		messageIterator++; // incremenet the iterator
	}

	
}

void Vehicle::messageReceived(MessagePosition message)
{
	if (message.name.compare(SEEK_MESSAGE) == 0)
	{
		m_forceMotion.clearForce();
	}
	if (message.name.compare(FLEE_MESSAGE) == 0)
		m_forceMotion.clearForce();
}

#pragma endregion
