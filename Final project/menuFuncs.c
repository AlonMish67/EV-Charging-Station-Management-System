#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <ctype.h> //needed for isdigit()
#include "stations.h"
#include "ports.h"
#include "errors.h"

#define LICENSE_SIZE 9
#define STATION_NAME_SIZE 256
#define ILS_PER_MIN 1.2
#define CHARGE_LIMIT_TIME (10*60)
#define STATIONS_FILE_NAME "Stations.txt"
#define CARS_FILE_NAME "cars.txt"
#define PORTS_FILE_NAME "Ports.txt"
#define LINE_OF_CARS_FILE_NAME "LineOfCars.txt"

void clearScreen()
{
#if defined (_WIN32)
    system("cls");

#elif  defined (linux)  defined (unix)   defined (APPLE)
    system("clear");

#else

#endif 

}

static void printLine()
{
    printf("-------------------------------\n");
}

void printMenu()
{
    printf(
        "1) Locate nearest station\n"
        "2) Charge car\n"
        "3) Check car status\n"
        "4) Stop charge\n"
        "5) Display all stations\n"
        "6) Display cars at station\n"
        "7) Report of station`s statictics\n"
        "8) Display Top customers\n"
        "9) Add new port\n"
        "10) Release Charging ports\n"
        "11) Remove out of order ports\n"
        "12) Remove customer\n"
        "13) Close station\n"
        "0) Exit system\n"
        "Please choose an action: ");
}

static int getChargingDuration(Date startTime)
{
    Date now = getCurrentTime();
    int minuteNow = dateToMinutes(now);
    int minutesStart = dateToMinutes(startTime);
    int duration = minuteNow - minutesStart;

    return duration;
}


bool validLicenseInput(char* licenseBuffer, int licenseSize)
{
    //check for NULL pointers, and valid license buffer size
    if (!licenseBuffer || licenseSize <= 0)
    {
        if (licenseSize <= 0)
            displayError(ERR_INVALID_BUFFER_SIZE);
        displayError(ERR_NULL_POINTER);
        return false;
    }
    char temp[100] = { 0 }; //buffer to store license input with ensured '\0'
    char confirm;

    while (1) //loop until input is valid and confirmed
    {
        printf("Enter car license plate: ");
        if (fgets(temp, sizeof(temp), stdin) == NULL)
        {
            displayError(ERR_INVALID_INPUT);
            continue;
        }
        //remove newline and set '\0' instead.
        temp[strcspn(temp, "\n")] = '\0';

        if (strlen(temp) != LICENSE_SIZE - 1) //check for valid length (8 chars)
        {
            displayError(ERR_INVALID_LICENSE_LENGTH);
            continue;
        }

        //check that all characters are digis
        bool isValid = true;
        for (int i = 0; i < LICENSE_SIZE - 1; i++)
        {
            if (!isdigit(temp[i])) //if some char is NOT a digit -> isValid = false
            {
                isValid = false;
                break;
            }
        }
        if (!isValid)
        {   //if not all characters are digits, ask to try again
            printf("License plate must contain digits only, please try again.\n\n");
            continue;
        }

        while (1)
        {
            printf("You entered: %s. Confirm? (y/n): ", temp);
            scanf(" %c", &confirm);

            if (confirm == 'y' || confirm == 'Y')
            {
                strcpy(licenseBuffer, temp);
                licenseBuffer[licenseSize - 1] = '\0'; //ensure NULL termination
                return true;
            }
            else if (confirm == 'n' || confirm == 'N')
            {
                printf("Canceled, please try again.\n");
                while (getchar() != '\n');
                break;
            }
            else
            {
                displayError(ERR_INVALID_INPUT);
                while (getchar() != '\n');
            }
        }
    }
}

bool stationNameInput(char* stationNameBuffer, int stationNameSize)
{
    //check for NULL pointer, and station buffer size
    if (!stationNameBuffer || stationNameSize <= 0)
    {
        if (stationNameSize <= 0)
            displayError(ERR_INVALID_BUFFER_SIZE);

        displayError(ERR_NULL_POINTER);
        return false;
    }

    char temp[STATION_NAME_SIZE] = { 0 };
    char confirm;

    while (1) //loop until valid and confirmed
    {
        printf("Enter station`s name: ");
        scanf(" %[^\n]", temp);
        while (getchar() != '\n');

        while (1) //confirmation loop
        {
            printf("You have entered: \"%s\". Confirm? (y/n): ", temp);
            scanf(" %c", &confirm);

            if (confirm == 'y' || confirm == 'Y')
            {
                strcpy(stationNameBuffer, temp);
                stationNameBuffer[STATION_NAME_SIZE - 1] = '\0'; //ensure NULL termination
                printf("\n");
                return true;
            }
            else if (confirm == 'n' || confirm == 'N')
            {
                printf("Canceled, please try again.\n");
                while (getchar() != '\n');
                break;
            }
            else
            {
                displayError(ERR_INVALID_INPUT);
            }
        }
    }
}

void locNearSt(stationBST_M* stationManager)
{
    //check for NULL pointers
    if (!stationManager )
    {
        displayError(ERR_NULL_POINTER);
        return;
    }
    if (!stationManager->root)
    {
        displayError(ERR_STATION_BST_EMPTY);
        return false;
    }
    Coordinates userCoords = { 0 };
    int validInput = 1;

    while (validInput == 1)
    {
        printf("Enter your coordinates in the format (x,y): ");

        //expects input in (x,y) format
        if (scanf(" (%lf,%lf)", &userCoords.x, &userCoords.y) == 2)
        {
            validInput = 0; //if valid input received
        }
        else
        {
            displayError(ERR_INVALID_INPUT_FORMAT);
            
            while (getchar() != '\n'); //clear input buffer.
        }
    }
    Station* nearest = searchStation(stationManager, SEARCH_BY_COORDS, &userCoords); //search station by coordinates

    if (nearest)
    {
        //calculate distance from user to the station
        double distance = calcDistance(userCoords, nearest->coord);
        printStation(nearest);
        printf("Station`s distance: %.2lf KM\n", distance);
    }
    else
    {
        printf("No nearest station found.\n");
    }
}


bool chargeCar(stationBST_M* stationManager, car_BST* carManager)
{
    //check for NULL pointers
    if (!stationManager || !carManager)
    {
        displayError(ERR_NULL_POINTER);
        return false;
    }
    if (!stationManager->root)
    {
        displayError(ERR_STATION_BST_EMPTY);
        return false;
    }
    char license[LICENSE_SIZE];

    if (validLicenseInput(license, LICENSE_SIZE)) //get a valid license plate input
    {
        printf("Confirmed license: %s\n\n", license);
    }

    Car* car = searchCar(carManager, license); //searches a car in car BST based in license plate

    if (car == NULL) //if car doesnt exist in carBST
    {
        PortType type;
        //ask the user for valid port type input, and than create car
        type = getValidPortTypeInput();
        car = createCar(license, type);

        if (!car)
        {
            displayError(ERR_MEMORY_ALLOCATION);
            return false;
        }

        if (!insertCar(carManager, car)) 
        {
            printf("Failed to insert car to BST.\n");
            free(car);
            return false;
        }

        Station* station = getStationByUserInput(stationManager);

        if (car->pPort != NULL) //sanity check - the new car shouldnt be already charging
        {
            printf("Car %s is already charging at a port.\n", car->nLicense);
            return false;
        }

        Port* freePort = findFreePortOfType(station->portsList, car->portType); //try to find a free port by car`s type
        if (freePort == NULL) //if none found - enqueue the car
        {
            printf("No available port of the required type at station \"%s\".\n", station->name);
            printf("Adding car to station's queue.\n");
            enqueueCar(station, car);
            return true;
        }
        else 
        {
            //if free port found - assign car
            if (assignCar2port(freePort, car))
            {
                printf("\nCar %s is now charging at port #%d of station %s.\n", car->nLicense, freePort->num, station->name);
                return true;
            }
            else
            {
                printf("Failed to assign car to port.\n");
                return false;
            }
        }
    } //end of car not existing part!

    else // if car found
    {
        if (car->pPort != NULL) //if car is already charging
        {
            printf("\nCar %s is already charging at a port.\n\n", car->nLicense);
            return false;
        }

        if (car->inqueue == 1) //check if car is already in some queue/line
        {
            printf("\nCar %s is already waiting in line.\n\n", car->nLicense);
            return false;
        }
        //search station according to user`s input and search for free port in the station.
        Station* station = getStationByUserInput(stationManager);
        Port* freePort = findFreePortOfType(station->portsList, car->portType);
        if (freePort == NULL) //if no free port was found - enqueue car
        {
            printf("\nNo available port of the required type at station %s.\n", station->name);
            printf("Adding car to station's queue.\n");
            enqueueCar(station, car);
            return true;
        }
        else //if a free port was found assign the car
        {
            if (assignCar2port(freePort, car))
            {
                printf("\nCar %s is now charging at port #%d of station %s.\n", car->nLicense, freePort->num, station->name);
                return true;
            }
            else
            {
                printf("Failed to assign car to port.\n");
                return false;
            }
        }
    }
}

void dispAllSt(stationBST_M* stationManager)
{
    //check for NULL pointer
    if (!stationManager)
    {
        displayError(ERR_STATION_NULL);
        return;
    }
    if (!stationManager->root)
    {
        displayError(ERR_STATION_BST_EMPTY);
        printLine();
        return;
    }

    printStationTree(stationManager);
}

void dispCarsAtSt(stationBST_M* stationManager)
{
    //check for NULL pointer
    if (!stationManager)
    {
        displayError(ERR_NULL_POINTER);
        return;
    }
    if (!stationManager->root)
    {
        displayError(ERR_STATION_BST_EMPTY);
        return;
    }

    Station* station = getStationByUserInput(stationManager); //find station according to user`s input
    if (!station)
    {
        printf("Station has not been found.\n");
        return;
    }
    printf("\nStation: %s\n", station->name);
    printChargingCars(station->portsList);
    printQueuedCars(station->carQueue);
}

static int getCarQueuePositionByType(qCar* queue, Car* car) //helper function
{
    //check for NULL pointers
    if (!queue || !car)
        return -1; //return -1 if either queue or car is NULL

    carNode* current = queue->front;
    int position = 1; 
    //traverse through the queue to find the car with the same port type
    while (current)
    {
        if (current->p2car && current->p2car->portType == car->portType)
        {
            if (current->p2car == car)
                return position; // if the current car in the queue matches the requested car, return position

            position++;
        }
        current = current->next;
    }
    return -1; //not found
}

void checkCarStatus(stationBST_M* stationManager, car_BST* carManager)
{
    if (!stationManager || !carManager)
    {
        displayError(ERR_NULL_POINTER);
        return;
    }
    if (!carManager->root)
    {
        displayError(ERR_CAR_BST_EMPTY);
        return;
    }

    char license[LICENSE_SIZE];

    if(!validLicenseInput(license, LICENSE_SIZE)) //get valid and confirmed license plate from user
    {
        printf("License input failed.\n");

        return;
    }
    printf("\n");

    Car* car = searchCar(carManager, license);
    if (!car)
    {
        printf("Car with license %s, does not exist in the system.\n", license);
        return;
    }

    if (car->pPort != NULL) //if the car is connected to a port
    {
        //find the station with the port the car is connected to.
        Port* port = car->pPort;
        Station* station = findStationWithPort(stationManager->root, port);
        if (!station)
        {
            displayError(ERR_STATION_NOT_FOUND);
            return;
        }
        //calculate charging time
        int duration = getChargingDuration(port->tin);

        printf("Car %s is current charging at \"%s\", at port #%d type: %s.\n",
            car->nLicense, station->name, port->num, portTypeToStr(port->portType));
        printf("  Car is charging for: %d minutes.\n", duration);
        return;
    }

    if (car->inqueue == 1) // car is in queue
    {
        //find station where car in enqueued
        Station* station = findStationByCarInQueue(stationManager->root, car);
        if (!station)
        {
            displayError(ERR_STATION_NOT_FOUND);
            return;
        }
        // get car`s position in queue
        int position = getCarQueuePositionByType(station->carQueue, car);
        if (position == -1)
        {
            printf("Error:Car is in queue, but not found in queue list...\n");
            return;
        }

        printf("Car %s is waiting in line at station \"%s\". \n", car->nLicense, station->name);
        printf("Car`s position in the queue by port type \"%s\": %d\n", portTypeToStr(car->portType), position);
        return;
    }

    //if car isnt in queue or not charging:
    printf("Car %s is currently not charging and not in a queue.\n", car->nLicense);
    return;
}


bool stopCharge(stationBST_M* stationManager, car_BST* carManager)
{
    if (!stationManager || !carManager)
    {
        displayError(ERR_NULL_POINTER);
        return false;
    }
    if (!stationManager->root)
    {
        displayError(ERR_STATION_BST_EMPTY);
        return false;
    }
    if (!carManager->root)
    {
        displayError(ERR_CAR_BST_EMPTY);
        return false;
    }

    char license[LICENSE_SIZE];

    if (!validLicenseInput(license, LICENSE_SIZE)) //get valid and confirmed license plate
    {
        displayError(ERR_INVALID_INPUT);
        return false;
    }
    printf("\n");
    
    Car* car = searchCar(carManager, license); 
    if (!car)
    {
        printf("  Car with %s does not exist in system.\n", license);
        return false;
    }

    if (!car->pPort) // if the car is not connected to a port
    {
        printf("  Car %s is not currently charging.\n", license);
        return false;
    }

    Port* port = car->pPort; // if the car is connected, find the station with the port
    Station* station = findStationWithPort(stationManager->root, port);
    if (!station)
    {
        displayError(ERR_STATION_NOT_FOUND);
        return false;
    }
    //calculate charging time
    int duration = getChargingDuration(port->tin);
    
    if (duration < 0)
        duration = 0;

    //calculate charging cost and update car fields
    float cost = duration * ILS_PER_MIN;
    car->totalPayed += cost;
    car->pPort = NULL;
    car->inqueue = 0;

    printf("Car %s has finished charging at station \"%s\",at port: #%d, type: %s\n", license, station->name, port->num, portTypeToStr(port->portType));
    printf("  Charging duration was: %d minutes. \n"
        "Total cost: %.2f ILS\n\n", duration, cost);

    //disconnect car from port
    port->p2car = NULL;
    port->status = FREE;
    port->tin = (Date){ 0 }; //0 all tin fields
    

    if(!assignNextCompatibleCarFromQueue(station, port)) //if there is a compatibble car in line, connect it to port
    {
        printf("\n");
    }
    return true;
}

static double sumOfOccupiedPorts(Station* station) // count the amount of occupied ports in the station
{
    //check for NULL pointers
    if (!station || !station->portsList)
        return 0;

    Port* port = station->portsList;

    int count = 0;
    while (port) //traverse through ports lsit
    {
        if (port->status == 1)
        {
            count++;
        }
        port = port->next;
    }
    return count;
}

static double sumOfOutOfOrderPorts(Station* station) // count the amount of out of order ports in the station
{
    // check for NULL pointers
    if (!station || !station->portsList)
    {
        return 0;
    }

    Port* port = station->portsList;
    int count = 0;
    
    while (port) //traverse through ports list
    {
        if (port->status == OUT_OF_ORDER)
        {
            count++;
        }
        port = port->next;
    }
    return count;
}


void reportStStat(stationBST_M* stationManager)
{
    if (!stationManager)
    {
        displayError(ERR_NULL_POINTER);
        return;
    }
    if (!stationManager->root)
    {
        displayError(ERR_STATION_BST_EMPTY);
        return;
    }
    char stationName[STATION_NAME_SIZE] = { 0 };

    Station* station = getStationByUserInput(stationManager); //find station by user input
    if (!station)
    {
        displayError(ERR_STATION_NOT_FOUND);
        return;
    }
    if (station->nPorts == NULL)
    {
        printf("Error: Station has no ports.\n");
        return;
    }
    printf("Station name: %s\n", station->name);
    printf("Charging ports efficency: %.2lf%%\n", (sumOfOccupiedPorts(station) / station->nPorts) * 100);

    printf("Out of order ports precentage: %.2lf%%\n", (sumOfOutOfOrderPorts(station)/station->nPorts)* 100);

    double carLoad;
   
    if (station->nCars == 0) //nCars = amount of cars waiting in station's queue
    {
        printf("There is no car load at the station at the moment\n");
        return;
    }

    carLoad = sumOfOccupiedPorts(station) / station->nCars;

    if (carLoad > 1)
    { 
        printf("  There is no car load at the station\n");
    }
    else if (carLoad == 1)
    {
        printf("  Station`s load is balanced.\n");
    }
    else if (carLoad < 1 && carLoad>0.2)
    {
        printf("  There is moderate load at the station.\n");
    }
    else
    {
        printf("  There is a heavy load of cars at the station.\n");
    }
}

static void updateTopPayer(Car* car, Car** topPayers)
{
    //Check for NULL pointers
    if (!car || !topPayers)
    {
        displayError(ERR_NULL_POINTER);
        return;
    }
    //add the car to the first available NULL slot in topPayers array
    for (int i = 0; i < 5; i++)
    {
        if (topPayers[i] == NULL)
        {
            topPayers[i] = car;
            return;
        }
    }
    // after all slots are full find the car with the lowest totalPayed
    int lowestIndex = 0;
    double lowestPay = DBL_MAX;
    for (int i = 0; i < 5; i++)
    {
      if (topPayers[i]->totalPayed < lowestPay)
      {
         lowestPay = topPayers[i]->totalPayed;
         lowestIndex = i;
      }
    }

    if (car->totalPayed > lowestPay) //if current car has paid more than the lowest in the list, replace it
    {
        topPayers[lowestIndex] = car;
    }
}

static void traversCarBST(const tCar* root, Car** topPayers) 
{
    //base case: reached to the end of the subtree/root
    if (!root)
    {
        return;
    }

    //traverse through right and left subtrees
    traversCarBST(root->left, topPayers);
    updateTopPayer(root->p2car, topPayers);
    traversCarBST(root->right, topPayers);
}

void dispTopCustomers(car_BST* carManager)
{
    //check for NULL pointers
    if (!carManager)
    {
        displayError(ERR_NULL_POINTER);
        return;
    }
    if (!carManager->root)
    {
        displayError(ERR_CAR_BST_EMPTY);
        return;
    }

    //initialize a pointers array
    Car* topPayers[5] = { 0 };
    traversCarBST(carManager->root, topPayers); //call recursive traversal function

    //sort customers from highest to lowest
    for (int i = 0; i < 4; i++)
    {
        for (int j = i + 1; j < 5; j++)
        {
            Car* carA = topPayers[i];
            Car* carB = topPayers[j];

            if (carA && carB && carB->totalPayed > carA->totalPayed)
            {
                topPayers[i] = carB;
                topPayers[j] = carA;
            }
        }
    }

    printf("Top 5 payers: \n");

    for (int i = 0; i < 5 && topPayers[i]; i++)
    {
        printf("Car license: %s, total payed: %.2lf ILS\n", topPayers[i]->nLicense, topPayers[i]->totalPayed);
    }
}

bool releasePorts(stationBST_M* stationManager)
{
    //Check for NULL pointers
    if (!stationManager)
    {
        displayError(ERR_NULL_POINTER);
        return false;
    }
    if (!stationManager->root)
    {
        displayError(ERR_STATION_BST_EMPTY);
        return false;
    }

    Station* station = getStationByUserInput(stationManager);
    if (!station)
    {
        displayError(ERR_STATION_NOT_FOUND);
        return false;
    }

    Port* port = station->portsList;
    bool portFreed = false;

    while (port) //traverse through ports list 
    {
        if (port->status == OCCUPIED && port->p2car) //if port is occupied and has a car charging
        {
            int duration = getChargingDuration(port->tin);
            if (duration < 0)
                duration = 0;

            if (duration >= CHARGE_LIMIT_TIME) //if the duration is about the 10h limit (600 minutes)
            {  
                Car* car = port->p2car;
                float cost = duration * ILS_PER_MIN;
                //update car fields
                car->totalPayed += cost;
                car->pPort = NULL;
                car->inqueue = 0;
               
                printf("Car %s exceeded 10h limit: disconnected from station \"%s\" - port #%d of type %s.\n",
                    car->nLicense, station->name, port->num, portTypeToStr(port->portType));

                printf("Charge time was: %d minutes.\nTotal cost: %.2f ILS\n",duration, cost);
                //realese port
                port->p2car = NULL;
                port->status = FREE;
                port->tin = (Date){ 0 };

                assignNextCompatibleCarFromQueue(station, port);

                portFreed = true;
                printf("\n");
                printLine();
            }
        }
        port = port->next;
    }
    
    return portFreed; //will return true only when at least 1 port has been freed.
}

bool remOutOrderPort(stationBST_M* stationManager)
{
    //check for NULL pointers
    if (!stationManager )
    {
        displayError(ERR_NULL_POINTER);
        return false;
    }
    if (!stationManager->root)
    {
        displayError(ERR_STATION_BST_EMPTY);
        return false;
    }

    Station* station = NULL;
    int stationID;

    while (!station) //while a station is not found
    {
        printf("Please enter the station`s ID: ");
        if (scanf("%d", &stationID) != 1)
        {
            displayError(ERR_INVALID_INPUT);
            while (getchar() != '\n');
            continue;
        }

        station = searchStation(stationManager, SEARCH_BY_ID, &stationID);
        if (!station)
        {
            displayError(ERR_STATION_NOT_FOUND);
        }
    }

    Port* port = station->portsList;
    
    int hasOutOfOrderPort = 0;
    printf("\nOut of order ports at station: \"%s\":\n", station->name);

    while (port) //traverse through ports list
    {
        if (port->status == OUT_OF_ORDER) //search for out of order ports
        {
            printf("  Port #%d of type: %s.\n", port->num, portTypeToStr(port->portType));
            hasOutOfOrderPort = 1;
        }
        port = port->next;
    }
    printf("\n");
    if (!hasOutOfOrderPort) //if the station doesnt have out of order ports
    {
        printf("  No out of order ports found at this station.\n");
        return false;
    }

    int portNum;
    while (1) //loop until valid input
    {
        printf("Enter port`s number to remove: ");
        if (scanf("%d", &portNum) == 1)
        {
            if (isPortOutOfOrder(station, portNum))
            {

                break; //if valid input - exit loop.
            }
            else
            {
                printf("Port #%d is not out of order or does not exist.\nTry again.\n\n", portNum);
            }
        }
        else
        {
            displayError(ERR_INVALID_INPUT);
            while (getchar() != '\n');
        }
    }

    if (removePort(station, portNum)) // check if port removed successfully
    {
        printf("Port #%d removed successfully.\n", portNum);
        station->nPorts--;
        return true;
    }
    else
    {
        printf("Port #%d not found.\n", portNum);
        return false;
    }
}

bool remCustomer(car_BST* carManager)
{
    //check for NULL pointers
    if (!carManager)
    {
        displayError(ERR_NULL_POINTER);
        return false;
    }
    if (!carManager->root)
    {
        displayError(ERR_CAR_BST_EMPTY);
        return false;
    }

    char license[LICENSE_SIZE];

    if (validLicenseInput(license, LICENSE_SIZE)) //get valid license plate from user
    {
        printf("Confirmed license: %s\n\n", license);
    }

    Car* car = searchCar(carManager, license);
    if (!car)
    {
        printf("Car with license %s not found.\n", license);
        return false;
    }

    if (car->pPort != NULL || car->inqueue == 1) //check if car is charging or is in some queue
    {   //if car is in queue or charging, it cannot be removed
        printf("Unable to remove the car %s.\nCar is either charging or in waiting in queue.\n", license);
        return false;
    }

    bool deleted = deleteCar(carManager, license);
    if (deleted) //check if car deletion was successful
    {
        printf("  Car %s successfully removed.\n", license);
        return true;
    }
    else
    {
        printf("  Failed to delete car %s.\n", license);
        return false;
    }
}

bool closeSt(stationBST_M* stationManager)
{
    //Check for NULL pointers
    if (!stationManager)
    {
        displayError(ERR_NULL_POINTER);
        return false;
    }
    if (!stationManager->root)
    {
        displayError(ERR_STATION_BST_EMPTY);
        return false;
    }

    Station* station = getStationByUserInput(stationManager);
    if (!station)
    {
        displayError(ERR_STATION_NOT_FOUND);
        return false;
    }

    

    char confirm;
    while (1)
    {
        printf("Are you sure you want to close station \"%s\"? (y/n): ", station->name);
        scanf(" %c", &confirm);
        if (confirm == 'y' || confirm == 'Y')
        {
            printf(" Operation confirmed.\n");
            while (getchar() != '\n');
            break;
        }
        else if (confirm == 'n' || confirm == 'N')
        {
            printf(" Operation canceled.\n");
            return false;
        }
        else
        {
            displayError(ERR_INVALID_INPUT);
            continue;
        }
    }

    Port* port = station->portsList;
    while (port) //traverse through ports list
    {
        if (port->status == 1 && port->p2car != NULL) //check if there are cars charging
        {
            printf("Cannot close a station while cars are charging.\n");
            return false;
        }
        port = port->next;
    }

    if (station->carQueue->front != NULL) //check if there is a queue
    {
        printf("Cannot close a station while cars are waiting in line.\n");
        return false;
    }


    if (deleteStation(stationManager, station->id)) //check if deletion was successful
    {
        printf("Station has closed successfully.\n");
        return true;
    }
    else
    {
        printf("Failed to delete the station.\n");
    }
    return false;
}

void exitSys(stationBST_M* stationManager, car_BST* carManager)
{
    //check for NULL pointers
    if (!stationManager || !carManager)
    {
        displayError(ERR_NULL_POINTER);
        return;
    }
    //release all allocated memory - first car BST, then portsList and car queues, lastly free station BST
    destroyCarTree(carManager);
    destroyStationTree(stationManager);
}

void updateFiles(stationBST_M* stationManager, car_BST* carManager)
{
    //check for NULL pointers
    if (!stationManager || !carManager)
    {
        displayError(ERR_NULL_POINTER);
        return;
    }
    //update all files
    updatePortFiles(stationManager, PORTS_FILE_NAME);
    updateCarQueueFile(stationManager, LINE_OF_CARS_FILE_NAME);
    updateStationFiles(stationManager, STATIONS_FILE_NAME);
    updateCarFiles(carManager, stationManager, CARS_FILE_NAME);
}


void menuFuncs(int userChoice, stationBST_M* stationManager, car_BST* carManager)
{   
    clearScreen();
    switch (userChoice)
    {
    case 0: //exit system
    {
        updateFiles(stationManager, carManager);
        exitSys(stationManager, carManager);
        printf("\nGoodBye\n");
        break;
    }
    case 1: //located nearest station
    {
        locNearSt(stationManager);
        printLine();
        break;
    }
    case 2: //charge car
    {
        if (chargeCar(stationManager, carManager))
        {
            updateFiles(stationManager, carManager);
        }
        printLine();
        break;
    }
    case 3: //check car status
    {
        checkCarStatus(stationManager, carManager);
        printLine();
        break;
    }
    case 4: //stop charge
    {
        if (stopCharge(stationManager, carManager))
        {
            updateFiles(stationManager, carManager);
        }
        printLine();
        break;
    }
    case 5: //display all stations
    {
        dispAllSt(stationManager);
        break;
    }
    case 6: //display cars at station
    {
        dispCarsAtSt(stationManager);
        printLine();
        break;
    }
    case 7: //report station stats
    {
        reportStStat(stationManager);
        printLine();
        break;
    }
    case 8: //display top customers
    {
        dispTopCustomers(carManager);
        printLine();
        break;
    }
    case 9: //add new port
    {
        if(addNewPort(stationManager))
        {
            updateFiles(stationManager, carManager);
        }
        printLine();
        break;
    }
    case 10: //rlease ports (cars over 10h limit)
    { 
        if (releasePorts(stationManager))
        {
            updateFiles(stationManager, carManager);
        }
        else
        {
            printf("No cars exceeded the 10h limit at this station.\n");
            printLine();
        }
        break;
    }
    case 11: //remove out of order ports
    {
        if (remOutOrderPort(stationManager))
        {
            updatePortFiles(stationManager, "Ports.txt");
            updateStationFiles(stationManager, "Stations.txt");
        }
        printLine();
        break;
    }
    case 12: //remove customer
    {
        if (remCustomer(carManager))
        {
           updateCarFiles(carManager, stationManager, "Cars.txt");
        }
        printLine();
        break;
    }
    case 13: //close station
    {
        if (closeSt(stationManager))
        {
            updateStationFiles(stationManager, "Stations.txt");
            updatePortFiles(stationManager, "Ports.txt");
        }
        printLine();
        break;
    }
    default: //invalid choice
        printf("\nInvalid choice.\n\n");
        break;
   }
}