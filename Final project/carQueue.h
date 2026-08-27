
#ifndef CARQUEUE_H
#define CARQUEUE_H

#include "car.h"
#include <stdbool.h>

typedef struct stationBST_M stationBST_M;

typedef struct carNode
{
	Car* p2car;
	struct carNode* next;
}carNode;

typedef struct
{
	carNode* front;
	carNode* rear;
}qCar;

//Initializes an empty car queue and returns an empty queue.
void initCarQueue(qCar* queue);

/**
* Enqueues a Car object to an existing station`s queue.
* @param station: Pointer to a station 
* @param car: Pointer to a car to insert
* @return True if car is successfully enqueued, False if insertion fails.
*/
bool enqueueCar(Station* station, Car* car);

/**
* Prints all cars in a queue
* @param queue: Pointer to a station`s queue
*/
void printQueuedCars(qCar* queue);

/**
* Assigns a Car object from station`s queue if there is a port of type available.
* @param station: Pointer to a station
* @param port: Pointer to a port the car will be assigned to
* @return True if car is successfully assigned to the port, False if no ports of car`s type available
*/
bool assignNextCompatibleCarFromQueue(Station* station, Port* port);

/**
* Update the line of car data files with the current state of the queue
* @param stationManager: Pointer to the station BST manager.
* @param filename: The name of the file to be updated with the latest car queue information.
*/
void updateCarQueueFile(stationBST_M* stationManager, const char* filename);

/**
* Frees the memory allocated for the queue.
* @param queue: Pointer to a station`s queue.
*/
void freeCarQueue(qCar* queue); 

#endif
