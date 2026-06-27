#include "TerrainBrushManager.h"

#include <cstddef>
#include <fstream>
#include <nlohmann/json.hpp>

#include "EditorConfig.h"
#include "log/Log.h"

namespace LowEngine::Tools {
	void TerrainBrushManager::AddBrush(const std::string& layerId, const TerrainBrush& brush) {
		_brushes[layerId].emplace_back(brush);
	}

	bool TerrainBrushManager::DeleteBrush(const std::string& layerId, std::size_t index) {
		auto layerIt = _brushes.find(layerId);
		if (layerIt == _brushes.end()) {
			return false;
		}

		auto& brushes = layerIt->second;
		if (index >= brushes.size()) {
			return false;
		}

		brushes.erase(brushes.begin() + static_cast<std::ptrdiff_t>(index));
		if (brushes.empty()) {
			_brushes.erase(layerIt);
		}

		return true;
	}

	std::vector<TerrainBrush>* TerrainBrushManager::GetBrushes(const std::string& layerId) {
		auto layerIt = _brushes.find(layerId);
		if (layerIt == _brushes.end()) {
			return nullptr;
		}

		return &layerIt->second;
	}

	TerrainBrush* TerrainBrushManager::GetBrush(const std::string& layerId, std::size_t index) {
		auto layerIt = _brushes.find(layerId);
		if (layerIt == _brushes.end()) {
			return nullptr;
		}

		auto& brushes = layerIt->second;
		if (index >= brushes.size()) {
			return nullptr;
		}

		return &brushes[index];
	}

	void TerrainBrushManager::RemoveLayer(const std::string& layerId) {
		_brushes.erase(layerId);
	}

	void TerrainBrushManager::Clear() {
		_brushes.clear();
	}

	bool TerrainBrushManager::Load(const std::filesystem::path& projectDirectory) {
		auto filePath = projectDirectory / LowEditor::Config::EDITOR_FOLDER_NAME /
			LowEditor::Config::TERRAIN_FOLDER_NAME / LowEditor::Config::TERRAIN_BRUSHES_FILE_NAME;

		std::ifstream file(filePath);
		if (!file.is_open()) {
			_log->warn("Failed to open terrain brushes file for loading: {}", filePath.string());
			return true;
		}

		nlohmann::ordered_json root;
		file >> root;
		file.close();

		if (!root.contains("layers") || !root["layers"].is_object()) {
			_log->error("Terrain brush file has wrong structure: {}", filePath.string());
			return false;
		}

		for (auto& [layerId, brushesJson] : root["layers"].items()) {
			if (brushesJson.is_array()) {
				for (auto& brushJson : brushesJson) {
					TerrainBrush brush;
					if (brush.DeserializeFromJSON(brushJson)) {
						AddBrush(layerId, brush);
					} else {
						_log->error("Unable to deserialize one of the brushes for layer '{}': {}", layerId,
						            filePath.string());
					}
				}
			} else {
				_log->error("Terrain brushes for layer '{}' is not an array: {}", layerId, filePath.string());
				return false;
			}
		}

		return true;
	}

	bool TerrainBrushManager::Save(const std::filesystem::path& projectDirectory) {
		auto dir = projectDirectory / LowEditor::Config::EDITOR_FOLDER_NAME / LowEditor::Config::TERRAIN_FOLDER_NAME;
		std::filesystem::create_directories(dir);
		auto filePath = dir / LowEditor::Config::TERRAIN_BRUSHES_FILE_NAME;

		nlohmann::ordered_json root;
		root["layers"] = nlohmann::ordered_json::object();
		for (auto& [layerId, brushes] : _brushes) {
			nlohmann::ordered_json brushesJson = nlohmann::ordered_json::array();
			for (auto& brush : brushes) {
				brushesJson.emplace_back(brush.SerializeToJSON());
			}

			root["layers"][layerId] = brushesJson;
		}

		std::ofstream file(filePath);
		if (!file.is_open()) {
			_log->error("Failed to open terrain brushes file for saving: {}", filePath.string());
			return false;
		}

		file << root.dump(4);
		file.close();
		if (file.fail()) {
			_log->error("Failed to write terrain brushes file: {}", filePath.string());
			return false;
		}

		_log->info("Terrain brushes saved to file: {}", filePath.string());
		return true;
	}
}
