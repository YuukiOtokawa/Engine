#pragma once
#include "Component.h"

#include "Material.h"

class MeshRenderer :
    public Component
{
private:
	Material* m_pMaterial = nullptr;

public:
	MeshRenderer();
	~MeshRenderer() = default;
    void Update() override;
	void Draw() override;

	void SetMaterial(MATERIAL material) { this->m_pMaterial->SetMaterial(material); }
	void SetLight(LIGHT light) { 
		this->m_pMaterial->SetLight(light);
	}

	void SetTexture(ID3D11ShaderResourceView* texture);

	void InitializeTag() override;
};

