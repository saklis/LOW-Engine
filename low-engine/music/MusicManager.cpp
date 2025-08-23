#include "MusicManager.h"

#include "assets/Assets.h"

namespace LowEngine::Music {
    void MusicManager::Update(float deltaTime) {
        if (!_nextMusic.empty()) {
            if (!_currentMusic.empty()) {
                Assets::GetMusic(_currentMusic).stop();
            }

            Assets::GetMusic(_nextMusic).play();

            _currentMusic = _nextMusic;
            _nextMusic.clear();
        }

        if (!IsMusicPlaying() && !_musicQueue.empty()) {
            PlayNextQueued();
        }
    }

    void MusicManager::PlayMusic(const std::string& musicAlias, float crossFadeTime) {
        if (_currentMusic != musicAlias) {
            if (Assets::MusicExists(musicAlias)) {
                _nextMusic = musicAlias;
            } else {
                _log->error("Music with alias: {} does not exist.", musicAlias);
            }
        }
    }

    void MusicManager::PauseMusic() {
        if (!_currentMusic.empty()) {
            Assets::GetMusic(_currentMusic).pause();
        }
    }

    void MusicManager::StopMusic(float crossFadeTime) {
        if (!_currentMusic.empty()) {
            Assets::GetMusic(_currentMusic).stop();
        }
    }

    sf::Music& MusicManager::GetCurrentMusic() {
        return Assets::GetMusic(_currentMusic);
    }

    std::string MusicManager::GetCurrentMusicAlias() {
        return _currentMusic;
    }

    void MusicManager::AddMusicToQueue(const std::string& musicAlias) {
        if (Assets::MusicExists(musicAlias)) {
            _musicQueue.emplace_back(musicAlias);
        }
    }

    void MusicManager::RemoveMusicFromQueue(size_t index) {
        _musicQueue.erase(_musicQueue.begin() + index);
    }

    void MusicManager::RemoveMusicFromQueue(const std::string& alias) {
           auto removed = std::ranges::remove(_musicQueue, alias);
        _musicQueue.erase(removed.begin(), removed.end());
    }

    void MusicManager::PlayNextQueued() {
        if (!_musicQueue.empty()) {
            _nextMusic = _musicQueue.front();
            _musicQueue.erase(_musicQueue.begin());
        }
    }

    std::vector<std::string> MusicManager::GetQueuedMusic() {
        return std::vector<std::string>(_musicQueue.begin(), _musicQueue.end());
    }

    bool MusicManager::IsMusicPlaying() {
        if (Assets::MusicExists(_currentMusic)) {
            return Assets::GetMusic(_currentMusic).getStatus() == sf::Music::Status::Playing;
        }
        return false;
    }

    bool MusicManager::IsMusicPaused() {
        if (Assets::MusicExists(_currentMusic)) {
            return Assets::GetMusic(_currentMusic).getStatus() == sf::Music::Status::Paused;
        }
        return false;
    }

    void MusicManager::ClearQueuedMusic() {
        _musicQueue.clear();
    }
}
