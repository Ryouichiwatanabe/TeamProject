#include "Game.h"
#include "ScreenConfig.h"
#include "InputManager.h"
#include "Master.h"

#include <algorithm>
#include <chrono>

using namespace std::chrono_literals;

Game::Game()
{
    if (!InitializeDxLib()) {
        return;
    }

    // 各マネージャーのポインタ受け渡し
    Master::mpResource = &m_resourceManager;
    Master::mpFontManager = &m_fontManager;
    Master::mpSceneManager = &m_sceneManager;

    InputManager::GetInstance().InitializeButton();
    InputManager::GetInstance().InitializeAxis();
    m_cursor.Initialize();

    // 最初のシーンを読み込む
    m_sceneManager.Initialize();

    m_initSuccess = true;
    m_isRunning = true;
    m_lastFrameTime = std::chrono::steady_clock::now();
}

Game::~Game()
{
    if (m_initSuccess) {
        m_goalEffect.Clear();
        m_clickEffect.clear();
        m_cursor.Finalize();
        m_sceneManager.Finalize();
        m_fontManager.Clear();

        Master::mpSceneManager = nullptr;
        Master::mpFontManager = nullptr;
        Master::mpResource = nullptr;
    }

    DxLib_End();
}

bool Game::InitializeDxLib()
{
    ChangeWindowMode(TRUE);

    SetGraphMode(
        static_cast<int>(ScreenConfig::SCREEN_WIDTH),
        static_cast<int>(ScreenConfig::SCREEN_HEIGHT),
        32
    );

    SetWaitVSyncFlag(TRUE);

    if (DxLib_Init() == -1) {
        return false;
    }

    SetDrawScreen(DX_SCREEN_BACK);

    SetUseZBufferFlag(TRUE);
    SetWriteZBufferFlag(TRUE);
    SetUseLighting(TRUE);

    SetLightDifColor(GetColorF(1.0f, 0.8f, 0.4f, 0.0f));
    SetLightAmbColor(GetColorF(3.2f, 3.2f, 3.2f, 0.0f));

    return true;
}

void Game::Run()
{
    if (!m_initSuccess) {
        return;
    }

    while (m_isRunning && ProcessMessage() == 0) {

        if (CheckHitKey(KEY_INPUT_ESCAPE) != 0) {
            m_isRunning = false;
            break;
        }

        // マウス更新とクリック判定
        m_mouse.Update();
        if (m_mouse.IsDown(MOUSE_INPUT_LEFT)) {
            const TofuGame::Vector2 clickPos{
                static_cast<float>(m_mouse.GetX()),
                static_cast<float>(m_mouse.GetY())
            };
            m_clickEffect.spawn(clickPos);
        }

        const auto currentTime = std::chrono::steady_clock::now();
        std::chrono::duration<double> frameTime = currentTime - m_lastFrameTime;
        m_lastFrameTime = currentTime;

        frameTime = std::clamp(frameTime, 0.0s, 0.2s);
        m_accumulator += frameTime;

        int stepCount{ 0 };
        const std::chrono::duration<double> fixedStep = FIXED_TIMESTEP;

        while (m_accumulator >= fixedStep && stepCount < MAX_SIMULATION_STEPS) {
            InputManager::GetInstance().Update();
            UpdateLogic(fixedStep.count());
            m_accumulator -= fixedStep;
            ++stepCount;
        }

        Draw();
        ScreenFlip();
    }
}

void Game::UpdateLogic(double deltaTime)
{
    m_cursor.Update();

    // シーン更新
    m_sceneManager.Update(static_cast<float>(deltaTime));

    // 各種エフェクト更新
    m_clickEffect.update(static_cast<float>(deltaTime));
    m_goalEffect.Update();
}

void Game::Draw()
{
    ClearDrawScreen();

    // 1. 3Dシーン描画（3D用の深度・ライティング設定）
    SetUseZBufferFlag(TRUE);
    SetWriteZBufferFlag(TRUE);
    SetUseLighting(TRUE);

    m_sceneManager.Draw();

    // 2. 2Dエフェクト描画（ライティング・ZテストをOFFにして最前面にクリア描画）
    SetUseZBufferFlag(FALSE);
    SetWriteZBufferFlag(FALSE);
    SetUseLighting(FALSE);

    m_clickEffect.draw();
    m_goalEffect.Draw();

    // 3. 最前面カーソル
    m_cursor.Draw();

    // 描画ステートを3D用のデフォルト状態に復帰
    SetUseZBufferFlag(TRUE);
    SetWriteZBufferFlag(TRUE);
    SetUseLighting(TRUE);
}