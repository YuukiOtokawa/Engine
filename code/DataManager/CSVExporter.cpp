#include "CSVExporter.h"

#include "Object.h"

#include "Component_Transform.h"

#include "MainEngine.h"

#include <yaml-cpp/yaml.h>

std::ofstream CSVExporter::m_File;
std::list<EngineMetaFile*> CSVExporter::m_ExportList;
std::list<VertexIndex*> CSVExporter::m_VertexIndicesExportList;

void CSVExporter::Export(std::list<Object*> objects)
{
	for (const auto& obj : objects) {
		obj->AddExportList();
	}

	// Open a file for writing
	auto filePath = OpenExportFileDialog();
	m_File = std::ofstream(filePath);
	if (!m_File.is_open()) {
		m_ExportList.clear();
		return;
	}


	// Write each object's data
	for (const auto& metaFile : m_ExportList) {
		m_File << metaFile->GetClassID() << ","; // Export ClassID
		m_File << metaFile->GetFileID() << ","; // Export FileID
		YAML::Emitter dummyNode; // CSVExporter is deprecated, using dummy YAML::Node
		metaFile->ExportFile(dummyNode); // Call the export function for the specific object
		m_File << "\n"; // New line after each object
	}
	// Close the file
	m_File.close();

	ExportVertexIndexList();
	ExportTextureInfoList();
}

void CSVExporter::ExportInt(int data)
{
	if (!m_File.is_open())
	{
		throw std::runtime_error("File is not open for writing.");
		return;
	}
	m_File << data << ",";
}

void CSVExporter::ExportFloat(float data)
{
	if (!m_File.is_open())
	{
		throw std::runtime_error("File is not open for writing.");
		return;
	}
	m_File << data << ",";
}

void CSVExporter::ExportString(const std::string data)
{
	if (!m_File.is_open())
	{
		throw std::runtime_error("File is not open for writing.");
		return;
	}
	m_File << "" << data << ",";
}

void CSVExporter::ExportVector2O(const Vector2O data)
{
	if (!m_File.is_open())
	{
		throw std::runtime_error("File is not open for writing.");
		return;
	}
	m_File << data.x << "," << data.y << ",";
}

void CSVExporter::ExportVector3O(const Vector3O data)
{
	if (!m_File.is_open())
	{
		throw std::runtime_error("File is not open for writing.");
		return;
	}
	m_File << data.x << "," << data.y << "," << data.z << ",";
}

void CSVExporter::ExportVector4O(const Vector4O data)
{
	if (!m_File.is_open())
	{
		throw std::runtime_error("File is not open for writing.");
		return;
	}
	m_File << data.x << "," << data.y << "," << data.z << "," << data.w << ",";
}

void CSVExporter::ExportFileID(const int fileID)
{
	if (!m_File.is_open())
	{
		throw std::runtime_error("File is not open for writing.");
		return;
	}
	m_File << "&" << fileID << ","; // Export file ID with a prefix
}


#include "VertexIndex.h"
#include <unordered_set>
#include <sstream>

void CSVExporter::ExportVertexIndexList()
{
	auto fileVertexInfoData = std::ifstream("AssetList\\VertexIndexList.csv");
	std::unordered_set<std::string> existing_lines;

	if (fileVertexInfoData.is_open()) {
		std::string line;
		while (std::getline(fileVertexInfoData, line)) {
			std::stringstream ss(line);
			std::string token;
			while (std::getline(ss, token, ',')) {
				existing_lines.insert(token);
				break;
			}
		}
		fileVertexInfoData.close();
	}

	auto fileVertexInfoList = std::ofstream("AssetList\\VertexIndexList.csv", std::ios::out | std::ios::app);
	for (auto vertexIndex : m_VertexIndicesExportList)
	{

		m_File = std::ofstream(vertexIndex->GetFilePath());
		if (!m_File.is_open()) {
			throw std::runtime_error("Could not open file for writing.");
			m_ExportList.clear();
			return;
		}


		// Write vertex data
		for (const auto& vertex : vertexIndex->GetVertexInfo()) {
			m_File << vertex.position.x << "," << vertex.position.y << "," << vertex.position.z << ",";
			m_File << vertex.normal.x << "," << vertex.normal.y << "," << vertex.normal.z << ",";
			m_File << vertex.color.x << "," << vertex.color.y << "," << vertex.color.z << "," << vertex.color.w << ",";
			m_File << vertex.texcoord.x << "," << vertex.texcoord.y << "\n";
		}

		m_File << "&\n";  // Separator between vertex data and index data

		// Write index data

		for (const auto& index : vertexIndex->GetIndexInfo()) {
			m_File << index;
			if (&index != &vertexIndex->GetIndexInfo().back()) {
				m_File << ",";
			}
		}
		m_File << "\n";
		m_File << std::to_string(vertexIndex->GetPrimitiveTopology());


		// Close the file
		m_File.close();

		if (existing_lines.find(std::to_string(vertexIndex->GetFileID())) != existing_lines.end())
			continue; // Skip if this FileID already exists

		fileVertexInfoList << vertexIndex->GetFileID() << "," << vertexIndex->GetName() << "," << vertexIndex->GetFilePath() << "\n";
	}

	fileVertexInfoList.close();
}

void CSVExporter::ExportTextureInfoList()
{
	auto fileTextureData = std::ifstream("AssetList\\VertexIndexList.csv");
	std::unordered_set<std::string> existing_lines;

	if (fileTextureData.is_open()) {
		std::string line;
		while (std::getline(fileTextureData, line)) {
			std::stringstream ss(line);
			std::string token;
			while (std::getline(ss, token, ',')) {
				existing_lines.insert(token);
				break;
			}
		}
		fileTextureData.close();
	}

	auto textureList = MainEngine::GetInstance()->GetRenderCore()->GetTextureInfo();

	auto fileTextureInfoList = std::ofstream("AssetList\\TextureList.csv");
	for (const auto& texture : textureList)
	{
		if (texture->GetFileID() == 0) continue;
		const WCHAR* filename = texture->filename.c_str();
		int len = WideCharToMultiByte(CP_UTF8, 0, filename, -1, nullptr, 0, nullptr, nullptr);
		std::string narrowStringFilePath;
		if (len > 0) {
			narrowStringFilePath.resize(len - 1); // null終端を除く
			WideCharToMultiByte(CP_UTF8, 0, filename, -1, &narrowStringFilePath[0], len, nullptr, nullptr);
		}

		if (existing_lines.find(std::to_string(texture->GetFileID())) != existing_lines.end())
			continue; // Skip if this FileID already exists

		if (texture->toExport)
			fileTextureInfoList << texture->GetFileID() << "," << narrowStringFilePath << "\n";
	}
}



