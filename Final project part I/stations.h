#ifndef STATIONS_H
#define STATIONS_H

#include "ports.h"
#include "carQueue.h"
#include "carBST.h"
#include <stdbool.h>
#define STATION_NAME_SIZE 256

typedef struct Coordinates 
{
    double x;
    double y;
} Coordinates;

typedef struct Station 
{
    int id;
    char* name; //name of a station
    int nPorts;
    Coordinates coord;
    Port* portsList;
    int nCars; //number of cars in queue
    qCar* carQueue;
    struct Station* left;
    struct Station* right;
} Station;

typedef struct stationBST_M 
{
    Station* root;
    int size; //can be used to get size (if needed...)
} stationBST_M;

typedef enum //enum for search by type func!
//0 = ID , 1 = NAME, 2 = COORDS
{
    SEARCH_BY_ID,
    SEARCH_BY_NAME,
    SEARCH_BY_COORDS
} SearchType;

// Initializes an empty station BST and returns a stationBST_M manager with no stations.
stationBST_M makeEmptyStationTree();

/**
* Inserts a new station into the station BST.
* @param manager: Pointer to the station BST manager.
* @param newStation: Pointer to the Station object to be inserted.
* @return True if the station is successfully inserted, False if insertion failed.
*/
bool insertStation(stationBST_M* manager, Station* newStation);

/**
* Searches for station by its ID.
* @param manager: Pointer to the station BST manager.
* @param stationID: ID of the station to search for.
* @return Pointer to the station if found, NULL if none found.
*/
Station* findStationByID(const stationBST_M* manager, int stationID); 

/**
* Searches for station by its name.
* @param manager: Pointer to the station BST manager.
* @param stationName: The name of the station.
* @return Pointer to the station if found, NULL if none found.
*/
Station* findStationByName(const stationBST_M* manager, const char* stationName);

/**
* Searches for station according to coordinates.
* @param manager: Pointer to the station BST manager.
* @param userX: X coordinate of type double.
* @param userY: Y coordinate of type double.
* @return Pointer to station if found, NULL if none found.
*/
Station* findNearestStation(stationBST_M* manager, double userX, double userY);

/**
* Searches for station according to searchType.
* @param stationManager: Pointer to the station BST manager.
* @param searchType: Value of enum SearchType indicating search method (ID, name, coordinates).
* @param data: Pointer of type void with user input data.
* @return Pointer to station if found, NULL if none found.
*/
Station* searchStation(stationBST_M* stationManager, int searchType, void* data);

/**
* Asks user to select a search method (ID / name) 
* @param stationManager: Pointer to the station BST manager.
* @return Pointer to station if found, NULL if none found.
*/
Station* getStationByUserInput(stationBST_M* stationManager);

/**
* Prints station information.
* @param station: Pointer of type Station.
*/
void printStation(Station* station);

/**
* Calculates distance from user's coordinated to station's coordinates.
* @param userCoords: Coordinates from user's input.
* @param stationCoords: Coordinates of station.
* @return Distance of type double.
*/
double calcDistance(Coordinates userCoords, Coordinates stationCoords);

/**
* Finds the station node with minimum ID in the subTree.
* @param station: Pointer to the root of the subtree.
* @return Pointer to the station with smallest ID.
*/
Station* minValueNode(Station* station);

/**
* Deletes a station from the BST by its ID.
* @param stationManager: Pointer to the station BST manager.
* @param stationID: ID of the station to delete.
* @return True if deletion successful, False if failed.
*/
bool deleteStation(stationBST_M* stationManager, int stationID);

/**
* Recursively deletes a station node from BST by ID.
* @param root: Pointer to the root of the subtree.
* @param stationID: ID of the station to delete.
* @param deleted: Pointer to a bool to indicate whether deletion occurred.
* @return Pointer to the new root of the subtree.
*/
Station* deleteStationNode(Station* root, int stationID, bool* deleted);

/**
* Searches for the station that owns the given port.
* @param station: Pointer to the root of the BST.
* @param targetPort: Pointer to the target port.
* @return Pointer to the station containing the port, or NULL if not found.
*/
Station* findStationWithPort(Station* station, Port* targetPort);

/**
* Searches for a station that has a given car in its queue.
* @param root: Pointer to the root of the BST.
* @param car: Pointer to the Car to search for.
* @return Pointer to the station with the car in its queue, or NULL if not found.
*/
Station* findStationByCarInQueue(Station* root, Car* car);

/**
* Prints all stations in the BST in in-order traversal.
* @param stationManager: Pointer to the station BST manager.
*/
void printStationTree(stationBST_M* stationManager);

/**
* Frees all memory allocated for the station BST.
* @param stationManager: Pointer to the station BST manager.
*/
void destroyStationTree(stationBST_M* stationManager);

/**
* Updates the stations data file with current state of the station BST.
* @param stationManager: Pointer to the station BST manager.
* @param filename: Name of the file to write the station data to.
*/
void updateStationFiles(stationBST_M* stationManager, const char* filename);


/**
* (Debug) Prints all cars assigned to ports in a specific station.
* @param station: Pointer to the station.
*/
void printAssignedCarsInStationTest(Station* station);

/**
* (Debug) Prints all cars assigned to ports in every station in the tree.
* @param manager: Pointer to the station BST manager.
*/
void printAllAssignedCarsTest(stationBST_M* manager);

/**
* Converts a Date object to the total amount of minutes since the start of charge.
* @param date: Date object.
* @returns Total number of minutes.
*/
int dateToMinutes(Date date);



#endif

