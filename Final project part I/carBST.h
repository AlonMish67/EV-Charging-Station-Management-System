#ifndef CARBST_H
#define CARBST_H

#include <stdbool.h>
#include "Car.h"

typedef struct tCar tCar;
typedef struct stationBST_M stationBST_M;


// Binary tree node for Car
typedef struct tCar
{
    Car* p2car;            // Pointer to Car object
    tCar* right;           // Right child
    tCar* left;            // Left child
} tCar;

// Car tree manager
typedef struct 
{
    tCar* root;           // Root of the tree
    int count;            // Number of cars in the tree
} car_BST;


//Initializes an empty car BST and returns a car_BST manager with no cars.
car_BST makeEmptyCarBST();

/**
* Inserts a new car into the car BST
* @param manager: Pointer to the car BST manager.
* @param newCar: Pointer to the Car object to be inserted.
* @return True if the car is successfully inserted, False if insertion fails
*/
bool insertCar(car_BST* manager, Car* newCar);

/**
* Creates a new Car object with given license plate and port type.
* @param licensePlate: string representing the car`s license plate (user input).
* @param portType: The car`s port type (user input).
* @return A pointer to the newly created Car object.
*/
Car* createCar(const char* licensePlate, PortType portType); //TO BE IMPLEMENTED for action 2 - chargeCar

/**
* Deletes a car from the car BST based on its license plate.
* @param carManager: Pointer to the car BST manager.
* @param licensePlate: license plate string of the car to be deleted (user input).
* @return True if the car is successfully deleted, False if deletion failed.
*/
bool deleteCar(car_BST* carManager, const char* licensePlate);

/**
* Searches for a car in the car BST by its license plate.
* @param manager: Pointer to the car BST manager.
* @param licensePlate: License plate string of the car to search for (user input)
* @return A pointer to the Car object if found, or NULL if the car is not found.
*/
Car* searchCar(const car_BST* manager, const char* licensePlate);
;

/**
* Destroys the car BST by recursively freeing all allocated memory.
* @param carManager: Pointer to the car BST manager.
*/
void destroyCarTree(car_BST* carManager);

/**
* Update the car data file with the current state of the car BST.
* @param carManager: Pointer to the car BST manager.
* @param stationManager: Pointer to the station BST manager.
* @param filename: The name of the file to be updated with the latest car information.
*/
void updateCarFiles(car_BST* carManager,stationBST_M* stationManager, const char* filename);

#endif


