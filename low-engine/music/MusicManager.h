#pragma once
#include <string>

#include "SFML/Audio/Music.hpp"

namespace LowEngine::Music {
    /**
     * @brief High-level controller for background music playback.
     *
     * MusicManager orchestrates track playback using string aliases, providing a simple API
     * to play, pause, stop, and queue music. It is designed to be ticked every frame via Update(),
     * which finalizes any pending track switches and automatically advances the queue when the
     * current track finishes.
     *
     * Key features:
     * - Alias-based control: select tracks by their registered aliases.
     * - Playback control: play, pause, stop, and query current status.
     * - Queue management: enqueue multiple tracks and advance automatically.
     * - Seamless switching: schedule the next track to start on the following update.
     *
     * Usage notes:
     * - Call Update(deltaTime) once per frame to process transitions and queue advancement.
     * - Tracks referenced by alias should be registered in the asset system before use.
     * - The manager does not own a rendering or game loop; it only coordinates playback state.
     */
    class MusicManager {
    public:
        /**
         * @brief Construct a MusicManager.
         *
         * Initializes an instance with no active or pending music and an empty queue.
         */

        MusicManager() = default;

        /**
         * @brief Advance the internal state of the music manager.
         *
         * Should be called once per frame. Applies pending track changes and
         * automatically starts the next queued track when the current one finishes.
         *
         * @param deltaTime Time elapsed since the last update, in seconds.
         */
        void Update(float deltaTime);

        /**
         * @brief Request playback of a music track by alias.
         *
         * If the requested track differs from the current one and exists, it is scheduled
         * to start playing. If a track is currently playing, switching may be immediate
         * or crossfaded based on the provided time.
         *
         * @param musicAlias Alias of the track to play.
         * @param crossFadeTime Optional crossfade duration in seconds. If 0, the switch is immediate.
         */
        void PlayMusic(const std::string& musicAlias, float crossFadeTime = 0.0f);

        /**
         * @brief Pause the currently playing track.
         *
         * Has no effect if no track is active.
         */
        void PauseMusic();

        /**
         * @brief Stop the currently playing track.
         *
         * Stops playback. If a crossfade time is provided, the transition may be smoothed.
         *
         * @param crossFadeTime Optional crossfade duration in seconds. If 0, the stop is immediate.
         */
        void StopMusic(float crossFadeTime = 0.0f);

        /**
         * @brief Access the currently active music instance.
         *
         * Returns a reference to the active music object. Use only when a valid
         * current track is set.
         *
         * @return Reference to the current sf::Music.
         */
        sf::Music& GetCurrentMusic();

        /**
         * @brief Get the alias of the currently active track.
         *
         * @return Alias of the current track, or an empty string if none is active.
         */
        std::string GetCurrentMusicAlias();

        /**
         * @brief Enqueue a music track to be played after the current one finishes.
         *
         * If the alias is valid, it is added to the end of the queue.
         *
         * @param musicAlias Alias of the track to enqueue.
         */
        void AddMusicToQueue(const std::string& musicAlias);

        /**
         * @brief Remove a queued track by index.
         *
         * @param index Zero-based index into the queue.
         */
        void RemoveMusicFromQueue(size_t index);

        /**
         * @brief Remove all occurrences of a queued track by alias.
         *
         * @param alias Alias of the track to remove from the queue.
         */
        void RemoveMusicFromQueue(const std::string& alias);

        /**
         * @brief Start playback of the next track in the queue.
         *
         * If the queue is not empty, schedules the first queued track for playback
         * and removes it from the queue.
         */
        void PlayNextQueued();

        /**
         * @brief Get a snapshot of the queued track aliases.
         *
         * @return Vector of aliases in the current queue order.
         */
        std::vector<std::string> GetQueuedMusic();

        /**
         * @brief Check if a track is currently playing.
         *
         * @return true if the active track is in the Playing state, false otherwise.
         */
        bool IsMusicPlaying();

        /**
         * @brief Check if the current track is paused.
         *
         * @return true if the active track is in the Paused state, false otherwise.
         */
        bool IsMusicPaused();

        /**
         * @brief Clear all tracks from the queue without affecting current playback.
         */
        void ClearQueuedMusic();

    protected:
        std::string _currentMusic;
        std::string _nextMusic;

        std::vector<std::string> _musicQueue;
    };
}
