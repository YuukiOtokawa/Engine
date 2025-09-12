#pragma once
#include "CSVExporter.h"

#include "GUI.h"

#include <string>
#include <vector>

enum ClassID {
    CID_None = 0, // 0は予約済み
    CID_Object = 1,
    CID_Material,
    CID_Component_Transform,
    CID_Component_MeshFilter,
    CID_Component_MeshRenderer,
    CID_Component_SkinnedMeshRenderer,
    CID_Component_RenderTexture,
    CID_Component_InputSystem,
    CID_Component_Light,
    CID_Component_Camera,
    CID_Component_EditorCamera,
    CID_PlayerCamera,
    CID_AssimpMeshRenderer,
    CID_Animation,
    CID_Billboard,
    CID_Particle,
    CID_Script,
};

class EngineMetaFile
{
protected:

    ClassID m_ClassID = CID_None; // ファイルID
    int m_FileID;

    static int m_FileIDCounter; // ファイルIDのカウンター

public:
    EngineMetaFile(ClassID id = CID_None) : m_ClassID(id) {
        m_FileID = m_FileIDCounter++;
    }

    void CreateEngineMetaFile(ClassID classID, int fileID) {
        m_ClassID = classID;
        m_FileID = fileID;
    }

    void Import(std::vector<std::string>& tokens) {
        ImportFile(tokens);
    }

    virtual void ImportFile(std::vector<std::string>& tokens) {
        // デフォルトの実装は何もしない
    }

    virtual void ExportFile() {
        // デフォルトの実装は何もしない
    }

    virtual void AddExportList() {
        // デフォルトの実装は何もしない
    }

    void SetClassID(ClassID id) {
        m_ClassID = id;
    }

    void SetFileID(int id) {
        m_FileID = id;
    }

    ClassID GetClassID() const {
        return m_ClassID;
    }
    int GetFileID() const {
        return m_FileID;
    }
};

