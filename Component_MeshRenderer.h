#pragma once
#include "Component.h"

#include "Material.h"

class MeshRenderer :
    public Component
{
private:
	Material* m_pMaterial = nullptr;
	int m_MaterialNum = 1;

public:
	MeshRenderer();
	~MeshRenderer() = default;
    void Update() override;
	void Draw() override;

	void SetMaterial(MATERIAL material) { this->m_pMaterial->SetMaterial(material); }
	void SetLight(LIGHT light) { 
		this->m_pMaterial->SetLight(light);
	}

	void SetMaterial(Material* material) { this->m_pMaterial = material; }
	void SetMaterialNum(int num) { this->m_MaterialNum = num; }

	void SetVertexShader(std::string key) {
		this->m_pMaterial->SetVertexShaderKey(key);
	}
	void SetPixelShader(std::string key) {
		this->m_pMaterial->SetPixelShaderKey(key);
	}

	void SetTexture(ID3D11ShaderResourceView* texture) { m_pMaterial->SetTexture(texture); }

	void InitializeTag() override;
};

