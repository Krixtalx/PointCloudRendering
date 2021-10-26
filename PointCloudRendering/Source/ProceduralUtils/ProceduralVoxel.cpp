#include "stdafx.h"
#include "ProceduralVoxel.h"


ProceduralVoxel::ProceduralVoxel(PointCloud* pointCloud, AABB* aabb) :pointCloud(pointCloud)
{
	setAABB(aabb);
}

ProceduralVoxel::~ProceduralVoxel()
{
	delete aabb;
	delete drawAABB;
}

void ProceduralVoxel::addPoint(unsigned pointIndex)
{
	pointsIndex.push_back(pointIndex);
}

void ProceduralVoxel::drawAsLines(RenderingShader* shader, const RendEnum::RendShaderTypes shaderType, std::vector<mat4>& matrix)
{
	if (wireframe) {
		if(!drawAABB)
			drawAABB = new DrawAABB(this->aabb);
		drawAABB->drawAsLines(shader, shaderType, matrix);
	}
}

void ProceduralVoxel::setAABB(AABB* aabb)
{
	if (this->aabb)
		delete this->aabb;
	this->aabb = aabb;
	if (drawAABB)
		delete drawAABB;
}

void ProceduralVoxel::setProcedural(bool proc)
{
	this->procedural = proc;
}

void ProceduralVoxel::computeHeight()
{
	std::vector<PointCloud::PointModel>* points = pointCloud->getPoints();
	unsigned size = pointsIndex.size();
	height = 0;
	if (size != 0) {
		for (size_t i = 0; i < size; i++)
		{
			height += (*points)[pointsIndex[i]]._point[2];
		}

		height /= size;
		/*PointCloud::PointModel point;
		point._point = aabb->center();
		point._point[2] = height;
		point.saveRGB(glm::vec3(1, 0, 0));
		points->push_back(point);*/
	}
	else {
		height = FLT_MAX;
	}
}

bool ProceduralVoxel::isInside(PointCloud::PointModel point)
{
	return aabb->isInside(point._point);
}

bool ProceduralVoxel::load(const mat4& modelMatrix)
{
	//this->_loaded = true;

	return true;
}

float ProceduralVoxel::getHeight()
{
	return height;
}
