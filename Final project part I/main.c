#define _CRT_SECURE_NO_WARNINGS
#include "car.h"
#include "carBST.h"
#include "FileLoader.h"
#include "menuFuncs.h"
#include "stations.h"
#include <stdio.h>



int main()
{
    stationBST_M stationManager = makeEmptyStationTree();
    car_BST carManager = makeEmptyCarBST();
    loadFiles(&stationManager, &carManager);

    int choice;
    clearScreen();
    printf("---Welcome to EasyCharge Menu!---\n\n");
    do 
    {
        printMenu();
        
        scanf(" %d", &choice);
        
        while (getchar() != '\n');

        menuFuncs(choice, &stationManager, &carManager);

    } while (choice != 0);

	return 0;
}