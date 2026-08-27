
#ifndef PORTS_H
#define PORTS_H

#include "car.h"
#include "date.h"
#include <stdbool.h>
#include <time.h>
#include "date.h"

typedef struct Car Car;
typedef struct Station Station;
typedef struct stationBST_M stationBST_M;

typedef enum
{
	FAST,
	MID,
	SLOW
}PortType;

typedef enum portStatus
{
	OCCUPIED = 1,
	FREE, // = 2
	OUT_OF_ORDER // = 3

}portStatus;

typedef struct Port
{
	int num;
	PortType portType;
	portStatus status; // occupied/free/outOfOrder
	Car* p2car;
	Date tin;
	struct Port* next;
}Port;


/**
* Add new port to station port list.
* @param stationManager: Pointer of the station BST manager.
* @return True if successfully added port, False if adding failed.
*/
bool addNewPort(stationBST_M* stationManager);


/**
* Remove port corresponding with given port number and frees allocated memory.
* @param station: Pointer to the station including the target port
* @param portNum: Number of the port designated to be removed.
* @return True if port removed successfully, False if removal failed.
*/
bool removePort(Station* station, int portNum); 

/**
* Finds the port corresponding with given port number.
* @param head: Head of port list.
* @param portNum: Number of the targeted port.
* @return Pointer to the found port, NULL if none found.
*/
Port* findPort(Port* head, int portNum);

/**
* Frees port list allocated memory.
* @param portList: Pointer to head of port list.
*/
void freePortList(Port* portList);

/**
* Assigns Car objects to ports (from saved data files)
* @param stationManager: Pointer of the station BST manager.
* @param stationID: ID number of the station.
* @param portNum: Number of the target port.
* @param car: Pointer to a Car object to assign to port.
* @return True if assigning successful, False if failed.
*/
bool LoadCar2port(stationBST_M* stationManager, int stationID, int portNum, Car* car);

/**
* Assigns a Car to the specified port and sets the current time as the start time.
* @param targetPort: Pointer to the targeted port.
* @param car: Pointer to a Car object we want to assign.
* @return True if assignment successful, False if failed.
*/
bool assignCar2port(Port* targetPort, Car* car);

/**
* Asks user for port type input.
* @return int of type enum PortType.
*/
PortType getValidPortTypeInput();

/**
* Returns the current system time as a Date object.
* @return Current time.
*/
Date getCurrentTime();

/**
* Prints the list of ports in order from head to tail.
* @param portsList: Pointer to the head of the list.
*/
void printPortList(Port* portList);

/**
* Searches for a port of a specific type.
* @param portsList: Pointer to the head of the list.
* @param type: Int of type PortType.
* @return Pointer to found port, NULL if no port found.
*/
Port* findFreePortOfType(Port* portsList, PortType type);

/**
* Prints all cars that are currently charging in station's ports
* @param portList: Pointer to the head of the list.
*/
void printChargingCars(Port* portList);

/**
* Converts int of type PortType into strings.
* @param: type: Int of PortType.
* @return Corresponding string.
*/
const char* portTypeToStr(PortType type);

/**
* Checks if a specific port is out of order.
* @param station: Pointer to the station containing the port.
* @param portNum: Number of the target port.
* @return True if port is out of order, False if not.
*/
bool isPortOutOfOrder(Station* station, int portNum);

/**
* Updates the port data file with the current state of all station ports.
* @param stationManager: Pointer of the station BST manager.
* @param filename: Name of the file to be updated.
*/
void updatePortFiles(stationBST_M* stationManager, const char* filename);

#endif

