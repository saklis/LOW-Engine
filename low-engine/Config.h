#pragma once

#include <limits>
#include <string>

namespace LowEngine {
    /* * Config class
     * This class contains the configuration settings for the LowEngine.
     */
    class Config {
    public:
        /**
         * @brief Default count of the component in the pool for each scene.
         *
         * This value is used to reserve the Component Pool for each scene.
         * Component Pool will automatically expand during the scene's lifetime.
         */
        inline static const std::size_t DEFAULT_COMPONENT_POOL_SIZE = 1000;

        /**
         * @brief Name of the logger used in the engine.
         */
        inline static const char* LOGGER_NAME = "low_engine_spdlog_logger_name";

        /**
         * @brief Maximum value for size_t.
         *
         * This value is used as "null" value for size_t.
         */
        inline static const unsigned int MAX_SIZE = std::numeric_limits<std::size_t>::max();

        /**
         * @brief Default alias for the texture that is used when no texture is specified.
         *
         * This alias is used to refer to a default texture that can be used in various components
         * when no specific texture is provided.
		 */
    	inline static const std::string DEFAULT_TEXTURE_ALIAS = "default";

        /**
         * @brief Default alias for the sound that is used when no sound is specified.
         *
         * This alias is used to refer to a default sound that can be used in various components
         * when no specific sound is provided.
		 */
		inline static const std::string DEFAULT_SOUND_ALIAS = "default";

        /**
         * @brief Default alias for the font that is used when no font is specified.
         *
         * This alias is used to refer to a default font that can be used in various components
         * when no specific font is provided.
		 */
		inline static const std::string DEFAULT_FONT_ALIAS = "default";

        /**
         * @brief Default file extension for project file.
         */
        inline static const std::string PROJECT_FILE_EXTENSION = ".lowproj";

        /**
         * @brief Default name for the assets folder.
         */
        inline static const std::string ASSETS_FOLDER_NAME = "assets";

        /**
         * @brief Default name for the texture assets directory.
         */
        inline static const std::string TEXTURES_FOLDER_NAME = "textures";

    	/**
		 * @brief Default name for the sound assets directory.
		 */
		inline static const std::string SOUNDS_FOLDER_NAME = "sounds";

    	/**
		 * @brief Default name for the music assets directory.
		 */
    	inline static const std::string MUSIC_FOLDER_NAME = "music";
    };
}
