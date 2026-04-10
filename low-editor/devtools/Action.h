#pragma once
#include <cstdint>
#include <unordered_map>

#include "SFML/Window/Mouse.hpp"

namespace LowEngine {
    /**
     * @brief Mouse button roles used by the editor input system.
     */
    enum class MouseAction : std::uint8_t {
        Primary,    /**< Left mouse button. */
        Secondary,  /**< Right mouse button. */
        Tertiary    /**< Middle mouse button. */
    };

    enum class MouseScrollAction : std::uint8_t {
        Up,
        Down
    };

    /**
     * @brief State of a single mouse button action for the current frame.
     */
    struct Mouse {
        bool Started = false;              /**< True if the button was pressed this frame. */
        bool Pressed = false;              /**< True if the button is pressed. */
        bool Stopped = false;              /**< True if the button was released this frame. */
        sf::Vector2i MouseScreenPosition;   /**< Screen-space position of the cursor at press time. */
    };

    struct Key {
        bool Started = false;              /**< True if the button was pressed this frame. */
        bool Pressed = false;              /**< True if the button is pressed. */
        bool Stopped = false;              /**< True if the button was released this frame. */
    };

    /**
     * @brief Singleton that tracks editor mouse button state for one frame at a time.
     *
     * ReadInput sets actions as active when mouse buttons are pressed outside ImGui panels.
     * ClearActions resets all actions at the end of each frame (called by DevTools::Build).
     */
    class EditorAction {
    public:
        /**
         * @brief Returns a pointer to the state of the given mouse action.
         * @param action The mouse button role to query.
         * @return Pointer to the corresponding Mouse state. Never null.
         */
        static Mouse* Action(MouseAction action);

        static Mouse* Action(MouseScrollAction action);

        /**
         * @brief Resets all mouse actions to inactive. Call once per frame after processing input.
         */
        static void ClearActions();

    protected:
        /**
         * @brief Returns the singleton instance, creating it on first call.
         */
        static EditorAction* GetInstance() {
            static EditorAction instance;
            return &instance;
        }

        /**
         * @brief Initializes the action map with default (inactive) entries for all mouse actions.
         */
        EditorAction();

        std::unordered_map<MouseAction, Mouse> _mouseActions;
        std::unordered_map<MouseScrollAction, Mouse> _mouseScrollActions;
    };
}