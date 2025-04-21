#include "Layer.h"

#include "Config.h"
#include "assets/Assets.h"
#include "SFML/System/Vector2.hpp"

namespace LowEngine::Terrain {
    void Layer::LoadTexture(size_t textureId) {
        m_TextureId = textureId;

        m_SourceImage = Assets::GetTexture(m_TextureId).copyToImage();
    }

    void Layer::SetSize(const sf::Vector2<size_t>& cellCount, const size_t& cellSize) {
        CellCount = cellCount;
        CellSize = cellSize;

        LayerSize.x = CellCount.x * CellSize;
        LayerSize.y = CellCount.y * CellSize;

        m_Image.resize({LayerSize.x, LayerSize.y}, sf::Color::Transparent);
        m_Sprite.setTextureRect(sf::IntRect({0, 0}, {static_cast<int>(LayerSize.x), static_cast<int>(LayerSize.y)}));
    }

    sf::Drawable* Layer::GetDrawable() {
        for (size_t cellIndex = 0; cellIndex < Cells.size(); cellIndex++) {
            auto sourceIndex = Cells[cellIndex];
            if (sourceIndex != Config::MAX_SIZE) {
                sf::Vector2<size_t> sourceFrame;
                if (AnimatedTiles.find(sourceIndex) != AnimatedTiles.end()) {
                    auto& animState = AnimatedTiles.at(sourceIndex);
                    sourceFrame.x = (animState.Clips[CellClipIndex[cellIndex]]->FirstFrameOrigin.x + animState.CurrentFrame * CellSize);
                    sourceFrame.y = sourceIndex * CellSize;
                } else {
                    sourceFrame.x = 0;
                    sourceFrame.y = sourceIndex * CellSize;
                }

                sf::Vector2<size_t> targetFrame;
                targetFrame.x = cellIndex * CellSize % LayerSize.x;
                targetFrame.y = (cellIndex * CellSize / LayerSize.x) * CellSize;

                for (size_t y = 0; y < CellSize; y++) {
                    for (size_t x = 0; x < CellSize; x++) {
                        auto color = m_SourceImage.getPixel({sourceFrame.x + x, sourceFrame.y + y});
                        m_Image.setPixel({targetFrame.x + x, targetFrame.y + y}, color);
                    }
                }
            }
        }

        if (m_Texture.loadFromImage(m_Image)) {
            m_Sprite.setTexture(m_Texture);
            return &m_Sprite;
        }

        _log->error("Failed to load image generated for a map's Layer.");
        return nullptr;
    }
}
