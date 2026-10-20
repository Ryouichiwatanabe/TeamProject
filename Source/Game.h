#pragma once
#include "DxLib.h"
#include "ScreenConfig.h"
#include "InputManager.h"
#include "ResourceManager.h"
#include "FontManager.h"
#include "SceneManager.h"
#include "EffectManager.h"
#include "Cursor.h"
#include "GoalEffect.h"
#include "Mouse.h"

#include <chrono>

class Game
{
public:
    Game();
    ~Game();

    void Run();
    bool InitializeDxLib();
private:
   
    void UpdateLogic(double deltaTime);
    void Draw();

private:
    // 各種マネージャー・オブジェクト
    ResourceManager m_resourceManager;
    FontManager m_fontManager;
    SceneManager m_sceneManager;
    EffectManager m_effectManager;

    Cursor m_cursor;
    GoalEffect m_goalEffect;
    Mouse m_mouse;

    // ゲームループ制御
    bool m_initSuccess = false;
    bool m_isRunning = false;
    std::chrono::steady_clock::time_point m_lastFrameTime;
    std::chrono::duration<double> m_accumulator{ 0.0 };

    static constexpr std::chrono::duration<double> FIXED_TIMESTEP{ 1.0 / 60.0 };
    static constexpr int MAX_SIMULATION_STEPS = 5;

    // ★デバッグ表示用変数
    int m_lastPlayHandle = -999;
    float m_lastClickX = 0.0f;
    float m_lastClickY = 0.0f;
    int m_clickCount = 0;
};