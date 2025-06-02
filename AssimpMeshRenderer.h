#pragma once
#include "Component_MeshRenderer.h"

#include <unordered_map>

class AssimpMeshRenderer :
    public MeshRenderer
{
private:
    std::unordered_map<std::string, ID3D11ShaderResourceView*> m_AssimpTexture;

public:
	AssimpMeshRenderer() = default;
    void SetMesh(std::unordered_map<std::string, ID3D11ShaderResourceView*> assimpTexture) {
		m_AssimpTexture = assimpTexture;
    }

    void DrawGUI() override;

};

