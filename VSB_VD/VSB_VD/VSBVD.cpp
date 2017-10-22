// VSBVD.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

int main()
{
	srand(time(NULL));

	// cv03
	//ScatteredPointInterpolation a;
	//a.Execute();

	// cv04 - flow
	Flow flow(1000, 1000);
	flow.execute();

    return 0;
}

