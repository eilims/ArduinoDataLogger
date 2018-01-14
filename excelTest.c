#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <windows.h>

int main(int argc, char** argv){
	FILE* csv;
	csv = fopen("C:/Users/DanielB/Desktop/moistureData.txt", "a+");
	fprintf(csv, "Moisture Level, System Time\n");
	fprintf(csv, "100, 1/11/2018::4:20PM\n");
}