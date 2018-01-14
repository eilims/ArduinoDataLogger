#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <windows.h>
#include <winbase.h>

//Variables
DWORD READ_BUF_SIZE  = 255;
int toRead = 0;
int i = 0;

//Return values
int retVal;

//Buffers

DWORD sizeOfData;  


//Handles
HANDLE comPortHandle;
COMSTAT status;
DWORD errors;
DWORD eventMask = EV_TXEMPTY|EV_RXCHAR;
OVERLAPPED overlap = {0};
DCB dcb = {0};
	
	
//This function populates the passed in data array
//Returns 1 when there are elements read from the Serial com port
//returns 0 if nothing is read from the port
int readData(char data[READ_BUF_SIZE]){
	Sleep(4000);
	//clear any errors
	ClearCommError(comPortHandle, &errors, &status);
	//Print out the number of elements in the queue
	printf("Queue size %d\n", status.cbInQue);
	
	//If the number of elements is less than the maximum size of the char read that number
	if(status.cbInQue>READ_BUF_SIZE){
		toRead = READ_BUF_SIZE;
	}
	//Else only read the maximum
	//This will cause the additional information to be read on the next read cycle.
	else{
		toRead = status.cbInQue;
	}
	//If there are present elements in the queue read them and return 1 for success
	if (toRead > 0){
		retVal = ReadFile(comPortHandle, data, toRead, &sizeOfData, &overlap);
		if (retVal) {
			return 1;
		} else {
			printf("ReadFile has failed\n");
		}
	}
	return 0;
}



int main (int argc, char** argv){

	//Data variables
	unsigned char data[READ_BUF_SIZE];
	int division;
	time_t currentTime;
	int dataExists = 0;

	//Open data logging file
	FILE* csv;
	csv = fopen("C:/Users/DanielB/Desktop/moistureData.txt", "a+");
	fprintf(csv, "Moisture Level, Time\n");
	
	
	//Open COM6 and get a handle
	comPortHandle = CreateFile( "COM6",  
                    GENERIC_READ | GENERIC_WRITE, //permissions (read OR write)
                    0, //sharing (none allowed)
                    NULL, //security (none)
                    OPEN_EXISTING, //creation
                    FILE_FLAG_OVERLAPPED,
                    NULL);
					
	if (comPortHandle == INVALID_HANDLE_VALUE) {
		printf("Error opening port\n");
		printf("Error: %d\n", GetLastError());	
	}


	//Get status of comm device
	retVal = GetCommState(comPortHandle, &dcb);
	if (retVal == 0) {
		printf("Error getting Comm State\n");
		printf("retVa; %d\n", retVal);
		printf("Error: %d\n", GetLastError());
		return 0;
	}
	
	//Set Controller parameters
	dcb.BaudRate = CBR_9600;
	dcb.ByteSize = 8;
	dcb.Parity = NOPARITY;
	dcb.StopBits = ONESTOPBIT;
	dcb.fDtrControl = DTR_CONTROL_ENABLE;
	
	//Set Port settings
	retVal = SetCommState(comPortHandle, &dcb);
	if (retVal == 0) {
		printf("Error setting Comm State");
		printf("retVal %d\n", retVal);
		printf("Error: %d\n", GetLastError());
		return 0;
	}
	
	//Create event mask
	retVal = SetCommMask(comPortHandle, eventMask); //trigger for finished transmission or recieve letter
	
	printf("Connected!\n");
	
	//clear comms
	PurgeComm(comPortHandle, PURGE_RXCLEAR | PURGE_TXCLEAR);
	
	//create event for overlap to trigger
	overlap.hEvent = CreateEvent(0,1,0,0);
	WaitCommEvent(comPortHandle, &eventMask, &overlap);


	while(1) {
		//wait for an event. Block until that occurance
		if (WaitForSingleObject(overlap.hEvent, INFINITE) == WAIT_OBJECT_0){	
			memset(data, 0, sizeof(data));
			dataExists = readData(data);
			printf("\nDivision: %d\n", data[0]);
			printf("\nData: %d\n", data[1]);
			printf("\n\n");
			if (dataExists){
				time(&currentTime);
				fprintf(csv, "%d, %d\n", (data[0]*255) + data[1], currentTime);
			}
		}	
	}
}