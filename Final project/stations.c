#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "stations.h"
#include <float.h>
#include <math.h>
#include "menuFuncs.h"
#include "errors.h"


// Defines a function pointer type for search station functions.
typedef Station* (*SearchFuncs)(const stationBST_M* manager, void* data);


stationBST_M makeEmptyStationTree()
{ //inits station tree manager with NULL and 0 as tree size.
    stationBST_M tree = { .root = NULL, .size = 0 };
    return tree;
}

//Helper function to insert station into BST.
static Station* insertStationRec(Station* root, Station* newStation, bool* inserted) 
{
    //if root == null - the BST is empty. so newStation will be first node of BST
    if (root == NULL) 
    {
        *inserted = true;
        return newStation;
    }
    if (newStation->id < root->id)
    {
        //if newStation id is less than root`s id it will go to left side of BST
        root->left = insertStationRec(root->left, newStation, inserted);
    }
    else if (newStation->id > root->id) 
    {   
        //if neStation id is greater than root`s id it will go to right side of BST
        root->right = insertStationRec(root->right, newStation, inserted);
    }
    else 
    {
        *inserted = false; // Duplicate ID
    }
    return root;
}

bool insertStation(stationBST_M* manager, Station* newStation) 
{
    //Check NULL pointers
    if (!manager || !newStation)
    {
     displayError(ERR_NULL_POINTER);
     return false; //checks if manager / newStation is invalid
    }
    
    bool inserted = false;
    manager->root = insertStationRec(manager->root, newStation, &inserted);
    if (inserted) //if insertion successful "inserted" will be True
        manager->size++;
    return inserted;
}


Station* findStationByID(const stationBST_M* manager, int stationID) 
{
    Station* current = NULL;
    //check if the station manager is valid
    if (manager != NULL) 
    {
        current = manager->root; //start the root of the bst
    }
    //traverse the BST
    while (current) 
    {
        if (stationID < current->id) 
        {
            //move to left subtree if id is smaller
            current = current->left;
        }
        else if (stationID > current->id) 
        {
            //move to right subtree if id is greater
            current = current->right;
        }
        else 
        {
            //found the station with matching ID
            return current;
        }
    }
    //return NULL if station not found
    return NULL;
}

 //Helper recursive function
static Station* findStationByNameRec(Station* root, const char* stationName)
{
    //check NULL pointer
    if (!root || !stationName) 
    {
        return NULL;
    }
    //If current station name matches stationName, return root.
    if (strcmp(root->name, stationName) == 0)
        return root;
    
    // recursively search the left subtree
    Station* leftResult = findStationByNameRec(root->left, stationName);
    if (leftResult) 
        return leftResult;
    // if not found in left subtree -> search in right subtree.
    return findStationByNameRec(root->right, stationName);
}

Station* findStationByName(const stationBST_M* manager, const char* stationName)
{
    //Check NULL pointers
    if (!manager || !stationName) 
    {
       // displayError(ERR_NULL_POINTER);
        return NULL;
    }
    // call recursive helper function
    return findStationByNameRec(manager->root, stationName);
}



void printAllAssignedCarsTest(stationBST_M* manager) 
{
    //check for NULL pointers
    if (!manager || !manager->root)
    {
       // displayError(ERR_NULL_POINTER);
        return; 
    }

    //call recursive helper function
    printAssignedCarsInStationTest(manager->root);
}


void printAssignedCarsInStationTest(Station* station) //tester function to check loaded files
{
    //check NULL pointer - base case
    if (!station) 
    {
        return;
    }

    printf("Station ID: %d\n", station->id);

    Port* currentPort = station->portsList; 
    while (currentPort) //if currenPort != NULL proceed...
    {
        if (currentPort->p2car != NULL) 
        {
            // (PortType) casting to avoid warnings
            const char* portTypeStr = "UNKNOWN";
            switch (currentPort->portType) 
            {
            case (PortType)FAST: 
                portTypeStr = "FAST"; 
                break;
            case (PortType)MID:
                portTypeStr = "MID"; 
                break;
            case (PortType)SLOW:
                portTypeStr = "SLOW"; 
                break;
            }

            printf("  Port %d [%s] => Car %s\n",
                currentPort->num,
                portTypeStr,
                currentPort->p2car->nLicense);
        }
        currentPort = currentPort->next;
    }

    // traverse through station tree recursively.
    printAssignedCarsInStationTest(station->left);
    printAssignedCarsInStationTest(station->right);
}

double calcDistance(Coordinates userCoords, Coordinates stationCoords)
{
    return sqrt((userCoords.x - stationCoords.x) * (userCoords.x - stationCoords.x) + (userCoords.y - stationCoords.y) * (userCoords.y - stationCoords.y));
}

static void findNearestStationHelper(Station* root, Coordinates userCoord, Station** nearest, double* minDist)
{
    //check for NULL pointer - base case
    if (!root) 
    {
        return;
    }
    //calculate distance from user`s coordinates to station coordinates
    double dist = calcDistance(userCoord, root->coord);
    //if current station is closer than previous station, update the result.
    if (dist < *minDist) 
    {
        *minDist = dist;
        *nearest = root;
    }

    // traverse through left and right side of the station BST
    findNearestStationHelper(root->left, userCoord, nearest, minDist);
    findNearestStationHelper(root->right, userCoord, nearest, minDist);
}


Station* findNearestStation(stationBST_M* manager, double userX, double userY)
{
    //check for NULL pointers
    if (!manager || !manager->root)
    {
        displayError(ERR_NULL_POINTER);
        return NULL;
    }
    // create a coordinate struct from user`s input
    Coordinates userCoord = { userX, userY };
    Station* nearest = NULL;
    double minDist = DBL_MAX;
    // find nearest station recursively
    findNearestStationHelper(manager->root, userCoord, &nearest, &minDist);

    return nearest;
}



// helper function for searchStation func
static Station* findByIDAdapter(stationBST_M* stationManager, void* data) 
{
    //check NULL pointers
    if (!stationManager || !data)
    {
        displayError(ERR_NULL_POINTER);
        return NULL;
    }

    int* id = (int*)data; //converts param data to int (ID)
    return findStationByID(stationManager, *id);
}

// helper function for searchStation func
static Station* findByNameAdapter(stationBST_M* stationManager, void* data)
{
    //check NULL pointers
    if (!stationManager || !data)
    {
        displayError(ERR_NULL_POINTER);
        return NULL;
    }

    char* name = (char*)data; //converts param data to char (Name)
    return findStationByName(stationManager, name);
}

// helper function for searchStation func
static Station* findByCoordsAdapter(stationBST_M* stationManager, void* data)
{
    //check NULL pointers
    if (!stationManager || !data)
    {
        displayError(ERR_NULL_POINTER);
        return NULL;
    }

    Coordinates* coords = (Coordinates*)data; //converts data param into Coordinates
    return findNearestStation(stationManager,coords->x,coords->y);
}

Station* searchStation(stationBST_M* stationManager, int searchType, void* data) 
{
    //check NULL pointer
    if (!stationManager) 
    {
        displayError(ERR_NULL_POINTER);
        return NULL;
    }

    // array of search station adapters indexed by searchType param
    SearchFuncs searchFuncs[] = 
    {
        findByIDAdapter,
        findByNameAdapter,
        findByCoordsAdapter
    };

    // validate search type
    if (searchType < 0 || searchType > 2)
    {
        displayError(ERR_INVALID_SEARCH_TYPE);
        return NULL;
    }

    //call the right search function based on searchType
    return searchFuncs[searchType](stationManager, data);
}

Station* getStationByUserInput(stationBST_M* stationManager)
{
    int choice = 0;
    Station* station = NULL;

    //asks for user`s input until valid input is provided
    while (choice != 1 && choice != 2)
    {
        printf("Please enter how would you like to search for a station (1 - ID | 2 - Name): ");
        if (scanf("%d", &choice) != 1)
        {   //handle none integer input
            displayError(ERR_INVALID_INPUT);
            while (getchar() != '\n'); //clear input buffer
            choice = 0; //reset choice
        }
        else if (choice != 1 && choice != 2)
        {
            //incase of invalid choice value
            displayError(ERR_INVALID_INPUT);
        }
    }
    // --search stations by ID --
    if (choice == 1)
    {
        int stationID;
        //keep asking for ID until a station is found
        while (!station)
        {
            printf("Please enter station's ID: ");
            if (scanf("%d", &stationID) != 1)
            {
                displayError(ERR_INVALID_INPUT);
                while (getchar() != '\n'); //clear input buffer
                continue;
            }
            printf("\n");
            //call function to search by ID
            station = searchStation(stationManager, SEARCH_BY_ID, &stationID);
            if (!station)
            {
                displayError(ERR_STATION_NOT_FOUND);
            }
        }
    }
    // -- search station by name --
    else 
    {
        char stationName[STATION_NAME_SIZE];
        // keep asking for a valid name until a station is found
        while (!station)
        {
            stationNameInput(stationName, STATION_NAME_SIZE); //get station name by user input
            //search station by name
            station = searchStation(stationManager, SEARCH_BY_NAME, stationName);
            if (!station)
            {
                displayError(ERR_STATION_NOT_FOUND);
            }
        }
    }
    return station;
}

void printStation(Station* station)
{
    //check for NULL pointer
    if (!station)
    {
        displayError(ERR_NULL_POINTER);
        return;
    }
    //print station information
    printf("\n--- Station Info ---\n");
    printf("Station Name: %s\n", station->name);
    printf("Station ID: %d\n", station->id);
    printf("Coordinates: (%.2f, %.2f)\n", station->coord.x, station->coord.y);
    printf("Ports: \n");
    printPortList(station->portsList);
    printf("Queue length: %d\n", station->nCars);
}

//print station BST recursively
static void printStationTreeRec(Station* root) 
{
    //check NULL pointer
    if (!root) 
    {
        return;
    }
    //traverse recursively
    printStationTreeRec(root->left);
    printStation(root);
    printf("\n-------------------------------\n");
    printStationTreeRec(root->right);
}

void printStationTree(stationBST_M* stationManager)
{
    //check NULL pointers
    if (!stationManager || !stationManager->root)
    {
        displayError(ERR_NULL_POINTER);
        return;
    }
    //call recursive traversal function
    printStationTreeRec(stationManager->root);
}


int dateToMinutes(Date date)
{ 
    return date.year * 525600 + date.month * 43800 + date.day * 1440 + date.hour * 60 + date.min;
}

//recursively free station BST
static void freeStationTree(Station* root) 
{
    //check for NULL pointer
    if (!root)
    {
        return;
    }

    // go through all sides of the tree
    freeStationTree(root->left); 
    freeStationTree(root->right); 

    freePortList(root->portsList); //free station`s port list

    freeCarQueue(root->carQueue); //free station`s car queue

    free(root->name); //frees allocated name

    free(root); //free the station node
}


void destroyStationTree(stationBST_M* stationManager)
{
    //check for NULL pointer
    if (!stationManager)
    {
        return;
    }

    freeStationTree(stationManager->root);

    stationManager->root = NULL;
}

/**
* Finds the station with the minimum value (left most node) in the BST.
* @param station: Root of the subtree to search at
* @return Pointer to the left most station node.
*/
Station* minValueNode(Station* station)
{
    Station* current = station;

    // Traverse left and find the smallest value
    while (current && current->left)
        current = current->left;

    return current;
}

/**
* Swaps all fields between two Station structs.
* Used when deleting a node from station BST. 
* Replaces the node to be deleted with its in order child.
* 
* @param stationToDelete: Pointer to the station being deleted.
* @param child: Pointer to the station replacing it.
*/
static void swapStations(Station* stationToDelete, Station* child)
{   //Stores all fields of stationToDelete in temp variables.
    int tempId = stationToDelete->id;
    char* tempName = stationToDelete->name;
    int tempNPorts = stationToDelete->nPorts;
    Coordinates tempCoords = stationToDelete->coord;
    Port* tempPortsList = stationToDelete->portsList;
    int tempNCars = stationToDelete->nCars;
    qCar* tempCarQueue = stationToDelete->carQueue;

    //copy all fields from child to stationToDelete
    stationToDelete->id = child->id;
    stationToDelete->name = child->name;
    stationToDelete->nPorts = child->nPorts;
    stationToDelete->coord = child->coord;
    stationToDelete->portsList = child->portsList;
    stationToDelete->nCars = child->nCars;
    stationToDelete->carQueue = child->carQueue;
    
    //copy previously saved values from StationToDelete into child.
    child->id = tempId;
    child->name = tempName;
    child->carQueue = tempCarQueue;
    child->coord = tempCoords;
    child->nCars = tempNCars;
    child->portsList = tempPortsList;
    child->nPorts = tempNPorts;
}

/**
* Recursively search in station BST for station based on stationID.
* 
* @param root: Pointer to the root of current subtree
* @param stationID: ID of the station to delete
* @param deleted: Pointer to a boolean flag that will be set to True if node was deleted.
* @return Updated root of the subtree after deletion
*/
Station* deleteStationNode(Station* root, int stationID, bool* deleted)
{
    //check for NULL pointer
    if (!root)
    {
        displayError(ERR_NULL_POINTER);
        return NULL;
    }

    // recurse to the left if stationID is smaller
    if (stationID < root->id)
    {
        root->left = deleteStationNode(root->left, stationID, deleted);
    }
    //recurse to the right if stationID is greater
    else if (stationID > root->id)
    {
        root->right = deleteStationNode(root->right, stationID, deleted);
    }
    else //if station is found
    {
        *deleted = true;
        //free all allocated memory 
        freePortList(root->portsList); 
        freeCarQueue(root->carQueue);
        free(root->name);

        if (!root->left && !root->right) // if no kids 
        {
            free(root);
            return NULL;
        }

        if (!root->left || !root->right) //if one kids
        {
            Station* child;
            if (root->left)
            {
                child = root->left;
            }
            else 
            {
                child = root->right;
            }

            free(root);
            return child;
        }
        //find child and replace its info
        Station* current = minValueNode(root->right); 

        swapStations(root, current); //swap the fields between the two stations
        root->right = deleteStationNode(root->right, current->id, deleted);
    }

    return root;
}


bool deleteStation(stationBST_M* stationManager, int stationID)
{
    //Check for NULL pointers
    if (!stationManager || !stationManager->root)
    {
        displayError(ERR_NULL_POINTER);
        return false;
    }

    bool deleted = false; //boolean variable that will be set to True when station is deleted.
    stationManager->root = deleteStationNode(stationManager->root,stationID, &deleted);

    if (deleted) //if deletion completed, decreament size of the tree.
    {
        stationManager->size--;
        //printf("Current station BST size: %d\n", stationManager->size); //debug print 
    }
    return deleted;
}

//Helper for updating station file function
static void writeToStationTXT(FILE* file, Station* root) 
{
    //check for base case - root = NULL
    if (!root)
    {
        return;
    }
    //move recursively - left subtree
    writeToStationTXT(file, root->left);
    //ID,StationName,NumOfPorts,CoordX,CoordY   -- format
    fprintf(file, "%d,%s,%d,%.2f,%.2f\n", root->id, root->name, root->nPorts, root->coord.x, root->coord.y);

    //move recursively - right subtree
    writeToStationTXT(file, root->right);
}

void updateStationFiles(stationBST_M* stationManager, const char* filename)
{
    //Check for NULL pointers
    if (!stationManager || !filename)
    {
        displayError(ERR_NULL_POINTER);
        return;
    }
    FILE* file = fopen(filename, "w");
    if (!file) //if opening the file failed
    {
        displayError(ERR_OPENING_FILE);
        return;
    }
    fprintf(file, "ID,StationName,NumOfPorts,CoordX,CoordY\n"); //print file header

    writeToStationTXT(file, stationManager->root); //start recursively print the BST to txt

    fclose(file);
    printf("Station file %s updated successfully.\n", filename); //debug print
}


Station* findStationWithPort(Station* station, Port* targetPort) //helper function
{
    //Check for NULL pointers
    if (!station || !targetPort)
    {
        return NULL;
    }

    Port* current = station->portsList;
    while (current) //search current station port list for targetPort
    {
        if (current == targetPort)
            return station; //port found in current station
        current = current->next;
    }
    //recursively search in the left subtree
    Station* stationBSTleft = findStationWithPort(station->left, targetPort);

    if (stationBSTleft) // if the station is found in left side of BST return station.
        return stationBSTleft;

    //recursively search in the right subtree (if not found in left)
    return findStationWithPort(station->right, targetPort);
}

Station* findStationByCarInQueue(Station* root, Car* car) //helper function
{
    //check for NULL pointers and base case
    if (!root || !car)
    {
        return NULL;
    }

    if (!root->carQueue) //check if the station has a car queue
        return NULL;

    carNode* current = root->carQueue->front;
    while (current) //search through the stations car queue.
    {
        if (current->p2car == car) //if car is found in this station's queue
            return root;
        current = current->next;
    }
    //recursively search through the left subtree
    Station* stationBSTleft = findStationByCarInQueue(root->left, car); 
    if (stationBSTleft)
        return stationBSTleft;
    //recurs through the right subtree
    return findStationByCarInQueue(root->right, car); //search right side of BST
}