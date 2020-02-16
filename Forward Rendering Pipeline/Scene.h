#ifndef _SCENE_H_
#define _SCENE_H_

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include "Camera.h"
#include "Color.h"
#include "Model.h"
#include "Rotation.h"
#include "Scaling.h"
#include "Translation.h"
#include "Triangle.h"
#include "Vec3.h"
#include "Vec4.h"

using namespace std;

typedef struct Window
{
	double xmin;
	double xmax;
	double ymin;
	double ymax;
	double zmin;
	double zmax;

}Window;


class Scene
{
public:
	Color backgroundColor;
	bool cullingEnabled;
	int projectionType;

	vector< vector<Color> > image;
	vector< Camera* > cameras;
	vector< Vec3* > vertices;
	vector< Color* > colorsOfVertices;
	vector< Scaling* > scalings;
	vector< Rotation* > rotations;
	vector< Translation* > translations;
	vector< Model* > models;

	Scene(const char *xmlPath);

	void initializeImage(Camera* camera);
	void forwardRenderingPipeline(Camera* camera);
	int makeBetweenZeroAnd255(double value);
	void writeImageToPPMFile(Camera* camera);
	void convertPPMToPNG(string ppmFileName, int osType);

	Vec4 Vec3ToVec4(const Vec3 &v);
	Vec4 perspectiveDivide(const Vec4 &v);
	Vec3 Vec4ToVec3(const Vec4 &v);
	void Draw(int i, int j, double c[3]);
	void Wireframe(const Vec3 v[3], Window w);
	void LineRasterization(const Vec3 &v1, const Vec3 &v2, double tE, double tL);
	bool LineClipping(const Vec3 &v1, const Vec3 &v2, Window w, double &tE, double &tL);
	void Solid(const Vec3 v[3],int nx,int ny);
};



#endif
