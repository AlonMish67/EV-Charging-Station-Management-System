#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "Ports.h"
#include "stations.h"
#include "errors.h"

PortType getValidPortTypeInput()
{
    int portTypeInput;
    char confirm;
    while (1)
    {
        printf("Enter port type: (0 - FAST | 1 - MID | 2 - SLOW): ");
        
        if(scanf(" %d", &portTypeInput) == 1)
        {
            if (portTypeInput >= FAST && portTypeInput <= SLOW) //checks if port type is valid
            {
                while (1) //confirmation loop
                {
                    printf("You have entered port type: %s. Confirm? (y/n): ", portTypeToStr(portTypeInput));
                    scanf(" %c", &confirm);

                    if (confirm == 'y' || confirm == 'Y')
                    {
                        printf("Confirmed port type: %s.\n\n", portTypeToStr(portTypeInput)); 
                        while (getchar() != '\n');
                        return (PortType)portTypeInput;
                    }
                    else if(confirm == 'n' || confirm == 'N')
                    {
                        printf("Canceled, please try again.\n");
                        while (getchar() != '\n');
                        break; //break confirmation loop
                    }
                    else
                    {
                        displayError(ERR_INVALID_INPUT);
                        while (getchar() != '\n'); //clear buffer
                    }
                }
            }
            else
            {
                displayError(ERR_INVALID_PORT_TYPE);
                while (getchar() != '\n');
            }
        }
        else
        {
            displayError(ERR_INPUT_NOT_NUM);
            while (getchar() != '\n');
        }
    }
}

bool removePort(Station* station, int portNum)
{
    //check for NULL pointer.
    if (!station)
    {
        displayError(ERR_NULL_POINTER);
        return false;
    }

    Port* current = station->portsList;
    Port* prev = NULL;

    while (current) //traverse through ports list
    {
        if (current->num == portNum)
        {
            if (prev)
                prev->next = current->next;
            else
                station->portsList = current->next;

            free(current);
            return true;
        }
        prev = current;
        current = current->next;
    }
    return false;
}

bool isPortOutOfOrder(Station* station, int portNum) 
{
    //check NULL pointers
    if (!station)
    {
        displayError(ERR_NULL_POINTER);
        return false;
    }

    Port* port = station->portsList;
    while (port) //traverse through port list
    {
        if (port->num == portNum)
            return (port->status == OUT_OF_ORDER);

        port = port->next;
    }
    return false;
}


bool addNewPort(stationBST_M* stationManager)
{
    //check for NULL pointers
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


    PortType type;
    int validInput = 1;

    Station* station = getStationByUserInput(stationManager);
    if (!station)
    {
        displayError(ERR_STATION_NOT_FOUND);
        return false;
    }
    type = getValidPortTypeInput();

    Port* newPort = (Port*)malloc(sizeof(Port));
    if (!newPort)
    {
        displayError(ERR_MEMORY_ALLOCATION);
        return false;
    }
    //init newPort fields
    newPort->portType = type;
    newPort->status = FREE;
    newPort->p2car = NULL;
    newPort->next = NULL;
    newPort->tin = (Date){ 0 };

    int portNum = 0;
    Port* port = station->portsList;
    while (port)
    {
        //printf("Found port num: %d\n", port->num); //debug print
        if (port->num > portNum)
            portNum = port->num;

        port = port->next;
    }
   // printf("Max port number found: %d\n", portNum); //debug print
    newPort->num = portNum + 1;
    //printf("Assigned new port number: %d\n", newPort->num); //debug print

    printf("Port #%d of type: %s, added successfully to station \"%s\". \n",newPort->num, portTypeToStr(newPort->portType), station->name); //debug print

    if (!assignNextCompatibleCarFromQueue(station, newPort))
    {
        printf("  Queue is empty. No car assigned to port #%d.\n", newPort->num); //debug print
    }

    if (!station->portsList)
    {
        station->portsList = newPort;
    }
    else
    {
        Port* last = station->portsList;
        while (last->next) //travers through ports list
            last = last->next;

        last->next = newPort; //appends new port at the end of the list
    }
    station->nPorts++;

    return true;
}

const char* portTypeToStr(PortType type) 
{    // (PortType) casting are to avoid warnings
    switch (type) 
    {
    case (PortType)FAST: 
        return "FAST";
    case (PortType)MID:
        return "MID";
    case (PortType)SLOW:
        return "SLOW";
    default:   
        return "UNKNOKN";
    }
}


Port* findPort(Port* head, int portNum) 
{
    Port* current = head;
    while (current != NULL)  //travers through ports list
    {
        if (current->num == portNum) 
            return current; //return Port* when a matching port was found

        current = current->next;
    }
    return NULL;
}


bool LoadCar2port(stationBST_M* stationManager, int stationID, int portNum, Car* car) // used to load cars to ports from .txt
{
    //Check NULL pointers
    if (!stationManager || !car)
    {
        //printf("LoadCar2port: stationManager or car is NULL\n"); //debug print
        displayError(ERR_NULL_POINTER);
        return false;
    }

    Station* station = findStationByID(stationManager, stationID);
    if (!station)
    {
        printf("LoadCar2port: station ID %d not found\n", stationID); //debug print
        return false;
    }


    Port* port = findPort(station->portsList, portNum);
    if (!port)
    {
        printf("LoadCar2port: port #%d not found in station %d\n", portNum, stationID); //debug print
        return false;
    }

    if (port->p2car != NULL) //checks if port is FREE
    {
        printf("LoadCar2port: Port %d in station %d already occupied by car %s\n", //debug print
            portNum, stationID, port->p2car->nLicense);
        return false;
    }

    if (car->pPort != NULL) //checks if car is not already is some other port
    {
        printf("Error: Car %s is already assigned to another port. Skipping.\n", car->nLicense); //debug print
        return false;
    }

    printf("LoadCar2port: assigning car %s to port #%d at station %d\n", car->nLicense, portNum, stationID); //debug print

    // Assign car to the port and mark port occupied
    port->p2car = car; 
    car->pPort = port; 
    port->status = 1; // occupied
    car->inqueue = 0; // 0 = charning/not in line

    return true;
}

Port* findFreePortOfType(Port* head, PortType type)
{
    Port* current = head;
    while (current) //traveres through ports list
    {
        //printf("    Port %d: type=%d, status=%d\n", current->num, current->portType, current->status); //debug print
        if (current->status == FREE && current->portType == type && current->p2car == NULL)
        {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

Date getCurrentTime()
{
    Date current;
    time_t now = time(NULL); //return the current system time in seconds
    struct tm* local = localtime(&now); //splits "now" variable into day,month,year,hour,minute...
    
    if (local)
    {
        current.day = local->tm_mday;
        current.month = local->tm_mon + 1; //+1 to get months 1 - 12
        current.year = local->tm_year + 1900; //+1900 to get yaers since 1900
        current.hour = local->tm_hour;
        current.min = local->tm_min;
    }
    else
    {
        current.day = 0;
        current.month = 0;
        current.hour = 0;
        current.year = 0;
        current.min = 0;
    }

    return current; 
}

bool assignCar2port(Port* targetPort, Car* car)
{       //check for NULL pointers.
        if (!targetPort || !car) 
        {
            displayError(ERR_NULL_POINTER);
            return false;
        }

        if (targetPort->status == OCCUPIED)
        {
            printf("Port is already occupied.\n");
            return false;
        }

        targetPort->p2car = car; //assign car to port

        targetPort->status = OCCUPIED;  //update status field

        targetPort->tin = getCurrentTime(); //get current charging time

        car->pPort = targetPort; //link between car and port
         
        car->inqueue = 0; //set car to charge

        return true;
 }

void printPortList(Port* portsList)
{   //check for NULL pointers
    if (!portsList)
    {
        displayError(ERR_PORTLIST_EMPTY);
        return;
    }

    Port* current = portsList;
    const char* type;
    int occupiedPortCount = 0;
    int activePortCount = 0;

    while (current) //traverse through ports list
    {
       type = portTypeToStr(current->portType);
        printf("    Port %d - Type:  %s", current->num, type);
        printf(" - Status: ");
        switch (current->status)
        {
            
        case OCCUPIED: 
          {
            printf("Occupied");
            occupiedPortCount++;
            activePortCount++;
            break;
          }
        case FREE: 
        {
            printf("Free");
            activePortCount++;
            break;
        }
        case OUT_OF_ORDER: 
        {
            printf("Out of order");
            break;
        }
        }
        printf("\n");
        current = current->next;
    }
    printf("Number of active ports: %d\n", activePortCount);
    printf("Number of occupied ports: %d\n", occupiedPortCount);
}

void printChargingCars(Port* portList)
{   //check for NULL pointer
    if (!portList)
    {
        displayError(ERR_PORTLIST_EMPTY);
        return;
    }
    bool found = false;
    Date now; //gets current time
    int minutesNow; //gets current time as minutes
    int minutesStart; //converts stating charge time to minutes
    int duration;

    while (portList) //travese through port list
    {
        if (portList->status == 1 && portList->p2car) 
        { // 1 = occupied
            now = getCurrentTime();
            minutesNow = dateToMinutes(now);
            minutesStart = dateToMinutes(portList->tin);
            duration = minutesNow - minutesStart;

            printf("  Charging Car: %s at Port #%d, Type: %s,",
                portList->p2car->nLicense,
                portList->num,
                portTypeToStr(portList->portType));
            printf("  Charging time: %d [Minutes]\n", duration);
            found = true;
        }
        portList = portList->next;
    }
    if (!found)
    {
        printf("There are no cars charging.\n");
        return;
    }
}

void freePortList(Port* portList)
{   //check for NULL pointer
    if (!portList)
    {
        displayError(ERR_PORTLIST_EMPTY);
        return;
    }

    Port* current = portList;

    while (current) //traverse through port list
    {
        Port* temp = current;
        current = current->next;

        temp->p2car = NULL;

        free(temp);
    }
}

static void writePortsFromStation(FILE* file, Station* station)
{   //check for NULL pointers
    if (!station || !file)
    {
        displayError(ERR_NULL_POINTER);
        return;
    }
    Port* port = station->portsList;

    while (port) //traverse through port list
    {
        const char* licensePlate = "-1"; //defualt
        if (port->p2car)
        {
            licensePlate = port->p2car->nLicense;
        }
        //StationID,PortNumber,PortType,Status,Year,Month,Day,Hour,Min,CarLicense -- format
        fprintf(file, "%d,%d,%s,%d,%d,%d,%d,%d,%d,%s\n",
            station->id, port->num, portTypeToStr(port->portType), port->status, port->tin.year, port->tin.month, port->tin.day,
            port->tin.hour, port->tin.min,licensePlate);

        port = port->next;
    }
}

static void writeAllPorts(FILE* file, Station* root)
{
    //check for NULL pointer
    if (!root)
    {
        return;
    }
    //traverse through station BST recursively
    writeAllPorts(file, root->left);
    writePortsFromStation(file, root); //write ports informaion to file
    writeAllPorts(file, root->right);
}

void updatePortFiles(stationBST_M* stationManager, const char* filename)
{
    //check for NULL pointers
    if (!stationManager || !filename)
    {
        displayError(ERR_NULL_POINTER);
        return;
    }

    FILE* file = fopen(filename, "w");
    if (!file)
    {
        displayError(ERR_OPENING_FILE);
        return;
    }

    fprintf(file, "StationID,PortNumber,PortType,Status,Year,Month,Day,Hour,Min,CarLicense\n");

    writeAllPorts(file, stationManager->root); //recursively traverse through station BST and access each station`s portslist
    fclose(file);
    printf("Port file %s updated successfully.\n", filename);
}