#include "Game.h"
#include "ScreenConfig.h"
#include "InputManager.h"
#include "Master.h"
#include "EffectManager.h"

#include <algorithm>
#include <chrono>
#include "ClickEffect.h"

using namespace std::chrono_literals;

Game::Game()
{
    if (!InitializeDxLib()) {
        return;
    }

    if (!m_effectManager.Initialize(
        static_cast<int>(ScreenConfig::SCREEN_WIDTH),
        static_cast<int>(ScreenConfig::SCREEN_HEIGHT)))
    {
        return;
    }

    // クリックエフェクトの読み込み（見失わないよう scale を 100.0f に設定）
    bool isLoaded = m_effectManager.Load("click", L"Resource/Effekseer/Effect.efk", 1.0f);
    if (!isLoaded) {
        MessageBox(nullptr, "ClickEffect.efk の読み込みに失敗しました！\nパスまたは配置場所を確認してください。", "エラー", MB_OK);
    }

    // 各マネージャーのポインタ受け渡し
    Master::mpResource = &m_resourceManager;
    Master::mpFontManager = &m_fontManager;
    Master::mpSceneManager = &m_sceneManager;
    Master::mpEffectManager = &m_effectManager;

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
        m_cursor.Finalize();
        m_sceneManager.Finalize();
        m_fontManager.Clear();

        m_effectManager.Terminate(); // Effekseerの終了
        Master::mpEffectManager = nullptr;

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

    // DxLib_Init の前にDirectX 11を設定
    SetUseDirect3DVersion(DX_DIRECT3D_11);
    SetChangeScreenModeGraphicsSystemResetFlag(FALSE);

    if (DxLib_Init() == -1) return false;

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

        // マウス入力と再生ハンドルの記録
      // Game.cpp の Run() 内
        m_mouse.Update();
        if (m_mouse.IsDown(MOUSE_INPUT_LEFT)) {
            const TofuGame::Vector2 clickPos{
                static_cast<float>(m_mouse.GetX()),
                static_cast<float>(m_mouse.GetY())
            };

            // ★自作ClickEffect呼び出しを無くし、Effekseerに一本化
            m_lastPlayHandle = m_effectManager.PlayEffect("click", clickPos.x, clickPos.y);
            m_lastClickX = clickPos.x;
            m_lastClickY = clickPos.y;
            m_clickCount++;
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

    // エフェクトの更新
    m_effectManager.Update();
}

void Game::Draw()
{
    ClearDrawScreen();

    // 1. 3Dシーン描画
    SetUseZBufferFlag(TRUE);
    SetWriteZBufferFlag(TRUE);
    SetUseLighting(TRUE);
    m_sceneManager.Draw();

    // 2. 2Dエフェクト描画
    SetUseZBufferFlag(FALSE);
    SetWriteZBufferFlag(FALSE);
    SetUseLighting(FALSE);
    m_goalEffect.Draw();

    // ★ここで毎フレーム確実に更新してから描画する
    m_effectManager.Update();
    m_effectManager.Draw();

    // 3. 最前面カーソル
    m_cursor.Draw();

    // 描画ステート復帰
    SetUseZBufferFlag(TRUE);
    SetWriteZBufferFlag(TRUE);
    SetUseLighting(TRUE);

    // --- デバッグ文字表示（画面最前面） ---
    SetUseZBufferFlag(FALSE);
    SetWriteZBufferFlag(FALSE);
    SetUseLighting(FALSE);

    unsigned int colorWhite = GetColor(255, 255, 255);
    unsigned int colorGreen = GetColor(0, 255, 0);
    unsigned int colorRed = GetColor(255, 0, 0);

    DrawString(10, 10, "--- Effekseer Debug Info ---", colorWhite);
    DrawFormatString(10, 30, colorWhite, "Click Count: %d", m_clickCount);
    DrawFormatString(10, 50, colorWhite, "Last Click Pos: (%.1f, %.1f)", m_lastClickX, m_lastClickY);

    if (m_lastPlayHandle == -999) {
        DrawString(10, 70, "Status: Not Clicked Yet", colorWhite);
    }
    else if (m_lastPlayHandle == -1) {
        DrawString(10, 70, "Status: Play Failed (-1: Effect not registered)", colorRed);
    }
    else {
        bool isPlaying = m_effectManager.IsPlaying(m_lastPlayHandle);
        DrawFormatString(10, 70, isPlaying ? colorGreen : colorRed,
            "Status: Handle %d (%s)", m_lastPlayHandle, isPlaying ? "PLAYING" : "STOPPED / ENDED");
    }

    // クリック位置に赤い十字線を描画
    if (m_clickCount > 0) {
        DrawLine(static_cast<int>(m_lastClickX) - 10, static_cast<int>(m_lastClickY),
            static_cast<int>(m_lastClickX) + 10, static_cast<int>(m_lastClickY), colorRed);
        DrawLine(static_cast<int>(m_lastClickX), static_cast<int>(m_lastClickY) - 10,
            static_cast<int>(m_lastClickX), static_cast<int>(m_lastClickY) + 10, colorRed);
    }
}