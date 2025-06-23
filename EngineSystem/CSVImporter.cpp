#include "CSVImporter.h"

#include "Component_Transform.h"

#include "fstream"

#include <vector>
#include <sstream>

std::list<Object*> CSVImporter::Import(const char* filePath)
{
	// Open the CSV file
	std::ifstream file(filePath);
	std::list<Object*> output;

	if (!file.is_open()) {
		throw std::runtime_error("Could not open file: " + std::string(filePath));
	}

	// 1行目（ヘッダー）をスキップ
	std::string line;
	if (!std::getline(file, line)) {
		// ファイルが空の場合は何もしない
		return output;
	}

	// 2行目以降を処理
	while (std::getline(file, line)) {
		// Split the line by commas
		std::vector<std::string> tokens;
		std::stringstream ss(line);
		std::string token;
		while (std::getline(ss, token, ',')) {
			tokens.push_back(token);
		}
		// Create a new Object and set its Transform component
		Object* obj = new Object();
		obj->AddComponent<Transform>();
		Transform* transform = obj->GetComponent<Transform>();
		obj->SetName(tokens[0]);
		if (tokens.size() == 1) {
			output.push_back(obj);
			continue;
		}
		transform->SetPosition(Vector4O(
			std::stof(tokens[1]), 
			std::stof(tokens[2]), 
			std::stof(tokens[3])
		));
		transform->SetRotation(Vector4O(
			std::stof(tokens[4]), 
			std::stof(tokens[5]), 
			std::stof(tokens[6])
		));
		transform->SetScale(Vector4O(
			std::stof(tokens[7]), 
			std::stof(tokens[8]), 
			std::stof(tokens[9])
		));
		output.push_back(obj);
	}
	return output;
}
