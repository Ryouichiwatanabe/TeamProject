#pragma once
#include "Scene.h"
#include <string>

// 料理と豆腐の世界観に合わせたロード画面
class LoadScene : public Scene
{
public:
    LoadScene() = default;
    ~LoadScene() override = default;

    void Initialize() override;             // フォント生成とTips設定
    void Update(float _deltaTime) override; // したごしらえ進行とクリック検知
    void Draw() override;                   // 湯気・出汁ゲージ・お料理メモの描画
    void Finalize() override;               // フォント解放

private:
    int mnStageNumber = 1;          // 挑戦するステージ番号
    std::string mTipTitle;          // お料理メモの見出し
    std::string mTipContent;        // お料理メモの本文

    int mnCustomFont = -1;          // クッキリ読めるLoadScene専用フォント
    float mfTimer = 0.0f;           // 経過タイマー
    const float LOAD_DURATION = 1.5f; // したごしらえ完了までの時間（秒）
    bool mbReady = false;           // 準備完了フラグ
};