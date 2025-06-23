#pragma once

#include <list>
#include <fstream>

class Object;
class Vector4O;
class CSVExporter
{
private:
    static std::ofstream m_File;
public:
    static void Export(std::list<Object*> objects);

    static void ExportInt(int data);
    static void ExportFloat(float data);
    static void ExportString(const std::string data);
    static void ExportVector4O(const Vector4O data);

};

