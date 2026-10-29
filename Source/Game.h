#pragma once

#include <DxLib.h>
#include <chrono>
#include <ratio>

#include "SceneManager.h"
#include "ResourceManager.h"
#include "FontManager.h"
#include "Cursor.h"
#include "Mouse.h"

class Game {
public:
    using FrameDuration = std::chrono::duration<double, std::ratio<1, 60>>;
    static constexpr FrameDuration FIXED_TIMESTEP{ 1.0 };
    static constexpr int MAX_SIMULATION_STEPS{ 5 };

    static constexpr int SCREEN_WIDTH = 1280;
    static constexpr int SCREEN_HEIGHT = 720;

    Game();
    ~Game();

    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;
    Game(Game&&) noexcept = delete;
    Game& operator=(Game&&) noexcept = delete;

    void Run();

    [[nodiscard]] bool IsInitialized() const noexcept { return m_initSuccess; }

private:
    bool InitializeDxLib();
    void UpdateLogic(double deltaTime);
    void Draw();

private:
    bool m_isRunning{ false };
    bool m_initSuccess{ false };

    std::chrono::steady_clock::time_point m_lastFrameTime{};
    std::chrono::duration<double> m_accumulator{ 0.0 };

    // 依存関係順に定義（下から上へ逆順に破棄される）
    ResourceManager   m_resourceManager{};
    FontManager       m_fontManager{};
    SceneManager      m_sceneManager{};
    Cursor            m_cursor{};
    Mouse             m_mouse{};
  
};