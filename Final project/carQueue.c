#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "carQueue.h"
#include "stations.h"
#include "errors.h"


void initCarQueue(qCar* queue) 
{   //inits qCar fields with NULLs
    if (queue) 
    {
        queue->front = NULL;
        queue->rear = NULL;
    }
}

bool enqueueCar(Station* station, Car* car) 
{   //check for NULL pointers
    if (!station || !car || !station->carQueue) 
        return false;
    //allocate memory for the new car node 
    carNode* newNode = (carNode*)malloc(sizeof(carNode));
    if (!newNode) 
    {
        displayError(ERR_MEMORY_ALLOCATION);
        return false;
    }

    //init the new car node
    newNode->p2car = car;
    newNode->next = NULL;

    qCar* queue = station->carQueue; //get the station's car queue

    //add the new node to the end of the queue.
    if (queue->rear)
    {
        //if queue is not empty - link the newNode to last node.
        queue->rear->next = newNode;
    }
    else 
    {
        //queue is empty
        queue->front = newNode;
    }

    queue->rear = newNode;
    car->inqueue = 1; // 1 = is in queue
    car->pPort = NULL; //while in queue car is not assigned to a port

    //printf("Before increment: %d\n", station->nCars); //debug print
    station->nCars++; //increment the number of cars in the station
    //printf("After increment: %d\n", station->nCars); //debug print

    return true;
}

void printQueuedCars(qCar* queue) 
{
    //check for NULL pointer.
    if (!queue) 
        return;

    //checks if car queue is empty.
    if (!queue->front)
    {
        displayError(ERR_CAR_QUEUE_EMPTY);
        return;
    }

    int place = 1; //counter for car position in line
    carNode* current = queue->front;

    printf("\nCars in line:\n");
    while (current) //move through the queue and print license of each car. 
    {
        if (current->p2car)
            printf("  %d)  %s\n",place, current->p2car->nLicense);

        place++;
        current = current->next;
    }
}


bool assignNextCompatibleCarFromQueue(Station* station, Port* port)
{
    // check NULL pointers.
    if (!station)
    {
       displayError(ERR_STATION_NULL);
       return false;
    }
    if (!station->carQueue)
    {
        displayError(ERR_CAR_QUEUE_NULL);
        return false;
    }
    //check if queue is empty
    if (!station->carQueue->front)
    {
        return false;
    }
    //check if port pointer is valid
    if (!port)
    {
        displayError(ERR_INVALID_PORT_TYPE);
        return false;
    }

    //start searching compatible car in car queue
    qCar* queue = station->carQueue;
    carNode* current = queue->front;
    carNode* prev = NULL;

    while (current) //move through car queue
    {
        //check for port type compatibility
        if (current->p2car && current->p2car->portType == port->portType)
        {
            Car* nextCar = current->p2car;

            //remove node from the queue
            if (prev) 
                prev->next = current->next;
            else 
                queue->front = current->next; //move front is first node is being removed.

            if (current == queue->rear)
                queue->rear = prev; //update rear if last node is removed

            free(current); //free memory of removed node

            station->nCars--; 
            nextCar->inqueue = 0; // 0 = no longer is queue

            //try to assign the car to the port
            if (assignCar2port(port, nextCar))
            {
                printf("  Car %s is now charging at port #%d.\n", nextCar->nLicense, port->num);
                
                return true;
            }
            else
            {
                printf("Failed to assign car %s to port #%d.\n", nextCar->nLicense, port->num);
                return false;
            }
        }
        //move to the next node.
        prev = current;
        current = current->next;
    }
    printf("No compatible car in queue for port type. port #%d is free\n", port->num);
    return false;
}

void freeCarQueue(qCar* queue)
{
    //check if queue is empty
    if (!queue)
    {
        displayError(ERR_CAR_QUEUE_EMPTY);
        return;
    }

    //start from front of queue
    carNode* current = queue->front;
    while (current) // traverse and free each node in the queue
    {
        carNode* temp = current; //saves current node
        current = current->next; //move to next node
        free(temp); //free current node
    }

    free(queue); // at the end - free queue struct itself.
}

void writeQueueToFile(Station* station, FILE* file)
{
    //check for NULL pointers
    if (!station || !file)
        return;

    // in order BST traversal - left subtree
    writeQueueToFile(station->left, file);

    if (station->carQueue) //if current station has a queue, move through it and print each car
    {
        carNode* current = station->carQueue->front;

        while (current)
        {
            if (current->p2car)
            {
                //write in the format: License-plate,station ID
                fprintf(file, "%s,%d\n", current->p2car->nLicense, station->id);
            }
            current = current->next;
        }
    }
    //in order traversal - right subtree
    writeQueueToFile(station->right, file);
}


void updateCarQueueFile(stationBST_M* stationManager, const char* filename)
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
    // Write txt headline
    fprintf(file, "License,StationID\n");

    //write the queue from all stations into the file
    writeQueueToFile(stationManager->root, file);

    fclose(file);
    printf("File update for %s has completed.\n", filename);
}