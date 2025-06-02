#pragma once
#include "Component_MeshFilter.h"
class CubeMesh :
    public MeshFilter
{
private:
    static const auto NUM_CUBE_VERTEX = 24;
	static const auto NUM_CUBE_INDEX = 36;
public:
    CubeMesh();
	void DrawGUI() override;
};

