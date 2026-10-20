#pragma once
#include "DxLib.h"
#include <string>
#include <unordered_map>

class EffectManager
{
public:

    // ★ここを追加：唯一のインスタンスを返す関数
    static EffectManager& GetInstance()
    {
        static EffectManager instance;
        return instance;
    }

    EffectManager() = default;
    ~EffectManager();

    // コピーと代入を禁止
    EffectManager(const EffectManager&) = delete;
    EffectManager& operator=(const EffectManager&) = delete;
   

    // 画面の幅と高さを渡して初期化
    bool Initialize(int screenWidth, int screenHeight);

    // 毎フレームの更新
    void Update();

    // 描画（スプライトなどの2D画像を描いた後に呼ぶ）
    void Draw();

    // 終了処理
    void Terminate();

    // 第3引数にデフォルト値 1.0f を追加
    bool Load(const std::string& name, const wchar_t* filePath, float scale = 1.0f);

    // 2D座標（X, Yピクセル）で再生
    int PlayEffect(const std::string& name, float x, float y);

    // エフェクトの停止
    void StopEffect(int playHandle);

    // 再生中エフェクトの座標更新（キャラ追従用）
    void SetPosition(int playHandle, float x, float y);

    [[nodiscard]] bool IsPlaying(int playHandle) const;


private:


    std::unordered_map<std::string, int> effectMap_;
    bool isInitialize_ = false;
    int screenWidth_ = 0;
    int screenHeight_ = 0;
}; 