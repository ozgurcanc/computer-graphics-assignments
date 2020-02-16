#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <fstream>
#include <cmath>

#include "Scene.h"
#include "Camera.h"
#include "Color.h"
#include "Model.h"
#include "Rotation.h"
#include "Scaling.h"
#include "Translation.h"
#include "Triangle.h"
#include "Vec3.h"
#include "tinyxml2.h"
#include "Helpers.h"

#include <map>
#include <algorithm>
#define DegToRad 3.14159265/180.0

using namespace tinyxml2;
using namespace std;

/*
	Transformations, clipping, culling, rasterization are done here.
	NOT implemented depth buffer algorithm. Models are given in back-to-front order.
*/


void Scene::forwardRenderingPipeline(Camera *camera)
{
	
	map<int, Matrix4> translationMatrix;
	map<int, Matrix4> scalingMatrix;
	map<int, Matrix4> rotationMatrix;

	for (Translation* t : translations)
	{
		double val[4][4] =
		{
			{1,0,0,t->tx},
			{0,1,0,t->ty},
			{0,0,1,t->tz},
			{0,0,0,1}
		};
		
		Matrix4 m(val);

		translationMatrix.insert(pair<int, Matrix4>(t->translationId, m));
	}

	for (Scaling* s : scalings)
	{
		double val[4][4] =
		{
			{s->sx,0,0,0},
			{0,s->sy,0,0},
			{0,0,s->sz,0},
			{0,0,0,1}
		};

		Matrix4 m(val);

		scalingMatrix.insert(pair<int, Matrix4>(s->scalingId, m));
	}

	for (Rotation* r : rotations)
	{
		int index = 0;
		double minAbs = abs(r->ux);

		if (abs(r->uy) < minAbs)
		{
			index = 1;
			minAbs = abs(r->uy);
		}
		if (abs(r->uz) < minAbs)
		{
			index = 2;
			minAbs = abs(r->uz);
		}

		Vec3 u(r->ux, r->uy, r->uz,0);
		Vec3 v = index == 0 ? Vec3(0, -r->uz, r->uy, 0) : index == 1 ? Vec3(-r->uz, 0, r->ux, 0) : Vec3(-r->uy, r->ux, 0, 0);
		Vec3 w = crossProductVec3(u, v);

		v = normalizeVec3(v);
		w = normalizeVec3(w);
			
		double m_Val[4][4] =
		{
			{u.x,u.y,u.z,0},
			{v.x,v.y,v.z,0},
			{w.x,w.y,w.z,0},
			{0,0,0,1}
		};
		
		double angle = r->angle * DegToRad;
		double c = cos(angle);
		double s = sin(angle);

		double rX_Val[4][4] =
		{
			{1,0,0,0},
			{0,c,-s,0},
			{0,s,c,0},
			{0,0,0,1}
		};

		double mI_Val[4][4] =
		{
			{u.x,v.x,w.x,0},
			{u.y,v.y,w.y,0},
			{u.z,v.z,w.z,0},
			{0,0,0,1}
		};

		Matrix4 m(m_Val);
		Matrix4 mI(mI_Val);
		Matrix4 rX(rX_Val);
		
		Matrix4 composition = multiplyMatrixWithMatrix(mI, multiplyMatrixWithMatrix(rX, m));

		rotationMatrix.insert(pair<int, Matrix4>(r->rotationId, composition));
	}


	double mCam_Val[4][4] =
	{
		{camera->u.x,camera->u.y,camera->u.z,-dotProductVec3(camera->u,camera->pos)},
		{camera->v.x,camera->v.y,camera->v.z,-dotProductVec3(camera->v,camera->pos)},
		{camera->w.x,camera->w.y,camera->w.z,-dotProductVec3(camera->w,camera->pos)},
		{0,0,0,1}
	};

	Matrix4 mCam(mCam_Val);

	double dx = camera->right - camera->left;
	double dy = camera->top - camera->bottom;
	double dz = camera->far - camera->near;

	double px = camera->right + camera->left;
	double py = camera->top + camera->bottom;
	double pz = camera->far + camera->near;

	Matrix4 mProj;

	if (projectionType == 0)
	{
		double mOrth_Val[4][4] =
		{
			{2/dx,0,0,-px/dx},
			{0,2/dy,0,-py/dy},
			{0,0,-2/dz,-pz/dz},
			{0,0,0,1}
		};
		mProj = Matrix4(mOrth_Val);
	}
	else
	{
		double n = camera->near;
		double f = camera->far;

		double mPers_Val[4][4] =
		{
			{(2*n)/dx,0,px/dx,0},
			{0,(2 * n)/dy,py/dy,0},
			{0,0,-pz/dz,-(2*f*n)/dz},
			{0,0,-1,0}
		};
		mProj = Matrix4(mPers_Val);
	}

	Matrix4 mView = multiplyMatrixWithMatrix(mProj, mCam);

	double nx = camera->horRes;
	double ny = camera->verRes;

	Window window;
	window.xmin = -0.5 + 0.0001;
	window.xmax = nx-0.5 - 0.0001;
	window.ymin = -0.5 + 0.0001;
	window.ymax = ny-0.5 - 0.0001;
	window.zmin = 0;
	window.zmax = 1;

 
	double mViewport_Val[4][4] =
	{
		{nx/2,0,0,nx/2-0.5},
		{0,ny/2,0,ny/2-0.5},
		{0,0,0.5,0.5},
		{0,0,0,1}
	};
	
	Matrix4 mViewport(mViewport_Val);
	
	
	for (Model *model : models)
	{
		int numberOfTransformations = model->numberOfTransformations;

		int rasterizationType = model->type;

		Matrix4 mWorld(getIdentityMatrix());

		for (int i = 0; i < numberOfTransformations; i++)
		{
			switch (model->transformationTypes[i])
			{
			case 't':
				mWorld = multiplyMatrixWithMatrix(translationMatrix[model->transformationIds[i]], mWorld);
				break;
			case 's':
				mWorld = multiplyMatrixWithMatrix(scalingMatrix[model->transformationIds[i]], mWorld);
				break;
			case 'r':
				mWorld = multiplyMatrixWithMatrix(rotationMatrix[model->transformationIds[i]], mWorld);
				break;
			default:
				break;
			}
		}

		Matrix4 mWorldViewProj = multiplyMatrixWithMatrix(mView, mWorld);
		

		for (Triangle triangle : model->triangles)
		{
			Vec3 v[3];
			Vec3 vBeforeVP[3];
			int i = 0;
			for (int index : triangle.vertexIds)
			{
				
				
				Vec4 temp = Vec3ToVec4(*(vertices[index - 1]));
				temp = multiplyMatrixWithVec4(mWorldViewProj, temp);
				temp = perspectiveDivide(temp);

				vBeforeVP[i] = Vec4ToVec3(temp);

				temp = multiplyMatrixWithVec4(mViewport, temp);
				Vec3 vertexOut = Vec4ToVec3(temp);
				vertexOut.colorId = index - 1;
						
				v[i] = vertexOut;
				i++;
			}

			if (cullingEnabled)
			{
				Vec3 normal = crossProductVec3(subtractVec3(vBeforeVP[2], vBeforeVP[1]), subtractVec3(vBeforeVP[0], vBeforeVP[1]));
				if (dotProductVec3(normal, vBeforeVP[0]) < 0) continue;
			}
			
			if (rasterizationType) Solid(v, nx - 1, ny - 1);
			else Wireframe(v,window);
			
		}
		
	}
	
}
void Scene::LineRasterization(const Vec3 &v1, const Vec3 &v2,double tE, double tL)
{
	Color *cV1 = colorsOfVertices.at(v1.colorId);
	Color *cV2 = colorsOfVertices.at(v2.colorId);
	
	double d1 = v2.x - v1.x;
	double d2 = v2.y - v1.y;
	double d3 = v2.z - v1.z;
	
	int x1 = round(v1.x + d1 * tL);
	int y1 = round(v1.y + d2 * tL);
	double z1 = v1.z + d3 * tL;

	int x0 = round(v1.x + d1 * tE);
	int y0 = round(v1.y + d2 * tE);
	double z0 = v1.z + d3 * tE;
	
	double d4[3] = { cV2->r - cV1->r , cV2->g - cV1->g , cV2->b - cV1->b };
	
	double c0[3] = { cV1->r + d4[0] * tE, cV1->g + d4[1] * tE ,cV1->b + d4[2] * tE };
	double c1[3] = { cV1->r + d4[0] * tL, cV1->g + d4[1] * tL ,cV1->b + d4[2] * tL };
	


	int dx = abs(x0 - x1);
	int dy = -abs(y0 - y1);
	int signX = x0 < x1 ? 1 : -1;
	int signY = y0 < y1 ? 1 : -1;

	int d = dx + dy;

	double dc[3];

	if (dx > -dy)
	{
		for (int i = 0; i < 3; i++) dc[i] = (c1[i] - c0[i]) / dx;
	}
	else
	{
		for (int i = 0; i < 3; i++) dc[i] = (c1[i] - c0[i]) / -dy;
	}

	while (true)
	{
		Draw(x0, y0, c0);
		if (x0 == x1 && y0 == y1) break;
		int temp = 2 * d;
		if (temp >= dy)
		{
			d += dy;
			x0 += signX;
		}
		if (temp <= dx)
		{
			d += dx;
			y0 += signY;
		}
		for (int i = 0; i < 3; i++) c0[i] += dc[i];
	}

}

bool Scene::LineClipping(const Vec3 &v1, const Vec3 &v2, Window w,double &tE,double &tL)
{
	
	double p1 = v2.x - v1.x;
	double p2 = -p1;
	double p3 = v2.y - v1.y;
	double p4 = -p3;
	double p5 = v2.z - v1.z;
	double p6 = -p5;

	double q1 = w.xmin - v1.x;
	double q2 = v1.x - w.xmax;
	double q3 = w.ymin - v1.y;
	double q4 = v1.y - w.ymax;
	double q5 = w.zmin - v1.z;
	double q6 = v1.z - w.zmax;


	if ((p1 == 0 && q1 > 0) || (p3 == 0 && q3 > 0) || (p5 == 0 && q5 > 0))
	{
		return false;
	}

	if (p1 != 0)
	{
		double r1 = q1 / p1;
		double r2 = q2 / p2;
		if (p1 > 0)
		{
			tE = r1 > tE ? r1 : tE;
			tL = r2 < tL ? r2 : tL;
		}
		else
		{
			tE = r2 > tE ? r2 : tE;
			tL = r1 < tL ? r1 : tL;
		}
	}
	if (p3 != 0)
	{
		double r3 = q3 / p3;
		double r4 = q4 / p4;
		
		if (p3 > 0)
		{
			tE = r3 > tE ? r3 : tE;
			tL = r4 < tL ? r4 : tL;
		}
		else
		{
			tE = r4 > tE ? r4 : tE;
			tL = r3 < tL ? r3 : tL;
		}
	}
	if (p5 != 0)
	{
		double r5 = q5 / p5;
		double r6 = q6 / p6;

		if (p5> 0)
		{
			tE = r5 > tE ? r5 : tE;
			tL = r6 < tL ? r6 : tL;
		}
		else
		{
			tE = r6 > tE ? r6 : tE;
			tL = r5 < tL ? r5 : tL;
		}
	}

	if (tE > tL)
	{
		return false;
	}


	return true;

}


void Scene::Wireframe(const Vec3 v[3], Window w)
{
	
	
	double tE = 0;
	double tL = 1;

	if (LineClipping(v[0], v[1], w,tE,tL))
	{		
		LineRasterization(v[0], v[1],tE,tL);
	}

	tE = 0;
	tL = 1;

	if (LineClipping(v[2], v[1], w, tE, tL))
	{	
		LineRasterization(v[2], v[1], tE, tL);
	}

	tE = 0;
	tL = 1;
	
	if (LineClipping(v[0], v[2], w, tE, tL))
	{
		LineRasterization(v[0], v[2], tE, tL);
	}
	
}


void Scene::Solid(const Vec3 v[3], int nx, int ny)
{
	Color *cV0 = colorsOfVertices.at(v[0].colorId);
	Color *cV1 = colorsOfVertices.at(v[1].colorId);
	Color *cV2 = colorsOfVertices.at(v[2].colorId);

	double c0[3] = { cV0->r , cV0->g , cV0->b };
	double c1[3] = { cV1->r , cV1->g , cV1->b };
	double c2[3] = { cV2->r , cV2->g , cV2->b };

	double color[3];

	int x0 = round(v[0].x);
	int y0 = round(v[0].y);

	int x1 = round(v[1].x);
	int y1 = round(v[1].y);

	int x2 = round(v[2].x);
	int y2 = round(v[2].y);

	int dy01 = y0 - y1;
	int dy12 = y1 - y2;
	int dy20 = y2 - y0;

	int dx01 = x1 - x0;
	int dx12 = x2 - x1;
	int dx20 = x0 - x2;

	int d01 = x0 * y1 - y0 * x1;
	int d12 = x1 * y2 - y1 * x2;
	int d20 = x2 * y0 - y2 * x0;

	int minx = min(x0, min(x1, x2));
	int maxx = max(x0, max(x1, x2));

	int miny = min(y0, min(y1, y2));
	int maxy = max(y0, max(y1, y2));

	minx = max(0, minx);
	maxx = min(nx, maxx);

	miny = max(0, miny);
	maxy = min(ny, maxy);

	int v0 = x0 * dy12 + y0 * dx12 + d12;
	int v1 = x1 * dy20 + y1 * dx20 + d20;
	int v2 = x2 * dy01 + y2 * dx01 + d01;

	
	for (int i = minx; i <= maxx; i++)
	{
		for (int j = miny; j <= maxy; j++)
		{
			double a = (i * dy12 + j * dx12 + d12) / (double)v0;
			double b = (i * dy20 + j * dx20 + d20) / (double)v1;
			double c = (i * dy01 + j * dx01 + d01) / (double)v2;

			if (a >= 0 && b >= 0 && c >= 0 )
			{
				for (int k = 0; k < 3; k++) color[k] = a * c0[k] + b * c1[k] + c * c2[k];
				Draw(i, j, color);
			}
				
		}
	}
}

Vec4 Scene::Vec3ToVec4(const Vec3 &v)
{
	Vec4 vec4(v.x, v.y, v.z, 1 , 0);
	return vec4;
}

Vec3 Scene::Vec4ToVec3(const Vec4 &v)
{
	Vec3 v1;
	v1.x = v.x;
	v1.y = v.y;
	v1.z = v.z;
	return v1;
}

Vec4 Scene::perspectiveDivide(const Vec4 &v)
{
	Vec4 v1;
	v1.x = v.x / v.t;
	v1.y = v.y / v.t;
	v1.z = v.z / v.t;
	v1.t = 1;

	return v1;
}

void Scene::Draw(int i, int j, double c[3])
{
	
	this->image[i][j].r = makeBetweenZeroAnd255(round(c[0]));
	this->image[i][j].g = makeBetweenZeroAnd255(round(c[1]));
	this->image[i][j].b = makeBetweenZeroAnd255(round(c[2]));
}
/*
	Parses XML file
*/
Scene::Scene(const char *xmlPath)
{
	const char *str;
	XMLDocument xmlDoc;
	XMLElement *pElement;

	xmlDoc.LoadFile(xmlPath);

	XMLNode *pRoot = xmlDoc.FirstChild();

	// read background color
	pElement = pRoot->FirstChildElement("BackgroundColor");
	str = pElement->GetText();
	sscanf(str, "%lf %lf %lf", &backgroundColor.r, &backgroundColor.g, &backgroundColor.b);

	// read culling
	pElement = pRoot->FirstChildElement("Culling");
	if (pElement != NULL)
		pElement->QueryBoolText(&cullingEnabled);

	// read projection type
	pElement = pRoot->FirstChildElement("ProjectionType");
	if (pElement != NULL)
		pElement->QueryIntText(&projectionType);

	// read cameras
	pElement = pRoot->FirstChildElement("Cameras");
	XMLElement *pCamera = pElement->FirstChildElement("Camera");
	XMLElement *camElement;
	while (pCamera != NULL)
	{
		Camera *cam = new Camera();

		pCamera->QueryIntAttribute("id", &cam->cameraId);

		camElement = pCamera->FirstChildElement("Position");
		str = camElement->GetText();
		sscanf(str, "%lf %lf %lf", &cam->pos.x, &cam->pos.y, &cam->pos.z);

		camElement = pCamera->FirstChildElement("Gaze");
		str = camElement->GetText();
		sscanf(str, "%lf %lf %lf", &cam->gaze.x, &cam->gaze.y, &cam->gaze.z);

		camElement = pCamera->FirstChildElement("Up");
		str = camElement->GetText();
		sscanf(str, "%lf %lf %lf", &cam->v.x, &cam->v.y, &cam->v.z);

		cam->gaze = normalizeVec3(cam->gaze);
		cam->u = crossProductVec3(cam->gaze, cam->v);
		cam->u = normalizeVec3(cam->u);

		cam->w = inverseVec3(cam->gaze);
		cam->v = crossProductVec3(cam->u, cam->gaze);
		cam->v = normalizeVec3(cam->v);

		camElement = pCamera->FirstChildElement("ImagePlane");
		str = camElement->GetText();
		sscanf(str, "%lf %lf %lf %lf %lf %lf %d %d",
			   &cam->left, &cam->right, &cam->bottom, &cam->top,
			   &cam->near, &cam->far, &cam->horRes, &cam->verRes);

		camElement = pCamera->FirstChildElement("OutputName");
		str = camElement->GetText();
		cam->outputFileName = string(str);

		cameras.push_back(cam);

		pCamera = pCamera->NextSiblingElement("Camera");
	}

	// read vertices
	pElement = pRoot->FirstChildElement("Vertices");
	XMLElement *pVertex = pElement->FirstChildElement("Vertex");
	int vertexId = 1;

	while (pVertex != NULL)
	{
		Vec3 *vertex = new Vec3();
		Color *color = new Color();

		vertex->colorId = vertexId;

		str = pVertex->Attribute("position");
		sscanf(str, "%lf %lf %lf", &vertex->x, &vertex->y, &vertex->z);

		str = pVertex->Attribute("color");
		sscanf(str, "%lf %lf %lf", &color->r, &color->g, &color->b);

		vertices.push_back(vertex);
		colorsOfVertices.push_back(color);

		pVertex = pVertex->NextSiblingElement("Vertex");

		vertexId++;
	}

	// read translations
	pElement = pRoot->FirstChildElement("Translations");
	XMLElement *pTranslation = pElement->FirstChildElement("Translation");
	while (pTranslation != NULL)
	{
		Translation *translation = new Translation();

		pTranslation->QueryIntAttribute("id", &translation->translationId);

		str = pTranslation->Attribute("value");
		sscanf(str, "%lf %lf %lf", &translation->tx, &translation->ty, &translation->tz);

		translations.push_back(translation);

		pTranslation = pTranslation->NextSiblingElement("Translation");
	}

	// read scalings
	pElement = pRoot->FirstChildElement("Scalings");
	XMLElement *pScaling = pElement->FirstChildElement("Scaling");
	while (pScaling != NULL)
	{
		Scaling *scaling = new Scaling();

		pScaling->QueryIntAttribute("id", &scaling->scalingId);
		str = pScaling->Attribute("value");
		sscanf(str, "%lf %lf %lf", &scaling->sx, &scaling->sy, &scaling->sz);

		scalings.push_back(scaling);

		pScaling = pScaling->NextSiblingElement("Scaling");
	}

	// read rotations
	pElement = pRoot->FirstChildElement("Rotations");
	XMLElement *pRotation = pElement->FirstChildElement("Rotation");
	while (pRotation != NULL)
	{
		Rotation *rotation = new Rotation();

		pRotation->QueryIntAttribute("id", &rotation->rotationId);
		str = pRotation->Attribute("value");
		sscanf(str, "%lf %lf %lf %lf", &rotation->angle, &rotation->ux, &rotation->uy, &rotation->uz);

		rotations.push_back(rotation);

		pRotation = pRotation->NextSiblingElement("Rotation");
	}

	// read models
	pElement = pRoot->FirstChildElement("Models");

	XMLElement *pModel = pElement->FirstChildElement("Model");
	XMLElement *modelElement;
	while (pModel != NULL)
	{
		Model *model = new Model();

		pModel->QueryIntAttribute("id", &model->modelId);
		pModel->QueryIntAttribute("type", &model->type);

		// read model transformations
		XMLElement *pTransformations = pModel->FirstChildElement("Transformations");
		XMLElement *pTransformation = pTransformations->FirstChildElement("Transformation");

		pTransformations->QueryIntAttribute("count", &model->numberOfTransformations);

		while (pTransformation != NULL)
		{
			char transformationType;
			int transformationId;

			str = pTransformation->GetText();
			sscanf(str, "%c %d", &transformationType, &transformationId);

			model->transformationTypes.push_back(transformationType);
			model->transformationIds.push_back(transformationId);

			pTransformation = pTransformation->NextSiblingElement("Transformation");
		}

		// read model triangles
		XMLElement *pTriangles = pModel->FirstChildElement("Triangles");
		XMLElement *pTriangle = pTriangles->FirstChildElement("Triangle");

		pTriangles->QueryIntAttribute("count", &model->numberOfTriangles);

		while (pTriangle != NULL)
		{
			int v1, v2, v3;

			str = pTriangle->GetText();
			sscanf(str, "%d %d %d", &v1, &v2, &v3);

			model->triangles.push_back(Triangle(v1, v2, v3));

			pTriangle = pTriangle->NextSiblingElement("Triangle");
		}

		models.push_back(model);

		pModel = pModel->NextSiblingElement("Model");
	}
}

/*
	Initializes image with background color
*/
void Scene::initializeImage(Camera *camera)
{
	if (this->image.empty())
	{
		for (int i = 0; i < camera->horRes; i++)
		{
			vector<Color> rowOfColors;

			for (int j = 0; j < camera->verRes; j++)
			{
				rowOfColors.push_back(this->backgroundColor);
			}

			this->image.push_back(rowOfColors);
		}
	}
	// if image is filled before, just change color rgb values with the background color
	else
	{
		for (int i = 0; i < camera->horRes; i++)
		{
			for (int j = 0; j < camera->verRes; j++)
			{
				this->image[i][j].r = this->backgroundColor.r;
				this->image[i][j].g = this->backgroundColor.g;
				this->image[i][j].b = this->backgroundColor.b;
			}
		}
	}
}

/*
	If given value is less than 0, converts value to 0.
	If given value is more than 255, converts value to 255.
	Otherwise returns value itself.
*/
int Scene::makeBetweenZeroAnd255(double value)
{
	if (value >= 255.0)
		return 255;
	if (value <= 0.0)
		return 0;
	return (int)(value);
}

/*
	Writes contents of image (Color**) into a PPM file.
*/
void Scene::writeImageToPPMFile(Camera *camera)
{
	ofstream fout;

	fout.open(camera->outputFileName.c_str());

	fout << "P3" << endl;
	fout << "# " << camera->outputFileName << endl;
	fout << camera->horRes << " " << camera->verRes << endl;
	fout << "255" << endl;

	for (int j = camera->verRes - 1; j >= 0; j--)
	{
		for (int i = 0; i < camera->horRes; i++)
		{
			fout << makeBetweenZeroAnd255(this->image[i][j].r) << " "
				 << makeBetweenZeroAnd255(this->image[i][j].g) << " "
				 << makeBetweenZeroAnd255(this->image[i][j].b) << " ";
		}
		fout << endl;
	}
	fout.close();
}

/*
	Converts PPM image in given path to PNG file, by calling ImageMagick's 'convert' command.
	os_type == 1 		-> Ubuntu
	os_type == 2 		-> Windows
	os_type == other	-> No conversion
*/
void Scene::convertPPMToPNG(string ppmFileName, int osType)
{
	string command;

	// call command on Ubuntu
	if (osType == 1)
	{
		command = "convert " + ppmFileName + " " + ppmFileName + ".png";
		system(command.c_str());
	}

	// call command on Windows
	else if (osType == 2)
	{
		command = "magick convert " + ppmFileName + " " + ppmFileName + ".png";
		system(command.c_str());
	}

	// default action - don't do conversion
	else
	{
	}
}
