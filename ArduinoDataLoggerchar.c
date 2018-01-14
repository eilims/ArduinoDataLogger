#include <stdio.h>
#include <unistd.h>
#include <windows.h>
#include <winbase.h>

int main (int argc, char** argv){
	//Variables
	DWORD READ_BUF_SIZE  = 255;
	int toRead = 0;
	
	//Return values
	int retVal;
	
	//Buffers
	char buf[READ_BUF_SIZE];
	int moisutreData;
	memset(buf, 0, sizeof(buf));
	DWORD sizeOfData;  
	
	
	//Handles
	HANDLE comPortHandle;
	COMSTAT status;
	DWORD errors;
	DWORD eventMask = EV_TXEMPTY|EV_RXCHAR;
	OVERLAPPED overlap = {0};
	DCB dcb = {0};
	
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
	
	printf("Connected!");
	
	//clear comms
	PurgeComm(comPortHandle, PURGE_RXCLEAR | PURGE_TXCLEAR);
	
	//create event for overlap to trigger
	overlap.hEvent = CreateEvent(0,1,0,0);
	WaitCommEvent(comPortHandle, &eventMask, &overlap);

	
	
	
	while(1) {
		//wait for an event. Block until that occurance
		printf("Loop Start\n");
		if (WaitForSingleObject(overlap.hEvent, INFINITE) == WAIT_OBJECT_0){	
			//Wait for the serial line to be fully propagated
			Sleep(4000);
			//clear any errors
			ClearCommError(comPortHandle, &errors, &status);
			printf("Queue size %d\n", status.cbInQue);
			if(status.cbInQue>READ_BUF_SIZE){
				toRead = READ_BUF_SIZE;
			}
			else{
				toRead = status.cbInQue;
			}
			if (toRead > 0){
				retVal = ReadFile(comPortHandle, buf, toRead, &sizeOfData, &overlap);
				if (retVal) {
					printf("retVal %d\n", retVal);
					printf("Error: %d\n", GetLastError());
					printf("sizeOfData %d\n", sizeOfData);
					printf("buf %s\n", buf);
				} else {
					printf("ReadFile has failed\n");
				}
			}
		}
	 }
}