#include "CSVExporter.h"

#include "Object.h"

#include "Component_Transform.h"

std::ofstream CSVExporter::m_File;

void CSVExporter::Export(std::list<Object*> objects)
{
	// Open a file for writing
	m_File = std::ofstream("exported_objects.csv");
	if (!m_File.is_open()) {
		throw std::runtime_error("Could not open file for writing.");
		return;
	}


	// Write header
	m_File << "Name,ObjectTag,Components...\n";
	// Write each object's data
	for (const auto& obj : objects) {
		m_File << obj->GetName() << ",";
		obj->ExportFile();
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
	m_File << "\"" << data << "\",";
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



