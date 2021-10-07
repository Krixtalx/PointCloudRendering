#include "stdafx.h"
#include "ProceduralGenerator.h"

void ProceduralGenerator::setCurrentCloudScene(PointCloudScene* pointCloudScene)
{
	this->_pointCloudScene = pointCloudScene;
	calculateCloudDensity();
	readParameters("proceduralParameters.ini");
	subdivideCloud();
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
			}
		}
		parametersFile.close();
	}
}

void ProceduralGenerator::subdivideCloud()
{
	vec3 size = _pointCloudScene->_pointCloud->getAABB().size();
	AABB* aabb = new AABB(_pointCloudScene->_pointCloud->getAABB());
	DrawAABB* test = new DrawAABB(aabb);
	_pointCloudScene->_sceneGroup->addComponent(test);
}
