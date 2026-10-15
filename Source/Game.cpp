#include "Game.h"
#include "ScreenConfig.h"
#include "InputManager.h"
#include "Master.h"

#include <algorithm>
#include <chrono>

using namespace std::chrono_literals;

SceneManager* Master::mpSceneManager{ nullptr };
ResourceManager* Master::mpResource{ nullptr };
FontManager* Master::mpFontManager{ nullptr };

Game::Game()
{
    // ここが失敗(false)を返すと、ウィンドウが開かずにアプリが即終了する
    if (!InitializeDxLib()) {
        return;
    }

    // 各マネージャーのポインタ受け渡し。
    // この順番を変えると、参照先が存在しない状態でアクセスしてクラッシュする原因になる
    Master::mpResource = &m_resourceManager;
    Master::mpFontManager = &m_fontManager;
    Master::mpSceneManager = &m_sceneManager;

    InputManager::GetInstance().InitializeButton();
    InputManager::GetInstance().InitializeAxis();
    m_cursor.Initialize();

    // 最初に読み込むシーン（タイトル・ゲーム等）を切り替えるなら、この Initialize 内を編集する
    m_sceneManager.Initialize();

    m_initSuccess = true;
    m_isRunning = true;
    m_lastFrameTime = std::chrono::steady_clock::now();
}

Game::~Game()
{
    if (m_initSuccess) {
        // 生成時と「逆順」で破棄している。
        // 例えば先に Resource を破棄して Scene を後回しにすると、画像解放時にクラッシュする
        m_cursor.Finalize();
        m_sceneManager.Finalize();
        m_fontManager.Clear();

        // ここを消すと、終了直前に他オブジェクトからアクセスされた際に「不正メモリ参照」で落ちる
        Master::mpSceneManager = nullptr;
        Master::mpFontManager = nullptr;
        Master::mpResource = nullptr;
    }

    DxLib_End();
}

bool Game::InitializeDxLib()
{
    // TRUE  → ウィンドウモード（デバッグがしやすい）
    // FALSE → フルスクリーンモード（製品版や没入感重視の時）
    ChangeWindowMode(TRUE);

    // 【解像度・色深度の変更】
    // 32 を 16 にすると低スペック向けになるが、発色やグラデーションが劣化する
    // SCREEN_WIDTH / HEIGHT を変えるとウィンドウ全体の表示サイズが変わる
    SetGraphMode(
        static_cast<int>(ScreenConfig::SCREEN_WIDTH),
        static_cast<int>(ScreenConfig::SCREEN_HEIGHT),
        32
    );

    // ウィンドウ上部のバーに表示されるタイトル文字
    SetMainWindowText("Game");

    // TRUE  → 60Hzや144Hzなどのモニター描画同期を待つ（画面のチラつき・ズレを防ぐ）
    // FALSE → V-Syncを待たずにフルパワーで回す（FPS測定や入力遅延を極限まで減らしたい時）
    SetWaitVSyncFlag(TRUE);

    if (DxLib_Init() == -1) {
        return false;
    }

    // DX_SCREEN_BACK → ダブルバッファリング有効（画面のチラつき防止）
    // DX_SCREEN_FRONTにすると描画途中の画面がそのまま映って激しく点滅する
    SetDrawScreen(DX_SCREEN_BACK);

    // 3Dモデルを描画する際の奥行き判定（Zバッファ）
    // どちらかを FALSE にすると、奥にあるキャラクターが手前の壁を突き抜けて表示されてしまう
    SetUseZBufferFlag(TRUE);
    SetWriteZBufferFlag(TRUE);

    // FALSE にすると陰影の計算がなくなり、3Dモデル全体が平坦で単色の見た目になる
    SetUseLighting(TRUE);

    // 【標準ライトの色指定：(赤, 緑, 青, アルファ)】
    // RGBの数値を (1.0f, 1.0f, 1.0f) にすると白い自然光になる（現在はやや温かみのある黄色系）
    SetLightDifColor(GetColorF(1.0f, 0.8f, 0.4f, 0.0f));

    // 【環境光（影になる暗がりの明るさ）】
    // 数値を下げて (0.1f, 0.1f, 0.1f) 等にすると明暗差が強くなり、ホラーゲームのような暗い絵になる
    // 現在の 3.2f はかなり明るめの設定
    SetLightAmbColor(GetColorF(3.2f, 3.2f, 3.2f, 0.0f));

    return true;
}

void Game::Run()
{
    if (!m_initSuccess) {
        return;
    }

    while (m_isRunning && ProcessMessage() == 0) {

        // 【終了キーの変更】
        // KEY_INPUT_ESCAPE を KEY_INPUT_Q に変えれば「Qキーで終了」になる
        // 開発中に誤爆終了を防ぎたい場合は、この if 文ごとコメントアウトすれば無効化できる
        if (CheckHitKey(KEY_INPUT_ESCAPE) != 0) {
            m_isRunning = false;
            break;
        }

        InputManager::GetInstance().Update();

        const auto currentTime = std::chrono::steady_clock::now();
        std::chrono::duration<double> frameTime = currentTime - m_lastFrameTime;
        m_lastFrameTime = currentTime;

        // 【スパイク対策の上限値】
        // 0.2s（最大0.2秒分まで蓄積）を設定中。
        // ここを 0.05s に狭めると、重い処理が起きた時にゲーム内時間がスキップされやすくなる
        // 逆に 1.0s などに広げると、カクついた後の復旧時にキャラクターが一瞬猛スピードで動く
        frameTime = std::clamp(frameTime, 0.0s, 0.2s);
        m_accumulator += frameTime;

        int stepCount{ 0 };

        // 【更新頻度の変更】
        // FIXED_TIMESTEP を (1.0s / 120.0) に変えると120FPS基準の精密な物理計算になる
        // (1.0s / 30.0) にすると計算負荷は半減するが、高速で動く弾やキャラが壁をすり抜けやすくなる
        const std::chrono::duration<double> fixedStep = FIXED_TIMESTEP;

        // 【無限ループ・フリーズ防止ガード】
        // MAX_SIMULATION_STEPS（例: 5回など）の上限を設けている。
        // ここを stepCount < 100 に増やすと、処理落ち時に無理に追いつこうとして完全にゲームが固まる（Spiral of Death）
        while (m_accumulator >= fixedStep && stepCount < MAX_SIMULATION_STEPS) {
            UpdateLogic(fixedStep.count());
            m_accumulator -= fixedStep;
            ++stepCount;
        }

        Draw();
        ScreenFlip(); // これを呼ばないと裏画面に描いた内容がモニターに反映されない
    }
}

void Game::UpdateLogic(double deltaTime)
{
    m_cursor.Update();

    // deltaTime に 2.0f を掛けると「ゲーム全体が2倍速」になる（倍速デバッグなどに利用可能）
    // 0.5f を掛けるとスローモーションになる
    m_sceneManager.Update(static_cast<float>(deltaTime));
}

void Game::Draw()
{
    // これを消すと、過去フレームの残像がすべて塗り重ねられて画面がぐちゃぐちゃになる
    ClearDrawScreen();

    // 【描画順序の変更】
    // 描画順を逆にして m_cursor.Draw() を先に書くと、
    // 背景や3Dモデル、UIの後ろにカーソルが隠れて見えなくなってしまう
    m_sceneManager.Draw();
    m_cursor.Draw(); // 最前面に表示したいものを一番最後に描く
}