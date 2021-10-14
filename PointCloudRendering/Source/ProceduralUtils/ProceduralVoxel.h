#pragma once
#include <Geometry/3D/AABB.h>
#include <Graphics/Core/DrawAABB.h>
#include <Graphics/Core/PointCloud.h>
class ProceduralVoxel : public Model3D {
private:
	AABB* aabb;
	DrawAABB* drawAABB;
	PointCloud* pointCloud;

public:

	ProceduralVoxel(AABB* aabb);

	~ProceduralVoxel();

	void addPoint(PointCloud::PointModel point);

	void drawAsLines(RenderingShader* shader, const RendEnum::RendShaderTypes shaderType, std::vector<mat4>& matrix);

	void setAABB(AABB* aabb);

	bool load(const mat4& modelMatrix);

};

