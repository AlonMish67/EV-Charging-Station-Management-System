#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "carBST.h"
#include "stations.h"
#include "errors.h"
#define LICENSE_SIZE 10




static tCar* deleteCarNode(tCar* root, const char* licensePlate, bool* deleted)
{
    //check for NULL pointer
    if (!root)
    {
        displayError(ERR_NULL_POINTER);
        return NULL;
    }
    //Compare the license plate with current node's plate.
    int cmp = strcmp(licensePlate, root->p2car->nLicense);
    if (cmp < 0)
    {
        // target is in the left side of tree
        root->left = deleteCarNode(root->left, licensePlate, deleted);
    }
    else if (cmp > 0)
    {
        // target is in the right side of tree
        root->right = deleteCarNode(root->right, licensePlate, deleted);
    }
    else
    {
        // found carNode to delete
        *deleted = true;

        if (!root->left && !root->right) // Node has no children
        {
            free(root->p2car); //free car object
            free(root); //free node
            return NULL; // returns null to parent
        }

        if (!root->left || !root->right) // Node has 1 child
        {
            tCar* child;
            if (root->left)
            {
                child = root->left;
            }
            else
            {
                child = root->right;
            }

            free(root->p2car); //free car object
            free(root); //free node

            return child; //return child to parent to not ruin the tree order
        }

        tCar* current = root->right; // if has 2 kids
        while (current && current->left != NULL) // find in order child (most left child in right side of tree)
            current = current->left;

        // swap car data between root and in order child
        Car* temp = root->p2car;
        root->p2car = current->p2car;
        current->p2car = temp;

        //Recursively delete the in order child ( will hold now the old car)
        root->right = deleteCarNode(root->right, current->p2car->nLicense, deleted);
    }
    return root; // return the root of the subtree
}

bool deleteCar(car_BST* carManager, const char* licensePlate)
{
    // check for NULL pointers
    if (!carManager || !carManager->root || !licensePlate)
    {
        displayError(ERR_NULL_POINTER);
        return false;
    }

    // Recursive deletetion, updating "deleted" with True if successful.
    bool deleted = false;
    carManager->root = deleteCarNode(carManager->root, licensePlate, &deleted);

    return deleted;
}

static  tCar* insertCarNode(tCar* root, Car* newCar, bool* inserted) //alphabetical order of license plate strings
{
    //check for NULL pointer
    if (!newCar)
    {
        *inserted = false;
        return root;
    }

    //if root empty - tree/subtree is empty. - insert here newNode
    if (!root) 
    {
        tCar* newNode = (tCar*)malloc(sizeof(tCar));
        if (!newNode) 
        {
            displayError(ERR_MEMORY_ALLOCATION);
            *inserted = false;
            return NULL; // malloc failed
        }
        //inits new node.
        newNode->p2car = newCar;
        newNode->left = newNode->right = NULL;
        *inserted = true;
        return newNode;
    }

    //ensure license plats are valid
    if (!root->p2car || !root->p2car->nLicense || !newCar->nLicense) 
    {
        *inserted = false;
        return root;
    }
    //compare license plate to maintain sorted BST.
    int cmp = strcmp(newCar->nLicense, root->p2car->nLicense);
    if (cmp < 0)
        //go left
        root->left = insertCarNode(root->left, newCar, inserted);
    else if (cmp > 0)
        //go right
        root->right = insertCarNode(root->right, newCar, inserted);
    else
    {
        *inserted = false;  // duplicate license, no insertion
    }
    //return current root.
    return root;
}


static Car* searchCarNode(tCar* root, const char* licensePlate)
 {
    //empty tree / subtree
    if (!root) 
    {
        return NULL;
    }

    //check for NULL pointer
    if (!root->p2car)
    {
        displayError(ERR_NULL_POINTER);
        return NULL;
    }

    //compare the current node's license plate with the targer.
    int cmp = strcmp(licensePlate, root->p2car->nLicense);
    if (cmp == 0) // match found
        return root->p2car;
    if (cmp < 0) // target license is smaller - go left
        return searchCarNode(root->left, licensePlate);
    return searchCarNode(root->right, licensePlate); // target license is bigger - go right (alphabetically)
}



car_BST makeEmptyCarBST() 
{// inits root with NULL and count = 0 initialy
    car_BST tree = { .root = NULL, .count = 0 };
    return tree;
}


bool insertCar(car_BST* manager, Car* newCar) 
{   // check for NULL pointers.
    if (!manager || !newCar) 
    {
        displayError(ERR_NULL_POINTER);
        return false;
    }
    // Recursive insert, updating "inserted" to True if successful.
    bool inserted = false;
    manager->root = insertCarNode(manager->root, newCar, &inserted);

    if (inserted) //If a new node was inserted, increment the count of Cars in the BST.
        manager->count++;
    return inserted;
}


Car* searchCar(const car_BST* manager, const char* licensePlate) 
{
    //check for NULL pointers.
    if (!manager || !licensePlate)
    {
        //printf("searchCar: Invalid input\n");
        displayError(ERR_INVALID_INPUT);
        return NULL;
    }
    //Recursive search in car BST.
    return searchCarNode(manager->root, licensePlate);
}


Car* createCar(const char* licensePlate, PortType portType)
{
    //check for NULL pointers.
    if (!licensePlate)
    {
        displayError(ERR_NULL_POINTER);
        return NULL;
    }

    //allocate memory for new car
    Car* car = (Car*)malloc(sizeof(Car));
    if (!car) 
    {
        //printf("Memory allocation failed for new car.\n");
        displayError(ERR_MEMORY_ALLOCATION);
        return NULL;
    }

    // Copy license plate safely
    strcpy(car->nLicense, licensePlate);
    car->nLicense[LICENSE_SIZE - 1] = '\0'; // ensure null termination

    car->portType = portType;
    car->inqueue = 1;
    car->pPort = NULL;
    car->totalPayed = 0;

    return car;
}


static void freeCarTree(tCar* root)
{
    // check for NULL pointer
    if (!root)
    {
        //displayError(ERR_NULL_POINTER);
        return;
    }

    // first go left side than right side
    freeCarTree(root->left);
    freeCarTree(root->right);

    // at the end free car and car-node
    free(root->p2car);
    free(root);
}

void destroyCarTree(car_BST* carManager)
{
    // check for NULL pointer
    if (!carManager)
    {
        return;
    }

    //Recursively free car BST.
    freeCarTree(carManager->root);

    carManager->root = NULL;
}

static void writeCarsToFile(FILE* file, tCar* root, stationBST_M* stationManager)
{
    //got to end of subtree
    if (!root)
        return;
    //Recursively write Car data into file
    writeCarsToFile(file, root->left, stationManager);

    const char* license = root->p2car->nLicense;
    const char* portTypeStr = portTypeToStr(root->p2car->portType); //used to convert port type into string
    double totalPayed = root->p2car->totalPayed;

    int stationID = 0; //default
    int portNum = 0; //default
    Port* port = NULL;
    Station* station = NULL;
    //checks if car is connected to a port
    if (root->p2car->pPort != NULL)
    {
        port = root->p2car->pPort;
        station = findStationWithPort(stationManager->root, port);
        if(station)
            stationID = station->id;

        portNum = root->p2car->pPort->num;
    }
    else if (root->p2car->inqueue == 1) //checks if car is in queue
    {
        station = findStationByCarInQueue(stationManager->root, root->p2car);
        if (station)
            stationID = station->id;
    }
    
    int inQueue = root->p2car->inqueue;

    //Format: License,PortType,TotalPayed,StationID,PortNumber,InQueue
    fprintf(file, "%s,%s,%.2lf,%d,%d,%d\n", license,portTypeStr,totalPayed,stationID,portNum,inQueue);

    //Recursively write Car data into file
    writeCarsToFile(file, root->right, stationManager);
}


void updateCarFiles(car_BST* carManager,stationBST_M* stationManager, const char* filename)
{
    //check for NULL pointers.
    if (!carManager || !filename)
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
    
    fprintf(file, "License,PortType,TotalPayed,StationID,PortNumber,InQueue\n"); //print format head line to txt
    
    //call recursive function to write car data to file
    writeCarsToFile(file, carManager->root, stationManager);

    fclose(file);
    printf("Cars file updated successfully.\n");
}