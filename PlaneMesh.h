#pragma once
#include "Component_MeshFilter.h"
class PlaneMesh :
    public MeshFilter
{
private:
	static const int NUM_PLANE_VERTEX = 4;
	static const int NUM_PLANE_INDEX = 4;

public:
	PlaneMesh();
	void DrawGUI() override;
};

