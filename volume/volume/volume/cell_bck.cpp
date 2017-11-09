#include "stdafx.h"

Cell::Cell( const DATA_TYPE * rhos, const Vector3 & A, const Vector3 & B )
{
	memcpy( rhos_, rhos, sizeof( rhos_ ) );

	bounds_ = AABB( A, B );
}

Cell::~Cell()
{

}

float Cell::gamma( Ray & ray, const float t ) const
{
	Vector3 uvw = u( ray, t );
	const float & u_t = uvw.x;
	const float & v_t = uvw.y;
	const float & w_t = uvw.z;

	const COMP_TYPE alpha_AB = rho_A() * ( 1 - u_t ) + rho_B() * u_t;
	const COMP_TYPE alpha_DC = rho_D() * ( 1 - u_t ) + rho_C() * u_t;
	const COMP_TYPE alpha_EF = rho_E() * ( 1 - u_t ) + rho_F() * u_t;
	const COMP_TYPE alpha_HG = rho_H() * ( 1 - u_t ) + rho_G() * u_t;

	const COMP_TYPE beta_0 = alpha_AB * ( 1 - v_t ) + alpha_DC * v_t;
	const COMP_TYPE beta_1 = alpha_EF * ( 1 - v_t ) + alpha_HG * v_t;

	return beta_0 * ( 1 - w_t ) + beta_1 * w_t;
}

float Cell::integrate_num( Ray & ray, const float t0, const float t1 ) const
{
	const long long n = 1e+1;	
	const COMP_TYPE dt = ( 1.0f / n ) * ( t1 - t0 );
	COMP_TYPE integ = 0;
	for ( long long i = 0; i < n; ++i )
	{
		const COMP_TYPE t = t0 + i * dt;
		const COMP_TYPE f = gamma( ray, t );
		integ += f * dt;
	}

	return integ;
}

Vector3 Cell::u( Ray & ray, const float t ) const
{
	return ( ray.eval( t ) - A() ) / ( G() - A() );
}

Vector3 Cell::A() const
{
	return bounds_.lower_bound();
}

Vector3 Cell::G() const
{
	return bounds_.upper_bound();
}

float Cell::rho_A() const
{
	return rhos_[0];
}
float Cell::rho_B() const
{
	return rhos_[1];
}
float Cell::rho_C() const
{
	return rhos_[2];
}
float Cell::rho_D() const
{
	return rhos_[3];
}

float Cell::rho_E() const
{
	return rhos_[4];
}
float Cell::rho_F() const
{
	return rhos_[5];
}
float Cell::rho_G() const
{
	return rhos_[6];
}
float Cell::rho_H() const
{
	return rhos_[7];
}
