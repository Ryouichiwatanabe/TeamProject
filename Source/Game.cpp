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

    // 各マネージャーのポインタ受け渡し（EffectManagerはScene側で持つため除外）
    Master::mpResource = &m_resourceManager;
    Master::mpFontManager = &m_fontManager;
    Master::mpSceneManager = &m_sceneManager;

    InputManager::GetInstance().InitializeButton();
    InputManager::GetInstance().InitializeAxis();
    m_cursor.Initialize();

    m_sceneManager.Initialize();

    m_initSuccess = true;
    m_isRunning = true;
    m_lastFrameTime = std::chrono::steady_clock::now();

    // -- フォントの読み込み -- //
    // Master::mpFontManager->LoadFontFile("")
}

Game::~Game()
{
    if (m_initSuccess) {
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

        m_mouse.Update();

        
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

    // シーンの更新（この中でシーンが所有するEffectManagerが更新される）
    m_sceneManager.Update(static_cast<float>(deltaTime));


}

void Game::Draw()
{
    ClearDrawScreen();

    // 1. 3Dシーン描画
    SetUseZBufferFlag(TRUE);
    SetWriteZBufferFlag(TRUE);
    SetUseLighting(TRUE);

    m_sceneManager.Draw();

    // 2. 2D描画（ZバッファOFF）
    SetUseZBufferFlag(FALSE);
    SetWriteZBufferFlag(FALSE);
    SetUseLighting(FALSE);


    // 3. 最前面カーソル
    m_cursor.Draw();

    // 4. ステート復元
    SetUseZBufferFlag(TRUE);
    SetWriteZBufferFlag(TRUE);
    SetUseLighting(TRUE);
}