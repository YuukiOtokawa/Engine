#pragma once

#include <list>
#include <fstream>

#include "EngineAPI.h"
#include "yaml.h"

class EngineMetaFile;
class Object;
struct Vector2O;
struct Vector3O;
struct Vector4O;
class VertexIndex;

class OTOKAWA_API SceneExporter
{
private:
    static std::list<EngineMetaFile*> m_ExportList;
    static std::list<VertexIndex*> m_VertexIndicesExportList;

public:
    static void Export(std::list<Object*> objects);

    static void AddExportList(EngineMetaFile* metaFile) {
        m_ExportList.push_back(metaFile);
    }

    static void ExportVertexIndexList();

    static void AddVertexIndexExportList(VertexIndex* vertexIndex) {
        m_VertexIndicesExportList.push_back(vertexIndex);
    }

    static void ExportTextureInfoList();

    static int CheckTextureFileNameExist(std::string path);
};
