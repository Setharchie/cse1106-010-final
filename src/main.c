#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <pigpio.h>

#define READING_COUNT 5
#define PIN 17

double calculateAverage(double readings[], int count)
{
	double total = 0;
	for (int i = 0; i < count; i++)
	{
		total += readings[i];
	}
	return total / count;
}

void displayReadings(double readings[], int count)
{
	printf("Temperature readings: \n");
	for (int i = 0; i < count; i++)
	{
		printf("%.1f F\n", readings[i]);
	}
}

int isOutofRange (double temperature)
{
	return (temperature < 68 || temperature > 76) ? 1 : 0;
}

void checkReadings(double readings[], int count)
{
	printf("\nReadings requiring attention : \n");
	for (int i = 0; i < count; i++)
	{
		if (isOutofRange(readings[i]))
		{
			printf("%.1f F\n", readings[i]);
		}
	}
}


/* this is where we actually get our input */

int main()
{
	FILE *file = fopen("reading.txt", "a");
	
	if (file == NULL)
	{
		printf("could not open the file.\n");
		gpioTerminate();
		return 1;
	}
	
	gpioInitialise();
	gpioSetMode(PIN, PI_OUTPUT);
	int i;
	char status[100];
	double readings[READING_COUNT];
	
	printf("Smart Classroom temperature monitor\n\n");
	
	printf("Please enter up to %d temperature readings\n", READING_COUNT);
	
	for (int i = 0; i < READING_COUNT; i++)
	{
		printf("\nReading %d: ", i + 1);
		scanf("%lf", &readings[i]);
		
		fprintf(file, "%.1f F\n", readings[i]);
	
		if (isOutofRange(readings[i]))
		{
			printf("WARNING\n");
			gpioWrite(PIN, 1); // LED ON
			getchar();
			getchar();
			gpioWrite(PIN, 0); // LED OFF
			fprintf(file, "WARNING\n");

		}
		else
		{
			printf("NORMAL\n");
			gpioWrite(PIN, 0); // LED OFF
			getchar();
			getchar();
			fprintf(file, "NORMAL\n");
		}
		
		
	}
	
	displayReadings(readings, READING_COUNT);
	printf("\nAverage temperature: %.1f F\n", calculateAverage(readings, READING_COUNT));
	checkReadings(readings, READING_COUNT);
	
	fclose(file);
	

	gpioTerminate();
	return 0;
}
