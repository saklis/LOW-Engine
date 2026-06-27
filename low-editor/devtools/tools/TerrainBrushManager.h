#pragma once

#include <filesystem>
#include <unordered_map>
#include <vector>

#include "TerrainBrush.h"

namespace LowEngine::Tools {
	class TerrainBrushManager {
	public:
		void AddBrush(const std::string& layerId, const TerrainBrush& brush);
		bool DeleteBrush(const std::string& layerId, std::size_t index);

		std::vector<TerrainBrush>* GetBrushes(const std::string& layerId);
		TerrainBrush* GetBrush(const std::string& layerId, std::size_t index);

		void RemoveLayer(const std::string& layerId);
		void Clear();

		bool Load(const std::filesystem::path& projectDirectory);
		bool Save(const std::filesystem::path& projectDirectory);

	protected:
		std::unordered_map<std::string, std::vector<TerrainBrush>> _brushes;
	};
}