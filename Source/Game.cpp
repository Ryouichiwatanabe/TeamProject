#include "Game.h"
#include "ScreenConfig.h"
#include "InputManager.h"
#include "Master.h"
#include "EffectManager.h"

#include <algorithm>
#include <chrono>

using namespace std::chrono_literals;

Game::Game()
{
    // 1. DXライブラリ初期化
    if (!InitializeDxLib()) {
        return;
    }

    const int screenWidth = static_cast<int>(ScreenConfig::SCREEN_WIDTH);
    const int screenHeight = static_cast<int>(ScreenConfig::SCREEN_HEIGHT);

    // 2. エフェクトマネージャー初期化
    if (!m_effectManager.Initialize(screenWidth, screenHeight)) {
        return;
    }

    // 3. 各マネージャーのポインタ受け渡し（サービスロケータ等）
    Master::mpResource = &m_resourceManager;
    Master::mpFontManager = &m_fontManager;
    Master::mpSceneManager = &m_sceneManager;
    Master::mpEffectManager = &m_effectManager;

    // 4. エフェクトのロード（自作リソースへの差し替え例）
    // constexpr float kEffectClickScale = 1.0f;
    // const bool isLoaded = m_effectManager.Load(
    //     "EffectClick",
    //     "Resource/Effect/EffectClick.png",
    //     kEffectClickScale
    // );
    // if (!isLoaded) {
    //     OutputDebugStringA("[WARN] EffectClick load failed!\n");
    // }

    InputManager::GetInstance().InitializeButton();
    InputManager::GetInstance().InitializeAxis();
    m_cursor.Initialize();

    m_sceneManager.Initialize();

    m_initSuccess = true;
    m_isRunning = true;
    m_lastFrameTime = std::chrono::steady_clock::now();
}

Game::~Game()
{
    if (m_initSuccess) {
        m_goalEffect.Clear();
        m_cursor.Finalize();
        m_sceneManager.Finalize();
        m_fontManager.Clear();

        // エフェクトマネージャーの終了 (DxLib_End より前)
        m_effectManager.Terminate();

        Master::mpEffectManager = nullptr;
        Master::mpSceneManager = nullptr;
        Master::mpFontManager = nullptr;
        Master::mpResource = nullptr;
    }

    DxLib_End();
}

bool Game::InitializeDxLib()
{
    // 画面モード・ウィンドウ設定
    ChangeWindowMode(TRUE);
    SetGraphMode(
        static_cast<int>(ScreenConfig::SCREEN_WIDTH),
        static_cast<int>(ScreenConfig::SCREEN_HEIGHT),
        32
    );

    SetWaitVSyncFlag(TRUE);

    // DXライブラリ初期化
    if (DxLib_Init() == -1) {
        return false;
    }

    // 描画先を裏画面に設定
    SetDrawScreen(DX_SCREEN_BACK);

    // 3D初期設定
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

        // マウス入力更新
        m_mouse.Update();

        // クリック入力トリガー（必要に応じてInputManager側に集約推奨）
        if (m_mouse.IsPress(MOUSE_INPUT_LEFT)) {
            const float clickX = static_cast<float>(m_mouse.GetX());
            const float clickY = static_cast<float>(m_mouse.GetY());

            m_effectManager.PlayEffect("EffectClick", clickX, clickY);
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

    m_goalEffect.Update();

    // 2Dエフェクト更新（固定デルタタイムを渡す）
    m_effectManager.Update(static_cast<float>(deltaTime));
}

void Game::Draw()
{
    ClearDrawScreen();

    // 1. 3Dシーン描画
    SetUseZBufferFlag(TRUE);
    SetWriteZBufferFlag(TRUE);
    SetUseLighting(TRUE);

    m_sceneManager.Draw();

    // 2. 2D描画（Zバッファ・ライティングをOFFにして最前面に描画）
    SetUseZBufferFlag(FALSE);
    SetWriteZBufferFlag(FALSE);
    SetUseLighting(FALSE);

    m_goalEffect.Draw();
    m_effectManager.Draw();

    // 3. 最前面カーソル
    m_cursor.Draw();

    // 4. 次フレーム用（または3D用）にステートを復元
    SetUseZBufferFlag(TRUE);
    SetWriteZBufferFlag(TRUE);
    SetUseLighting(TRUE);
}