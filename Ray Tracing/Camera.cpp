#include "Camera.h"
#include <cstring>

Camera::Camera(int id,                      // Id of the camera
               const char* imageName,       // Name of the output PPM file 
               const Vector3f& pos,         // Camera position
               const Vector3f& gaze,        // Camera gaze direction
               const Vector3f& up,          // Camera up direction
               const ImagePlane& imgPlane)  // Image plane parameters
{
	this->id = id;

	this->pos = pos;
	Vector3f _gaze = gaze.normalized();
	Vector3f _u = (_gaze % up).normalized();
	Vector3f _up = (_u % _gaze).normalized();
	this->imgPlane = imgPlane;
	

	this->planeTopLeft = pos + (_gaze * imgPlane.distance) + (_u * imgPlane.left) + (_up * imgPlane.top);

	float h = (imgPlane.right - imgPlane.left) / (float)imgPlane.nx;
	float v = (imgPlane.top - imgPlane.bottom) / (float)imgPlane.ny;

	horizantal = _u * h;
	vertical = _up * v;

	strcpy(this->imageName, imageName);
}

/* Takes coordinate of an image pixel as row and col, and
 * returns the ray going through that pixel. 
 */
Ray Camera::getPrimaryRay(int row, int col) const
{
	Vector3f pointOnPlane = planeTopLeft + horizantal * (row + 0.5) - vertical * (col + 0.5) ;

	Vector3f direction = (pointOnPlane - pos).normalized();

	return Ray(pos, direction);
}

