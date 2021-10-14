#pragma once
#include "Utilities/Singleton.h"
#include <Graphics/Application/PointCloudScene.h>
#include <ProceduralUtils/ProceduralVoxel.h>
class ProceduralGenerator: public Singleton<ProceduralGenerator>{

	friend class Singleton<ProceduralGenerator>;

private:
	PointCloudScene* _pointCloudScene = nullptr;
	unsigned axisSubdivision[3];
	std::vector<std::vector<std::vector<ProceduralVoxel*>>> subdivisions;
	int expansion;
	float cloudDensity;

protected:
	ProceduralGenerator() = default;
	void calculateCloudDensity();
	void readParameters(const std::string & path);
	void subdivideCloud();

public:
	~ProceduralGenerator();
	void setCurrentCloudScene(PointCloudScene* pointCloudScene);
	

};