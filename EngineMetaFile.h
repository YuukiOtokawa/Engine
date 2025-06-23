#pragma once
#include "CSVExporter.h"

class EngineMetaFile
{
private:
    int m_FileID = 0; // ファイルID

public:
    void Export() {
        if (m_FileID == 0) {
            throw std::runtime_error("Component ID is not initialized.");
        }
        CSVExporter::ExportInt(m_FileID);
        ExportFile();
    }

    virtual void ExportFile() {
        // デフォルトの実装は何もしない
    }

    void SetFileID(int id) {
        m_FileID = id;
    }
};

