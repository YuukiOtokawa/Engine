#include "CSVExporter.h"

#include "Object.h"

#include "Component_Transform.h"

std::ofstream CSVExporter::m_File;
std::list<EngineMetaFile*> CSVExporter::m_ExportList;
std::list<VertexIndex*> CSVExporter::m_VertexIndicesExportList;

void CSVExporter::Export(std::list<Object*> objects)
{
	for (const auto& obj : objects) {
		obj->AddExportList();
	}

	// Open a file for writing
	m_File = std::ofstream("exported_objects.csv");
	if (!m_File.is_open()) {
		throw std::runtime_error("Could not open file for writing.");
		m_ExportList.clear();
		return;
	}


	// Write each object's data
	for (const auto& metaFile : m_ExportList) {
		m_File << metaFile->GetClassID() << ","; // Export ClassID
		m_File << metaFile->GetFileID() << ","; // Export FileID
		metaFile->ExportFile(); // Call the export function for the specific object
		m_File << "\n"; // New line after each object
	}
	// Close the file
	m_File.close();
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


#include "../VertexIndex.h"

void CSVExporter::ExportVertexIndexList()
{
	auto fileVertexInfoList = std::ofstream("VertexIndexInfo\\VertexIndexList.csv");
	for (auto vertexIndex : m_VertexIndicesExportList)
	{
		std::string fileName = "VertexIndexInfo\\" + vertexIndex->GetName() + ".csv";

		m_File = std::ofstream(fileName);
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


		// Close the file
		m_File.close();

		fileVertexInfoList << vertexIndex->GetFileID() << "," << fileName << "," << vertexIndex->GetFilePath() << "\n";
	}

	fileVertexInfoList.close();
}



