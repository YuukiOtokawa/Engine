#pragma once

#include <list>
#include <fstream>

class EngineMetaFile;
class Object;
class Vector4O;
class CSVExporter
{
private:
    static std::ofstream m_File;
    static std::list<EngineMetaFile*> m_ExportList;
public:
    static void Export(std::list<Object*> objects);

    static void ExportInt(int data);
    static void ExportFloat(float data);
    static void ExportString(const std::string data);
    static void ExportVector4O(const Vector4O data);
    static void ExportFileID(const int fileID);

    static void AddExportList(EngineMetaFile* metaFile) {
        m_ExportList.push_back(metaFile);
    }

};

