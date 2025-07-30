#include "Layer.h"

#include "Config.h"
#include "assets/Assets.h"
#include "SFML/System/Vector2.hpp"

namespace LowEngine::Terrain {
    void Layer::LoadTexture(size_t textureId) {
        TextureId = textureId;

        _sourceImage = Assets::GetTexture(TextureId).copyToImage();
    }

    void Layer::SetSize(const sf::Vector2<size_t>& cellCount, const size_t& cellSize) {
        CellCount = cellCount;
        CellSize = cellSize;

        LayerSize.x = CellCount.x * CellSize;
        LayerSize.y = CellCount.y * CellSize;

        _image.resize({static_cast<unsigned>(LayerSize.x), static_cast<unsigned>(LayerSize.y)}, sf::Color::Transparent);
        _sprite.setTextureRect(sf::IntRect({0, 0}, {static_cast<int>(LayerSize.x), static_cast<int>(LayerSize.y)}));
    }

    sf::Sprite* Layer::GetDrawable() {
        if (_sourceImage.getSize() == sf::Vector2u(0, 0)) {
            // source image for this layer was not assigned - layer will not be drawn
            return nullptr;
        }

        // function updates _image, whish is a local "buffer"/source for internal Sprite.
        // it goes through all Cells, checking what is the index for each of it. Index is equvalent to "Cell Type" (and also actual image, that should be used)
        for (size_t cellIndex = 0; cellIndex < Cells.size(); cellIndex++) {
            auto sourceIndex = Cells[cellIndex];
            if (sourceIndex != Config::MAX_SIZE) {
                // sourceFrame is a coordinate of origin point (upper-left corner) for a piece of texture that should be used to paint current cell.
                sf::Vector2<size_t> sourceFrame;

                // ceck if Cell under Index as animation assigned
                if (AnimatedTiles.find(sourceIndex) != AnimatedTiles.end()) {
                    auto& animState = AnimatedTiles.at(sourceIndex);
                    sourceFrame.x = animState.Clips[CellClipIndex[cellIndex]]->FirstFrameOrigin.x + animState.CurrentFrame * CellSize;
                    sourceFrame.y = sourceIndex * CellSize;
                } else {
                    sourceFrame.x = 0;
                    sourceFrame.y = sourceIndex * CellSize;
                }

                // targetFrame is an origin point on the _image, to which selected source should be copied to
                sf::Vector2<size_t> targetFrame;
                targetFrame.x = cellIndex * CellSize % LayerSize.x;
                targetFrame.y = (cellIndex * CellSize / LayerSize.x) * CellSize;

                // copy source to target pixel-by-pixel
                for (size_t y = 0; y < CellSize; y++) {
                    for (size_t x = 0; x < CellSize; x++) {
                        auto color = _sourceImage.getPixel({ static_cast<unsigned>(sourceFrame.x + x), static_cast<unsigned>(sourceFrame.y + y)});
                        _image.setPixel({ static_cast<unsigned>(targetFrame.x + x), static_cast<unsigned>(targetFrame.y + y)}, color);
                    }
                }
            }
        }

        if (_texture.loadFromImage(_image)) {
            _sprite.setTexture(_texture);
            return &_sprite;
        }

        _log->error("Failed to load image generated for a map's Layer.");
        return nullptr;
    }
}
