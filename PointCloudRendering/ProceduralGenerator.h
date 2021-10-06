#pragma once
#include "Utilities/Singleton.h"
#include <Graphics/Application/PointCloudScene.h>
class ProceduralGenerator: public Singleton<ProceduralGenerator>{

	friend class Singleton<ProceduralGenerator>;

private:
	PointCloudScene* _pointCloudScene = nullptr;
	unsigned axisSubdivision[3];
	float cloudDensity;

public:
	void setCurrentCloudScene(PointCloudScene* pointCloudScene);
	
protected:
	//ProceduralGenerator();
	void calculateCloudDensity();
	void readParameters(const std::string& path);
	void subdivideCloud();
};