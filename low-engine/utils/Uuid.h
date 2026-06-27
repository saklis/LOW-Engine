#pragma once

#include <array>
#include <cstdint>
#include <iomanip>
#include <random>
#include <string>
#include <sstream>

namespace LowEngine::Utils {
	/**
	 * @brief Generate a random UUID version 4 string.
	 *
	 * The returned value uses the canonical 36-character UUID text format:
	 * xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx, where the version and variant bits
	 * are set according to the UUID v4 layout. This is intended for stable
	 * project/editor identifiers and is not a cryptographic random token.
	 *
	 * @return A newly generated UUID v4 string.
	 */
	inline std::string GetNewUuidV4() {
		static thread_local std::mt19937_64 rng(std::random_device{}());

		std::array<std::uint8_t, 16> bytes{};
		for (auto& byte : bytes) {
			byte = static_cast<std::uint8_t>(rng() & 0xFF);
		}

		bytes[6] = static_cast<std::uint8_t>((bytes[6] & 0x0F) | 0x40); // version 4
		bytes[8] = static_cast<std::uint8_t>((bytes[8] & 0x3F) | 0x80); // variant

		std::ostringstream out;
		out << std::hex << std::setfill('0');

		for (std::size_t i = 0; i < bytes.size(); i++) {
			if (i == 4 || i == 6 || i == 8 || i == 10) {
				out << '-';
			}
			out << std::setw(2) << static_cast<int>(bytes[i]);
		}

		return out.str();
	}
}
