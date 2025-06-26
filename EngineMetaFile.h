#pragma once
#include "CSVExporter.h"

#include "GUI.h"

class EngineMetaFile
{
protected:
    enum ClassID {
        CID_None = 0, // 0は予約済み
        CID_Object = 1,
        CID_Material,
        CID_Transform,
        CID_MeshFilter,
        CID_MeshRenderer,
        CID_InputSystem,
        CID_Camera,
        CID_EditorCamera,
        CID_PlayerCamera,
        CID_AssimpMeshRenderer,
        CID_MonoBehaviour,
        CID_PlaneMesh,
        CID_Animation,
        CID_SpriteMesh,
        CID_Billboard,
        CID_Particle,
        CID_CubeMesh,
    };

    ClassID m_ClassID = CID_None; // ファイルID
    int m_FileID;

public:
    EngineMetaFile(ClassID id = CID_None) : m_ClassID(id) {}
    void Export() {
        if (m_ClassID == CID_None) {
            throw std::runtime_error("Component ID is not initialized.");
        }
        CSVExporter::ExportInt((int)m_ClassID);
        ExportFile();
    }

    virtual void ExportFile() {
        // デフォルトの実装は何もしない
    }

    void SetFileID(ClassID id) {
        m_ClassID = id;
    }
};

