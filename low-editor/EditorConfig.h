#pragma once

#include <string>

namespace LowEditor {
    /**
     * @brief Central location for editor-wide configuration constants.
     *
     * These values are used by project creation and scene-management code to keep
     * user-facing names and editor filesystem paths consistent.
     */
    class Config {
    public:
        /**
         * @brief Framerate cap used when opening the editor window.
         *
         * The editor executable passes this value to LowEngine::Game::OpenWindow(),
         * which applies it to the SFML render window.
         */
        inline static const unsigned int EDITOR_FRAMERATE_LIMIT = 60;

        /**
         * @brief Default display name assigned to newly created projects.
         *
         * If unchanged in the project wizard, this name becomes the project folder,
         * project file basename, and persisted project title.
         */
        inline static const std::string DEFAULT_NEW_PROJECT_NAME = "New Project";

        /**
         * @brief Relative default directory used by editor project workflows.
         *
         * New-project and save-menu flows use this as the default project root, but
         * loaded projects can live elsewhere because the project directory is derived
         * from the loaded project file path.
         */
        inline static const std::string PROJECT_DIRECTORY = "projects";

        /**
         * @brief Suffix appended to temporary play-mode scene names shown in the editor.
         */
        inline static const std::string TEMPORARY_SCENE_SUFFIX = " (TEMPORARY)";

        /**
         * @brief Project-local folder name for editor-owned data.
         *
         * Terrain brush persistence combines this with TERRAIN_FOLDER_NAME and
         * TERRAIN_BRUSHES_FILE_NAME under the active project directory.
         */
        inline static const std::string EDITOR_FOLDER_NAME = "editor";

        /**
         * @brief Folder name for terrain editor data inside the editor data folder.
         *
         * Terrain brushes are loaded from and saved to
         * <projectDirectory>/editor/terrain/brushes.json.
         */
        inline static const std::string TERRAIN_FOLDER_NAME = "terrain";

        /**
         * @brief File name used to persist terrain brush definitions.
         *
         * TerrainBrushManager appends this file name to the project-local editor
         * terrain directory when loading and saving brush data.
         */
        inline static const std::string TERRAIN_BRUSHES_FILE_NAME = "brushes.json";
    };
}
