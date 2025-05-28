#pragma once

#include <d3d11.h>

#include "FrameWork.h"
class Material
{
private:

	std::string m_VertexShader;
	std::string m_PixelShader;

	MATERIAL m_Material;

	LIGHT m_Light;

	ID3D11ShaderResourceView* m_Texture;

public:
	Material() = default;
	~Material() = default;

	void DrawMaterial();

	void SetVertexShaderKey(std::string key);
	void SetPixelShaderKey(std::string key);

	void SetMaterial(MATERIAL material) { m_Material = material; }

	void SetLight(LIGHT light) { m_Light = light; }

	void SetTexture(ID3D11ShaderResourceView* texture) { m_Texture = texture; }

	std::string GetVertexShader() const { return m_VertexShader; }
	std::string GetPixelShader() const { return m_PixelShader; }

	MATERIAL GetMaterial() const { return m_Material; }
	LIGHT GetLight() const { return m_Light; }
	ID3D11ShaderResourceView* GetTexture() const { return m_Texture; }
};

