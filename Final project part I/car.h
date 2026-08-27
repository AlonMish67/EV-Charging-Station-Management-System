
#ifndef CAR_H
#define CAR_H

//#include "Ports_list.h" //causes a LOOP!


typedef enum PortType PortType;
typedef struct Station Station;
typedef struct Port Port;
typedef struct Car Car;

typedef struct Car
{
	char nLicense[10];
	PortType portType;
	double totalPayed;
	Port* pPort; // pointer to the port it's charging at ,if not charging it points to NULL
	int inqueue; // not in line/charging - 0 ,waiting in line - 1
} Car;

#endif

