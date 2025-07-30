#pragma once
#include <string>
#include <stdexcept>

namespace LowEngine::Terrain {
	/**
	 * @brief Type of the layer.
	 */
	enum class LayerType {
		Empty,
		Terrain,
		Features,
	};

	/**
	 * @brief Convert LayerType enum to string representation.
	 * @param type The LayerType enum value to convert.
	 * @return String representation of the LayerType.
	 */
	inline std::string ToString(LayerType type) {
		switch (type) {
			case LayerType::Empty: return "Empty";
			case LayerType::Terrain: return "Terrain";
			case LayerType::Features: return "Features";
			default: throw std::invalid_argument("Unknown LayerType enum value");
			
		}
	}

	/**
	 * @brief Convert string to LayerType enum.
	 * @param str The string to convert to LayerType.
	 * @return LayerType enum value. Returns LayerType::Empty for unknown strings.
	 */
	inline LayerType Parse(const std::string& str) {
		if (str == "Empty") return LayerType::Empty;
		if (str == "Terrain") return LayerType::Terrain;
		if (str == "Features") return LayerType::Features;
		throw std::invalid_argument("Unknown LayerType string value: " + str);
	}
}