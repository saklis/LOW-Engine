
#include "TileMap.h"

#include "Config.h"

void LowEngine::Terrain::TileMap::Update(float deltaTime) {
    for (auto& state: TerrainLayer.AnimatedTiles | std::views::values) {
        state.FrameTime += deltaTime;
        if (state.FrameTime >= state.Clips[state.ClipIndex]->FrameDuration) {
            state.FrameTime = 0.0f;
            state.CurrentFrame++;
            if (state.CurrentFrame >= state.Clips[state.ClipIndex]->FrameCount) {
                state.CurrentFrame = 0;
            }
        }
    }

    for (auto& state: FeaturesLayer.AnimatedTiles | std::views::values) {
        state.FrameTime += deltaTime;
        if (state.FrameTime >= state.Clips[state.ClipIndex]->FrameDuration) {
            state.FrameTime = 0.0f;
            state.CurrentFrame++;
            if (state.CurrentFrame >= state.Clips[state.ClipIndex]->FrameCount) {
                state.CurrentFrame = 0;
            }
        }
    }
}

void LowEngine::Terrain::TileMap::LoadFromLDTkJson(nlohmann::json::const_reference jsonData) {
    Name = jsonData["identifier"].get<std::string>();
    Size.x = jsonData["pxWid"].get<size_t>();
    Size.y = jsonData["pxHei"].get<size_t>();

    for (auto& layer : jsonData["layerInstances"]) {
        if (layer["__identifier"] == "Terrain") {
            TerrainLayer.SetSize({layer["__cWid"].get<size_t>(), layer["__cHei"].get<size_t>()}, layer["__gridSize"].get<size_t>());
            NavGrid.Cells.resize(layer["__cWid"].get<size_t>() * layer["__cHei"].get<size_t>());
            NavGrid.Width = layer["__cWid"].get<size_t>();
            NavGrid.Height = layer["__cHei"].get<size_t>();

            TerrainLayer.Cells.assign(TerrainLayer.CellCount.x * TerrainLayer.CellCount.y, Config::MAX_SIZE);
            for (auto& gridCell : layer["gridTiles"]) {
                auto cellIndex = static_cast<size_t>(gridCell["d"][0]);
                size_t tileIndex = static_cast<size_t>(gridCell["src"][1]) / TerrainLayer.CellSize;
                TerrainLayer.Cells[cellIndex] = tileIndex;
            }
        }

        if (layer["__identifier"] == "Features") {
            FeaturesLayer.SetSize({layer["__cWid"].get<size_t>(), layer["__cHei"].get<size_t>()}, layer["__gridSize"].get<size_t>());

            FeaturesLayer.Cells.assign(FeaturesLayer.CellCount.x * FeaturesLayer.CellCount.y, Config::MAX_SIZE);
            for (auto& gridCell : layer["gridTiles"]) {
                auto cellIndex = static_cast<size_t>(gridCell["d"][0]);
                size_t tileIndex = static_cast<size_t>(gridCell["src"][1]) / FeaturesLayer.CellSize;
                FeaturesLayer.Cells[cellIndex] = tileIndex;
            }
        }
    }
}
