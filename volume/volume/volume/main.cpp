#include "stdafx.h"

int main(int argc, char* argv[])
{
	printf( "VD, (c)2015-2016 Tomas Fabian\n\n" );
	
	// Test 1
	//Volume volume( 3, 2, 4, Vector3( 1, 1, 1 ) ); // width x height x #slices
	//volume.Load( std::string( "../../data/slice_%04d.png" ), 0, 3 ); // 0, #slices-1

	// Test 2
	Volume volume( 512, 512, 150, Vector3( 1, 1, 1 ) ); // width x height x #slices
	volume.Load( std::string( "../../data/female_ankle/slice%03d.png" ), 1, 150 ); // 0, #slices-1

	//const float f = volume.cell( 2, 1, 0 ).rho_D(); // test interpolace dat

	Camera camera = Camera( Vector3( 500.0, 400.0, 300.0 ), Vector3( 0, 0, 0 ), 640 * 1, 480 * 1, DEG2RAD( 40 ) );

	volume.RaycastLoop(camera);
	return 0;
}
