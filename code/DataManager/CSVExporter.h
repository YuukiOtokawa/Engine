#pragma once

#include <list>
#include <fstream>

#include "EngineAPI.h"

class EngineMetaFile;
class Object;
struct Vector2O;
struct Vector3O;
struct Vector4O;
class VertexIndex;
class OTOKAWA_API CSVExporter
{
private:
    static std::ofstream m_File;
    static std::list<EngineMetaFile*> m_ExportList;
    static std::list<VertexIndex*> m_VertexIndicesExportList;
public:
    static void Export(std::list<Object*> objects);

    static void ExportInt(int data);
    static void ExportFloat(float data);
    static void ExportString(const std::string data);
    static void ExportVector2O(const Vector2O data);
    static void ExportVector3O(const Vector3O data);
    static void ExportVector4O(const Vector4O data);
    static void ExportFileID(const int fileID);

    static void AddExportList(EngineMetaFile* metaFile) {
        m_ExportList.push_back(metaFile);
    }

    static void ExportVertexIndexList();

    static void AddVertexIndexExportList(VertexIndex* vertexIndex) {
        m_VertexIndicesExportList.push_back(vertexIndex);
    }

    static void ExportTextureInfoList();
};

