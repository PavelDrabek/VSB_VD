#ifndef VOLUME_H_
#define VOLUME_H_

float Random( const float range_min = 0, const float range_max = 1 );

#define CACHE_LINE_SIZE 64

struct CellIndices
{
public:
	CellIndices()
	{
		i = -1;
		j = -1;
		k = -1;
	}

	CellIndices( const int i, const int j, const int k );

	int i;
	int j;
	int k;
};

struct CellHit
{
public:
	CellHit( CellIndices & indices, const float t_0, const float t_1 )
	{
		this->indices = indices;
		this->t_0 = t_0;
		this->t_1 = t_1;

		f = 0;
	}

	CellIndices indices;

	float t_0;
	float t_1;

	float f;
};

class Volume
{
public:
	Volume( const int width, const int height, const int n, const Vector3 & cell_size );
	~Volume();

	void Load( std::string & file_name_mask, const int first_slice_index, const int last_slice_index );

	Cell cell( const CellIndices & indices ) const;
	Cell cell( const int i, const int j, const int k ) const;

	CellIndices cell_indices( const Vector3 & p ) const; // vr�t� indexy bu�ky, ve kter� le�� bod p
	float find_iso(Ray& ray, std::vector<CellHit> traverse, float& t, CellIndices& indices, float threshold);

	void Traverse( Ray & ray, std::vector<CellHit> & traversed_cells );

	void Raycast( Camera & camera, const int samples = 1 );
	void RaycastLoop(Camera & camera, const int samples = 1);
	
protected:

	int offset( const int i, const int j, const int k ) const;


private:
	int width_; // horizont�ln� rozli�en� zdrojov�ho sn�mku [px]; osa y
	int height_; // vertik�ln� rozli�en� zdrojov�ho sn�mku [px]; osa -z
	int n_; // po�et �ez� [-]; osa x

	int width_step_; // po�et re�ln�ch hodnot na jednom ��dku datov� matice [-]
	
	Vector3 cell_size_; // rozm�ry bu�ky [m]
	Vector3 half_volume_size_; // rozm�ry p�lky cel�ho objemu [m]

	DATA_TYPE * data_; // datov� matice s hustotami o rozm�rech (width_step_, height_, n_) a v tomto po�ad� jsou tak� ulo�eny jednotliv� hodnoty

	AABB bounds_;

	static const int kElement_size = sizeof( DATA_TYPE );

	Vector3 getGradient(CellIndices& indicies);
};

#endif
