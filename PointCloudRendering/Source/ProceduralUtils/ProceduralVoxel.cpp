#include "stdafx.h"
#include "ProceduralVoxel.h"


ProceduralVoxel::ProceduralVoxel(AABB* aabb)
{
	pointCloud = new PointCloud();
	setAABB(aabb);
}

ProceduralVoxel::~ProceduralVoxel()
{
	delete aabb;
	delete drawAABB;
	delete pointCloud;
}

void ProceduralVoxel::addPoint(PointCloud::PointModel point)
{
	if (pointCloud)
		pointCloud->getPoints()->push_back(point);
}

void ProceduralVoxel::drawAsLines(RenderingShader* shader, const RendEnum::RendShaderTypes shaderType, std::vector<mat4>& matrix)
{
	if (drawAABB)
		drawAABB->drawAsLines(shader, shaderType, matrix);
}

void ProceduralVoxel::setAABB(AABB* aabb)
{
	if (this->aabb)
		delete this->aabb;
	this->aabb = aabb;
	if (drawAABB)
		delete drawAABB;
	drawAABB = new DrawAABB(this->aabb);
}

bool ProceduralVoxel::load(const mat4& modelMatrix)
{
	this->_loaded = true;

	return true;
}