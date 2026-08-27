#ifndef MENUFUNCS_H
#define MENUFUNCS_H
#include "stations.h"

// Prints menu
void printMenu();

/**
* Asks the user for the car`s license plate.
* @param licenseBuffer: Buffer to store string (license plate)
* @param bufferSize: Size of the license buffer array.
* @return True if the license input is valid, False if the license is invalid.
*/
bool validLicenseInput(char* licenseBuffer, int bufferSize);

/**
* Searches for a station based on the user`s coordinates.
* @param stationManager: Pointer to the station BST manager
*/
void locNearSt(stationBST_M* stationManager);

/**
* Calls the corresponding function based on the selected action.
* @param userChoice: Action number selected by the user from the menu.
* @param stationManager: Pointer to the station BST manager.
* @param carManager: Pointer to the car BST manager.
*/
void menuFuncs(int userChoice, stationBST_M* stationManager, car_BST* carManager);


/**
* Sets a car to a charging post, If car does not exist in the BST,a new car is created.
* @param stationManager: Pointer to the station BST manager.
* @param: carManager: Pointer to the car BST manager.
* @return True if the car was successfully set to charge or enqueued, False if insertion failed.
*/
bool chargeCar(stationBST_M* stationManager, car_BST* carManager);

/**
* Stops the charging process for a car, updates the payment, and sets a new car to charge if queued.
* @param stationManager: Pointer to the station BST manager.
* @param: carManager: Pointer to the car BST manager.
* @return True when stop charging process successful, False if stop charge proccess failed.
*/
bool stopCharge(stationBST_M* stationManager, car_BST* carManager);

/**
* Print the details of all stations.
* @param stationManager: Pointer to the station BST manager.
*/
void dispAllSt(stationBST_M* stationManager);

/**
* Prints all cars that are currently charging or in queue at a station.
* @param stationManager: Pointer to the station BST manager.
*/
void dispCarsAtSt(stationBST_M* stationManager);

/**
* Prints the charging status of a car.
* @param stationManager: Pointer to the station BST manager.
* @param carManager: Pointer to the car BST manager.
*/
void checkCarStatus(stationBST_M* stationManager, car_BST* carManager);


/**
* Prints statistics for a station.
* @param stationManager: Pointer to the station BST manager.
*/
void reportStStat(stationBST_M* stationManager);

/**
* Prints top 5 customers who paid the most.
* @param carManager: Pointer to the car BST manager.
*/
void dispTopCustomers(car_BST* carManager);

/**
* Stops charging process of cars that have exceeded the 10-hour time limit.
* @param stationManager: Pointer to the station BST manager.
*/
void releasePorts(stationBST_M* stationManager);

/**
* Frees the allocated memory for ports that are out of order (based on user input).
* @param stationManager: Pointer to the station BST manager.
* @return True if port was successfully removed, False if removal failed.
*/
bool remOutOrderPort(stationBST_M* stationManager);

/**
* Asks for station name.
* @param stationNameBuffer: Buffer to store user`s input (Station name).
* @param stationNameSize: Size of station name buffer.
* @return True if user confirmed their input.
*/
bool stationNameInput(char* stationNameBuffer, int stationNameSize);

/**
* Removes a car from the car BST and frees allocated memory.
* @param carManager: Pointer to the car BST manager.
* @return True if car was successfully removed, False if removal failed.
*/
bool remCustomer(car_BST* carManager);

/**
* Removes a station from the station BST and frees allocated memory.
* @param stationManager: Pointer to the station BST manager.
* @return True if the station was successfully removed, False if removal failed.
*/
bool closeSt(stationBST_M* stationManager);

/**
* Frees all allocated memory in the system.
* @param stationManager: Pointer to the station BST manager.
* @param carManager: Pointer to the car BST manager.
*/
void exitSys(stationBST_M* stationManager, car_BST* carManager);

/**
* Updates all data files with the current system state.
* @param stationManager: Pointer to the station BST manager.
* @param carManager: Pointer to the car BST manager.
*/
void updateFiles(stationBST_M* stationManager, car_BST* carManager);


// Clears cmd screen
void clearScreen();


#endif
