#ifndef FILELOADER_H
#define FILELOADER_H

#include <stdio.h>
#include "car.h"
#include "stations.h"
#include "carQueue.h"
#include "carBST.h"


/**
* Loads station data from a file and inserts it into the station BST.
* @param filename: The name of the file to load station data from.
* @param manager: Pointer of the station BST manager 
*/
void loadStationsToTree(const char* filename, stationBST_M* manager);

/**
* Loads ports into the station`s ports list from a file.
* @param filename: The name of the file to load port data from.
* @param stationManager: Pointer of the station BST manager
* @param carManager: Pointer of the car BST manager
*/
void loadPortsToStations(const char* filename, stationBST_M* stationManager, car_BST* carManager);

/**
* Loads car data from a file  and inserts it into the car BST
* @param filename: The name of the file to load port data from.
* @param carManager: Pointer of the car BST manager
*/
void loadCarsToTree(const char* filename, car_BST* carManager);

/**
* Loads queued car data from a file and assigns cars to stations.
* @param filename: The name of the file to load port data from.
* @param stationManager: Pointer of the station BST manager
* @param carManager: Pointer of the car BST manager
*/
void assignQueuedCarsToStations(const char* filename, stationBST_M* stationManager, car_BST* carManager);

/**
* Loads all data into the managers, queues and ports lists.
* @param stationManager: Pointer of the station BST manager
* @param carManager: Pointer of the car BST manager
*/
void loadFiles(stationBST_M* stationManager, car_BST* carManager);


#endif