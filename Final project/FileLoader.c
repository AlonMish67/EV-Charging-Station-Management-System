#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Stations.h"
#include "carQueue.h"
#include "ports.h"
#include "errors.h"

#define MAX_SIZE 256
#define INVALID_LICENSE "-1"
#define STATIONS_FILE_NAME "Stations.txt"
#define CARS_FILE_NAME "cars.txt"
#define PORTS_FILE_NAME "Ports.txt"
#define LINE_OF_CARS_FILE_NAME "LineOfCars.txt"



static PortType convertStr2PortType(const char* typeStr)
{ // Convert string to enum value
    // (PortType) casting are to avoid warnings
    if (!typeStr)
    {
        displayError(ERR_NULL_POINTER);
        return -1;
    }

    if (strcmp(typeStr, "FAST") == 0) 
        return (PortType)FAST;
    if (strcmp(typeStr, "MID") == 0) 
        return (PortType)MID;
    if (strcmp(typeStr, "SLOW") == 0) 
        return (PortType)SLOW;

    return (PortType)- 1; //incase of an UNKNOWN portType
}

void loadStationsToTree(const char* filename, stationBST_M* manager) 
{
    //check for NULL pointer
    if (!manager)
    {
        displayError(ERR_NULL_POINTER);
        return;
    }
    FILE* file = fopen(filename, "r");
    if (!file) 
    {
        printf("Error: Cannot open file %s\n", filename);
        return;
    }

    char line[MAX_SIZE];

    fgets(line, sizeof(line), file); // Skip file header

    while (fgets(line, sizeof(line), file)) 
    {
        Station* newStation = (Station*)malloc(sizeof(Station));
        if (!newStation) 
        {
            displayError(ERR_MEMORY_ALLOCATION);
            continue; //continue to next line
        }

        newStation->name = NULL; // default (to avoid warnings)
        char nameBuffer[MAX_SIZE] = { 0 };
        double x, y;

        // Format of line: ID,StationName,NumOfPorts,CoordX,CoordY
        int loaded = sscanf(line, "%d,%99[^,],%d,%lf,%lf",
            &newStation->id, nameBuffer, &newStation->nPorts, &x, &y);
        if (loaded != 5) 
        {
            printf("Invalid line format: %s", line);
            free(newStation);
            continue; //skip the line if format is invalid
        }

        // Copy name and coordinates
        newStation->name = _strdup(nameBuffer); // dynamically allocates name
        newStation->coord.x = x;
        newStation->coord.y = y;
        newStation->portsList = NULL; //inits list as NULL
        newStation->nCars = 0; //at the start - no cars so 0
        newStation->carQueue = (qCar*)malloc(sizeof(qCar));
        if (!newStation->carQueue)
        {
            displayError(ERR_MEMORY_ALLOCATION);
            return;
        }
        initCarQueue(newStation->carQueue); //inits carQueue as NULL at the start.
        newStation->left = NULL;
        newStation->right = NULL;

        printf("\nLoaded station: %s, with stationID: %d, number of ports: %d, coordX: %.2lf, coordY: %.2lf\n", newStation->name, newStation->id, newStation->nPorts, newStation->coord.x, newStation->coord.y); //debug print

        // Insert to BST
        if (!insertStation(manager, newStation)) //TRUE == insertion completed
        { //if insert fails
            printf("Something went wrong with inserting station %s , with ID : %d Skipping...\n", newStation->name, newStation->id);
            free(newStation->name);
            free(newStation);
            continue;
        }

        printf("Inserted station: %s To the BST.\n", newStation->name);
    }

    fclose(file);
    printf("Finished loading stations!\n\n");
}

void loadPortsToStations(const char* filename, stationBST_M* stationManager, car_BST* carManager) 
{
    //check for NULL pointers
    if (!stationManager || !carManager)
    {
        displayError(ERR_NULL_POINTER);
        return;
    }

    FILE* file = fopen(filename, "r");
    if (!file) 
    {
        printf("Error opening ports file: %s\n", filename);
        return;
    }

    int stationID, portNum, status;
    int year, month, day, hour, min;
    char typeStr[MAX_SIZE] = { 0 };
    char license[MAX_SIZE] = { 0 };

    char line[MAX_SIZE];
    fgets(line, sizeof(line), file); // Skip header

    while (fgets(line, sizeof(line), file))
    {
        // Format of line: StationID,PortNumber,PortType,Status,Year,Month,Day,Hour,Min,CarLicense
        int loaded = sscanf(line, "%d,%d,%9[^,],%d,%d,%d,%d,%d,%d,%19s",
            &stationID, &portNum, typeStr, &status,
            &year, &month, &day, &hour, &min, license);

        if (loaded != 10)
        {
            printf("Skipping invalid line: %s\n", line);
            continue; //if any line of the txt isnt by format, skip it.
        }

        printf("loaded data: StationID=%d, PortNum=%d, Type=%s, Status=%d, Date=%04d-%02d-%02d %02d:%02d, License=%s\n",
            stationID, portNum, typeStr, status, year, month, day, hour, min, license); //debug print

        Station* station = findStationByID(stationManager, stationID);
        if (!station) 
        {
            printf("Station ID %d not found. Skipping port.\n", stationID);
            continue; //skips line
        }

        Port* newPort = (Port*)malloc(sizeof(Port));
        if (!newPort) 
        {
            displayError(ERR_MEMORY_ALLOCATION);
            fclose(file);
            return;
        }
        //inits newPort`s fields.
        newPort->num = portNum; //port`s ID
        newPort->portType = convertStr2PortType(typeStr); 
        newPort->status = status; //occupied / free / out of order
        newPort->tin.year = year;
        newPort->tin.month = month;
        newPort->tin.day = day;
        newPort->tin.hour = hour;
        newPort->tin.min = min;
        newPort->next = NULL;
        newPort->p2car = NULL;

        if (strcmp(license, INVALID_LICENSE) == 0) 
        {
            newPort->p2car = NULL;
        }
        else 
        {
            Car* car = searchCar(carManager, license);
            if (!car)
            {
                printf("Car with license '%s' not found for port %d. Assigned as NULL.\n", license, portNum); //debug print
                newPort->p2car = NULL;
            }
            else 
            {
                newPort->p2car = car;
                car->pPort = newPort;
                newPort->status = 1;  // Mark port occupied
                printf("\nCar %s assigned to the port!\n\n", car->nLicense); //debug print
            }
        }
        // Add to station's ports list (insert at front)
        newPort->next = station->portsList;
        station->portsList = newPort;
        int count = 0;
        // traverse the linked list of ports for the station and count the total number of ports.
        for (Port* p = station->portsList; p != NULL; p = p->next) 
        {
            count++;
        }

        //makes sure the station gets the currect number of ports from file. 
        if (count > station->nPorts) 
        {
            printf("Warning: Station %d has more ports than expected (%d).\n",stationID, station->nPorts); //debug print
        }

    } //end of while loop

    fclose(file);
    printf(" Finished loading ports from file: %s\n\n", filename); //debug print
}

void loadCarsToTree(const char* filename, car_BST* carManager) 
{
    //check for NULL pointer
    if (!carManager) 
    {
        displayError(ERR_NULL_POINTER);
        return;
    }

    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Failed to open car file: %s\n", filename); //debug print
        return;
    }

    char line[MAX_SIZE];
    fgets(line, sizeof(line), file); // Skip header

    while (fgets(line, sizeof(line), file)) 
    {
        char license[MAX_SIZE] = { 0 };
        char portTypeStr[MAX_SIZE] = { 0 };
        double totalPayed;
        int stationID, portNum, inQueueInt;
        
        //Format of line: License,PortType,TotalPayed,StationID,PortNumber,InQueue
        int loaded = sscanf(line, "%9[^,],%9[^,],%lf,%d,%d,%d",
            license, portTypeStr, &totalPayed,
            &stationID, &portNum, &inQueueInt);

        if (loaded != 6) 
        {
            printf("Invalid line format in car file: %s\n", line); //debug print
            continue; //skips if line format is not valid
        }

        Car* newCar = (Car*)malloc(sizeof(Car));
        if (!newCar) 
        {
            displayError(ERR_MEMORY_ALLOCATION);
            break;
        }
        //init newCar fields with data from file
        strcpy(newCar->nLicense, license);
        newCar->portType = convertStr2PortType(portTypeStr); //converts str loaded from .txt to PortType (enum)
        newCar->totalPayed = totalPayed;
        newCar->inqueue = inQueueInt;
        newCar->pPort = NULL;

        // Insert car into BST
        if (insertCar(carManager, newCar)) 
        {
            printf("Car with %s, port type: %s, total payed: %.2lf, stationID: %d, port number: %d, inQueue: %d, got inserted!\n\n", newCar->nLicense, portTypeStr, newCar->totalPayed, stationID, portNum, newCar->inqueue); //debug print
            carManager->count++;
        }
        else //if insertion failed
        {
            printf("Insertion failed or duplicate license: %s\n", license); //debug print
            free(newCar);
        }
    }
    fclose(file);
    printf("Finished loading the cars!\n"); //debug print
}

void assignQueuedCarsToStations(const char* filename, stationBST_M* stationManager, car_BST* carManager) 
{
    //check for NULL pointers
    if (!stationManager || !carManager || !filename)
    {
        displayError(ERR_NULL_POINTER);
        return;
    }

    FILE* file = fopen(filename, "r");
    if (!file) 
    { 
        printf("Error opening LineOfCars file: %s\n", filename);  //debug print
        return;
    }

    char license[MAX_SIZE] = { 0 };
    int stationID;

    char line[MAX_SIZE] = { 0 };
    fgets(line, sizeof(line), file); // Skip header

    while (fgets(line, sizeof(line), file))
    {

        //Format of line: License,StationID
        int loaded = sscanf(line, "%19[^,],%d", license, &stationID);
        if (loaded != 2) 
        {
            printf("Invalid line format: %s", line); //debug print
            continue; //if invalid line format , skip the line.
        }

        // Find the car by license
        Car* car = searchCar(carManager, license);
        if (!car) 
        {
            printf("Car with license %s not found!\n", license); //debug print
            exit(1);
        }

        // Find the station by ID
        Station* station = findStationByID(stationManager, stationID);
        if (!station) 
        {
            printf("Station with ID %d not found for car %s!\n", stationID, license); //debug print
            exit(1);
        }

        // Check if car already assigned
        if (car->pPort != NULL) 
        {
            printf("Car %s is already assigned to a port. Skipping.\n", license); //debug print
            continue;
        }
        else 
        {
            // No free port -> add to station`s queue
            enqueueCar(station, car);
            printf("No port available. Enqueued car %s at station %d\n", license, stationID); //debug print
        }
    }

    fclose(file);
    printf("\n\n assigning or queuing cars from file: %s is complete\n\n", filename); //debug print
}

void loadFiles(stationBST_M* stationManager, car_BST* carManager)
{
    //check for NULL pointers
    if (!stationManager || !carManager)
    {
        displayError(ERR_NULL_POINTER);
        return;
    }

    loadStationsToTree(STATIONS_FILE_NAME, stationManager);
    loadCarsToTree(CARS_FILE_NAME, carManager);
    loadPortsToStations(PORTS_FILE_NAME, stationManager, carManager);
    assignQueuedCarsToStations(LINE_OF_CARS_FILE_NAME, stationManager, carManager);

    printAllAssignedCarsTest(stationManager); //prints to check everything got loaded
}