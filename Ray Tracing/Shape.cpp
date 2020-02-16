#include "Shape.h"
#include "Scene.h"
#include <cstdio>

Shape::Shape(void)
{
}

Shape::Shape(int id, int matIndex)
    : id(id), matIndex(matIndex)
{
}

Sphere::Sphere(void)
{}


Sphere::Sphere(int id, int matIndex, int cIndex, float R,vector<Vector3f> *vertices)
    : Shape(id, matIndex)
{
	this->center = vertices->at(cIndex - 1);
	radius = R;
	

}


ReturnVal Sphere::intersect(const Ray & ray) const
{
	ReturnVal willreturn;
	
	Vector3f d = ray.direction;
	Vector3f temp = ray.origin - center;

	float delta = (d * temp) * (d*temp) - (d*d) * (temp * temp - radius * radius);

	if (delta > 0)
	{
		float sqrDelta = sqrt(delta);
		float t1 = (-(d*temp) - sqrDelta) / (d*d);
		float t2 = (-(d*temp) + sqrDelta) / (d*d);
		float t;

		float tmin, tmax;
		if (t1 < t2)
		{
			tmin = t1;
			tmax = t2;
		}
		else
		{
			tmin = t2;
			tmax = t1;
		}

		

		if (tmin > 0) {
			t = tmin;
		}
		else if (tmax > 0)
		{
			t = tmax;
		}
		else
		{
			willreturn.isValid = false;
			return willreturn;
		}

		willreturn.isValid = true;

		Vector3f intsPoint = ray.getPoint(t);
		

		willreturn.normal = (intsPoint - center).normalized();
		willreturn.incomingDir = d * -1;
		willreturn.t = t;
		willreturn.intersectPoint = intsPoint ;
		willreturn.matIndex = matIndex;

		return willreturn;

	}
	else if (delta == 0)
	{
		float t = -(d*temp) / (d*d) ;
		
		if (t < 0)
		{
			willreturn.isValid = false;
			return willreturn;
		}

		willreturn.isValid = true;
		 	
		Vector3f intsPoint = ray.getPoint(t);
	

		willreturn.normal = (intsPoint - center).normalized();
		willreturn.incomingDir = d *-1;
		willreturn.t = t;
		willreturn.intersectPoint = intsPoint;
		willreturn.matIndex = matIndex;

		return willreturn;

	}
	else
	{
		willreturn.isValid = false;
		return willreturn;
	}
}

Triangle::Triangle(void)
{}


Triangle::Triangle(int id, int matIndex, int p1Index, int p2Index, int p3Index,vector<Vector3f> *vertices)
    : Shape(id, matIndex)
{
	p1 = vertices->at(p1Index - 1);
	p2 = vertices->at(p2Index - 1);
	p3 = vertices->at(p3Index - 1);
	

}	

ReturnVal Triangle::intersect(const Ray & ray) const
{
	
	ReturnVal willreturn;


	float A_det = Vector3f::determinant3x3(p1-p2,p1-p3,ray.direction);
	
	if (A_det == 0)
	{
		willreturn.isValid = false;
		return willreturn;
	}
	else
	{
		
		float T_det = Vector3f::determinant3x3(p1 - p2, p1 - p3, p1 - ray.origin);
		float B_det = Vector3f::determinant3x3(p1 - ray.origin, p1 - p3, ray.direction);
		float G_det = Vector3f::determinant3x3(p1 - p2, p1 - ray.origin, ray.direction);
		
		float t = T_det / A_det;
		float b = B_det / A_det;
		float g = G_det / A_det;

		if (t < 0 || b < 0 || g < 0 || (b+g) > 1 )
		{
			willreturn.isValid = false;
			return willreturn;
		}
		
		willreturn.isValid = true;

		Vector3f intsPoint = ray.getPoint(t);
		willreturn.intersectPoint = intsPoint;

		Vector3f normal = ((p3 - p1) % (p2 - p1)).normalized();
		willreturn.incomingDir = ray.direction * -1;
		willreturn.normal = willreturn.incomingDir * normal > 0 ? normal : normal * -1;
		willreturn.t = t;
		willreturn.matIndex = matIndex;

		return willreturn;
	}
}

Mesh::Mesh()
{}

Mesh::Mesh(int id, int matIndex, const vector<Triangle>& faces , vector<int> *pIndices, vector<Vector3f> *vertices)
    : Shape(id, matIndex)
{
	this->faces = faces;
}


ReturnVal Mesh::intersect(const Ray & ray) const
{
	ReturnVal willreturn;
	willreturn.isValid = false;
	for (Triangle face : faces)
	{
		ReturnVal temp = face.intersect(ray);
		
		if (temp.isValid == true)
		{
			if (willreturn.isValid == true)
			{
				if (temp.t < willreturn.t) 
				{
					willreturn = temp;
				}
			}
			else 
			{
				willreturn = temp;
			}
		}
	}

	return willreturn;
}
