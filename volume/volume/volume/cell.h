#ifndef CELL_H_
#define CELL_H_

#define DATA_TYPE float
#define COMP_TYPE double

class Cell
{
public:
	Cell( const DATA_TYPE * rhos, const Vector3 & A, const Vector3 & B );
	~Cell();

	//void Precompute( Ray & ray );

	float gamma(Ray & ray, const float t) const;
	float gamma( Vector3 uvw ) const;
	Vector3 grad_gamma( Ray & ray, const float t ) const;
	/*
	float gamma2( Ray & ray, const float t ) const;	
	float der_gamma( Ray & ray, const float t ) const;

	float integrate( Ray & ray, const float t0, const float t1 ) const;
	*/
	float integrate_num( Ray & ray, const float t0, const float t1 ) const;

	DATA_TYPE rho_A() const;
	DATA_TYPE rho_B() const;
	DATA_TYPE rho_C() const;
	DATA_TYPE rho_D() const;

	DATA_TYPE rho_E() const;
	DATA_TYPE rho_F() const;
	DATA_TYPE rho_G() const;
	DATA_TYPE rho_H() const;

	Vector3 A() const;
	Vector3 G() const;

protected:
	Vector3 u( Ray & ray, const float t ) const;	

private:
	DATA_TYPE rhos_[8];
	//COMP_TYPE a_[4];
	AABB bounds_;
};

#endif
