#include <stdio.h>
#include "errors.h"


void displayError(ErrorCode code)
{	//print error according to given code
	switch (code)
	{
	case ERR_INVALID_INPUT:
	{
		printf("Error: Invalid input.\n");
		break;
	}
	case ERR_CAR_NOT_FOUND:
	{
		printf("Error: Car not found.\n");
		break;
	}
	case ERR_DUPLICATE_STATION_ID:
	{
		printf("Error: Station ID already exists.\n");
	}
	case ERR_STATION_NOT_FOUND:
	{
		printf("Error: Station not found\n");
		break;
	}
	case ERR_MEMORY_ALLOCATION:
	{
		printf("Error: Memory allocation failed.\n");
		break;
	}
	case ERR_NO_AVAILABLE_PORT:
	{
		printf("No available charging ports available at the station.\n");
		break;
	}
	case ERR_NULL_POINTER:
	{
		printf("Error: NULL pointer encountered.\n");
		break;
	}
	case ERR_INVALID_PORT_TYPE:
	{
		printf("Invalid port type.\n");
		break;
	}
	case ERR_INPUT_NOT_NUM:
	{
		printf("Invalid input, please enter a number.\n");
		break;
	}
	case ERR_PORTLIST_EMPTY:
	{
		printf("Couldnt find ports.\n");
		break;
	}
	case ERR_CAR_QUEUE_NULL:
	{
		printf("Error: Car queue is NULL.\n");
		break;
	}
	case ERR_STATION_NULL:
	{
		printf("Error: Station pointer is NULL.\n");
		break;
	}
	case ERR_CAR_QUEUE_EMPTY:
	{
		printf("Car queue is empty.\n");
		break;
	}
	case ERR_INVALID_SEARCH_TYPE:
	{
		printf("Invalid search type.\n");
		break;
	}
	case ERR_INVALID_INPUT_FORMAT:
	{
		printf("Invalid input format.\n");
		break;
	}
	case ERR_INVALID_BUFFER_SIZE:
	{
		printf("Error: Size is invalid.\n");
		break;
	}
	case ERR_INVALID_LICENSE_LENGTH:
	{
		printf("License plate must be exactly 8 characters.\n");
		break;
	}
	case ERR_STATION_BST_EMPTY:
	{
		printf("Error: No stations exist in the system.\n");
		break;
	}
	case ERR_CAR_BST_EMPTY:
	{
		printf("Error: No cars exist in the system\n");
		break;
	}
	default:
	{
		printf("Error: An unknown error occured.\n");
		break;
	}
	}
	
}