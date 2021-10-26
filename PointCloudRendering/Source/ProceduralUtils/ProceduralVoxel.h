#pragma once
#include <Geometry/3D/AABB.h>
#include <Graphics/Core/DrawAABB.h>
#include <Graphics/Core/PointCloud.h>
class ProceduralVoxel{
private:
	AABB* aabb;
	DrawAABB* drawAABB;
	PointCloud* pointCloud;
	std::vector<unsigned> pointsIndex;
	bool procedural = true;
	static const bool wireframe = false;
	float height;

public:

	ProceduralVoxel(PointCloud* pointCloud, AABB* aabb);

	~ProceduralVoxel();

	void addPoint(unsigned pointIndex);

	void drawAsLines(RenderingShader* shader, const RendEnum::RendShaderTypes shaderType, std::vector<mat4>& matrix);

	void setAABB(AABB* aabb);

	void setProcedural(bool proc);

	void computeHeight();

	bool isInside(PointCloud::PointModel point);

	bool load(const mat4& modelMatrix);

	float getHeight();
};

