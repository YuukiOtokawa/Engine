#pragma once

#include "FrameWork.h"

#include "Editor.h"

class VertexIndex : public EngineMetaFile {
    std::vector<VERTEX> m_VertexInfo;
    std::vector<unsigned int> m_IndexInfo;
    std::string m_Name;
    std::string m_FilePath;
public:
    VertexIndex(std::string name, std::vector<VERTEX> vertexInfo, std::vector<unsigned int> indexInfo)
        : m_Name(name), m_VertexInfo(vertexInfo), m_IndexInfo(indexInfo) {
        m_FilePath = "VertexIndexInfo\\" + name + ".csv";
    }

    std::vector<VERTEX> GetVertexInfo() const {
        return m_VertexInfo;
    }
    std::vector<unsigned int> GetIndexInfo() const {
        return m_IndexInfo;
    }
    std::string GetName() const {
        return m_Name;
    }
    void SetFilePath(std::string filePath) {
        m_FilePath = filePath;
    }
    std::string GetFilePath() const {
        return m_FilePath;
    }
};
