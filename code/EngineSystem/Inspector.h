#pragma once

#include <string>

class Object;
class MeshFilter;

void AddComponentPopup(Object* object);

void GetMesh(MeshFilter* meshFilter, std::string filePath);

std::string GetOpenFilePath();
