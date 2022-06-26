#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>

int main()
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	// you can loop k higher to see more color choices
	for (int k = 1; k < 255; k++)
	{
		// pick the colorattribute k you want
		SetConsoleTextAttribute(hConsole, k);
		printf("%d 0x%x I want to be nice today!\n", k, k);
	}

    return 1;
}