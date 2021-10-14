#include "stdafx.h"
#include "ProceduralGenerator.h"

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
		cloudDensity = numberPoints / (AABBSize.x* AABBSize.y* AABBSize.z);
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
				if (parameter == "xSubdivisions") axisSubdivision[0] = stoi(value);
				if (parameter == "ySubdivisions") axisSubdivision[1] = stoi(value);
				if (parameter == "zSubdivisions") axisSubdivision[2] = stoi(value);
				if (parameter == "expansion")	  expansion = stoi(value);
			}
		}
		parametersFile.close();
	}
}

void ProceduralGenerator::subdivideCloud()
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
				ProceduralVoxel* procVoxel = new ProceduralVoxel(newAABB);
				subdivisions[x][y][z] = procVoxel;
				_pointCloudScene->_sceneGroup->addComponent(procVoxel);
			}
		}
	}
}