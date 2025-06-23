#pragma once

#include <list>
#include "Object.h"
class CSVImporter
{
public:
    static std::list<Object*> Import(const char* filePath);
};

