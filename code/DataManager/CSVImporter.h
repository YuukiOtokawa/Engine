#pragma once

#include <list>
#include "Object.h"
class CSVImporter
{
public:
    static std::list<Object*> Import(std::string filePath);

    static int ImportMesh(std::string filePath);

    static Vector4O ImportVector4O(const std::string& token1, const std::string& token2, const std::string& token3, const std::string& token4);
private:
    static EngineMetaFile* CreateObject(ClassID classID);
};

