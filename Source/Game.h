#pragma once

#include <DxLib.h>
#include <chrono>
#include <ratio>

#include "SceneManager.h"
#include "ResourceManager.h"
#include "FontManager.h"
#include "Cursor.h"
#include "Mouse.h"          // 追加
#include "ClickEffect.h"    // 追加
#include "GoalEffect.h"
#include "EffectManager.h"

class Game {
public:
    // C++20 chrono�^��p�����Œ�^�C���X�e�b�v��`
    using FrameDuration = std::chrono::duration<double, std::ratio<1, 60>>;
    static constexpr FrameDuration FIXED_TIMESTEP{ 1.0 };
    static constexpr int MAX_SIMULATION_STEPS{ 5 };

    Game();
    ~Game();

    // �R�s�[�E���[�u�֎~
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;
    Game(Game&&) = delete;
    Game& operator=(Game&&) = delete;

    void Run();

    bool IsInitialized() const noexcept { return m_initSuccess; }

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
    ResourceManager       m_resourceManager{};
    FontManager           m_fontManager{};
    SceneManager          m_sceneManager{};
    Cursor                m_cursor{};
    Mouse                 m_mouse{};        // 追加
    TofuGame::ClickEffect m_clickEffect{};  // 追加
    GoalEffect m_goalEffect;
    EffectManager m_effectManager;
};