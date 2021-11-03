#include "stdafx.h"
#include "ProceduralGenerator.h"
#include <omp.h>

void ProceduralGenerator::setCurrentCloudScene(PointCloudScene* pointCloudScene)
{
	for (size_t x = 0; x < axisSubdivision[0]; x++)
	{
		for (size_t y = 0; y < axisSubdivision[1]; y++)
		{
			for (size_t z = 0; z < axisSubdivision[2]; z++)
			{
				delete subdivisions[x][y][z];
			}
		}
	}

	this->_pointCloudScene = pointCloudScene;
	calculateCloudDensity();

	readParameters("proceduralParameters.ini");
	createVoxelGrid();
	subdivideCloud();
}

ProceduralGenerator::~ProceduralGenerator()
{
	for (size_t x = 0; x < axisSubdivision[0]; x++)
	{
		for (size_t y = 0; y < axisSubdivision[1]; y++)
		{
			for (size_t z = 0; z < axisSubdivision[2]; z++)
			{
				delete subdivisions[x][y][z];
			}
		}
	}
}

void ProceduralGenerator::calculateCloudDensity()
{
	if (_pointCloudScene) {
		int numberPoints = _pointCloudScene->_pointCloud->getNumberOfPoints();
		vec3 AABBSize = _pointCloudScene->getAABB().size();
		cloudDensity = numberPoints / (AABBSize.x * AABBSize.y * AABBSize.z);
		std::cout << "Cloud density: " << cloudDensity << std::endl;
	}
	else {
		std::cerr << "[ProceduralGenerator]->Nullptr in _pointCloudScene" << std::endl;
	}
}

void ProceduralGenerator::readParameters(const std::string& path)
{
	std::ifstream parametersFile;
	parametersFile.open(path, std::ifstream::in);

	if (!parametersFile) {
		std::cerr << "[ProceduralGenerator]->Error while trying to open the parameters file. Path: " + path << std::endl;
	}
	else {
		std::string line;
		while (!parametersFile.eof()) {
			getline(parametersFile, line);
			if (line != "") {
				int pos = line.find("=");
				std::string parameter = line.substr(0, pos);
				std::string value = line.substr(++pos, line.length());
				if (parameter == "gsd")			   gsd = stof(value);
				else if (parameter == "expansion") expansion = stoi(value);
			}
		}
		parametersFile.close();
		vec3 size = _pointCloudScene->_pointCloud->getAABB().size();
		for (size_t i = 0; i < 2; i++)
		{
			axisSubdivision[i] = round(size[i] / gsd);
			axisSubdivisionOriginal[i] = axisSubdivision[i];
		}
		//axisSubdivisionOriginal[0] = axisSubdivision[0] = 1000;
		//axisSubdivisionOriginal[1] = axisSubdivision[1] = 500;
		axisSubdivisionOriginal[2] = axisSubdivision[2] = 1;
	}
}

void ProceduralGenerator::createVoxelGrid()
{
	vec3 size = _pointCloudScene->_pointCloud->getAABB().size();
	vec3 minPoint = _pointCloudScene->_pointCloud->getAABB().min();
	float stride[3];

	for (unsigned i = 0; i < 3; i++)
	{
		stride[i] = size[i] / axisSubdivision[i];
		axisSubdivision[i] *= expansion;
		minPoint[i] *= expansion;
	}
	axisSubdivision[2] /= expansion;
	minPoint[2] /= expansion;

	subdivisions.resize(axisSubdivision[0]);
	for (size_t x = 0; x < axisSubdivision[0]; x++)
	{
		subdivisions[x].resize(axisSubdivision[1]);
		for (size_t y = 0; y < axisSubdivision[1]; y++)
		{
			subdivisions[x][y].resize(axisSubdivision[2]);
			for (size_t z = 0; z < axisSubdivision[2]; z++)
			{
				AABB* newAABB = new AABB;
				vec3 point(minPoint);
				point[0] += stride[0] * x;
				point[1] += stride[1] * y;
				point[2] += stride[2] * z;
				newAABB->update(point);
				point[0] += stride[0];
				point[1] += stride[1];
				point[2] += stride[2];
				newAABB->update(point);
				ProceduralVoxel* procVoxel = new ProceduralVoxel(_pointCloudScene->_pointCloud, newAABB);
				subdivisions[x][y][z] = procVoxel;
				//_pointCloudScene->_sceneGroup->addComponent(procVoxel);
			}
		}
	}
}

void ProceduralGenerator::subdivideCloud()
{
	unsigned startPoint[3];
	for (size_t i = 0; i < 3; i++)
	{
		startPoint[i] = axisSubdivision[i] / 2 - axisSubdivisionOriginal[i] / 2;
	}

	unsigned pointCloudSize = this->_pointCloudScene->_pointCloud->getNumberOfPoints();
	std::vector<PointCloud::PointModel>* points = _pointCloudScene->_pointCloud->getPoints();

	vec3 size = _pointCloudScene->_pointCloud->getAABB().size();
	vec3 minPoint = _pointCloudScene->_pointCloud->getAABB().min();
	float stride[3];

	for (unsigned i = 0; i < 3; i++)
	{
		stride[i] = size[i] / axisSubdivisionOriginal[i];
	}

	unsigned x, y, z;
	glm::vec3 relativePoint;
	unsigned xOffset, yOffset, zOffset;
	xOffset = (axisSubdivision[0] - axisSubdivisionOriginal[0]) / 2;
	yOffset = (axisSubdivision[1] - axisSubdivisionOriginal[1]) / 2;
	zOffset = (axisSubdivision[2] - axisSubdivisionOriginal[2]) / 2;
	for (unsigned i = 0; i < points->size(); i++)
	{
		relativePoint = (*points)[i]._point - minPoint;
		x = floor(relativePoint[0] / stride[0]) + xOffset;
		y = floor(relativePoint[1] / stride[1]) + yOffset;
		z = floor(relativePoint[2] / stride[2]) + zOffset;
		if (x == axisSubdivisionOriginal[0] + xOffset)
			x--;
		if (y == axisSubdivisionOriginal[1] + yOffset)
			y--;
		if (z == axisSubdivisionOriginal[2] + zOffset)
			z--;

		subdivisions[x][y][z]->addPoint(i);
	}

#pragma omp parallel for collapse(3)
	for (int x = startPoint[0]; x < startPoint[0] + axisSubdivisionOriginal[0]; x++)
	{
		for (int y = startPoint[1]; y < startPoint[1] + axisSubdivisionOriginal[1]; y++)
		{
			for (int z = startPoint[2]; z < startPoint[2] + axisSubdivisionOriginal[2]; z++)
			{
				subdivisions[x][y][z]->setProcedural(false);
				subdivisions[x][y][z]->computeHeight();
				subdivisions[x][y][z]->computeColor();
				//subdivisions[x][y][z]->checkPoints();
			}
		}
	}

	saveHeightMap();
	saveTextureMap();
}

void ProceduralGenerator::saveHeightMap()
{
	float minPointZ = _pointCloudScene->_pointCloud->getAABB().min()[2];
	float relativeMaxPointZ = _pointCloudScene->_pointCloud->getAABB().max()[2] - minPointZ;
	float relativeHeightValue;
	float height;
	std::vector<unsigned char>* pixels = new std::vector<unsigned char>();
	for (int y = 0; y < axisSubdivision[1]; y++)
	{
		for (int x = 0; x < axisSubdivision[0]; x++)
		{
			unsigned char color;
			height = subdivisions[x][y][0]->getHeight();
			if (height != FLT_MAX)
				relativeHeightValue = (height - minPointZ) / relativeMaxPointZ;
			else
				relativeHeightValue = 0;
			color = std::min(255, int(relativeHeightValue * 256.0f));
			glm::vec3 aux = subdivisions[x][y][0]->getColor();
			pixels->push_back(color);
			pixels->push_back(color);
			pixels->push_back(color);
			pixels->push_back(255);

		}
	}
	Image* image = new Image(pixels->data(), axisSubdivision[0], axisSubdivision[1], 4);
	image->saveImage("heightmap.png");
}

void ProceduralGenerator::saveTextureMap()
{
	float minPointZ = _pointCloudScene->_pointCloud->getAABB().min()[2];
	float relativeMaxPointZ = _pointCloudScene->_pointCloud->getAABB().max()[2] - minPointZ;
	float relativeHeightValue;
	glm::vec3 color;
	std::vector<unsigned char>* pixels = new std::vector<unsigned char>();
	for (int y = 0; y < axisSubdivision[1]; y++)
	{
		for (int x = 0; x < axisSubdivision[0]; x++)
		{
			color = subdivisions[x][y][0]->getColor();
			pixels->push_back(color[0]);
			pixels->push_back(color[1]);
			pixels->push_back(color[2]);
			pixels->push_back(255);

		}
	}
	Image* image = new Image(pixels->data(), axisSubdivision[0], axisSubdivision[1], 4);
	image->saveImage("texturemap.png");
}