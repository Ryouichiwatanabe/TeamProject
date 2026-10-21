#include "EffectManager.h"
#include "EffekseerForDXLib.h"

namespace EffectState
{
    constexpr int MaxParticle = 8000;
}
EffectManager::~EffectManager()
{
    Terminate();
}

bool EffectManager::Initialize(int screenWidth, int screenHeight)
{
    if (isInitialize_) return true;

    screenWidth_ = screenWidth;
    screenHeight_ = screenHeight;

    // 初期化（パーティクル最大数: 8000）
    if (Effekseer_Init(EffectState::MaxParticle) == -1) {
        return false;
    }

    // 歪み表現の初期化
    Effekseer_InitDistortion();

    // 2D表示用の画面解像度を設定
    Effekseer_Set2DSetting(screenWidth_, screenHeight_);

    SetUseZBuffer3D(TRUE);
    SetWriteZBuffer3D(TRUE);


    isInitialize_ = true;
    return true;
}

void EffectManager::Update()
{
    if (!isInitialize_) return;

    // 2Dエフェクトのコマ送り更新
    UpdateEffekseer2D();
}

void EffectManager::Draw()
{
    if (!isInitialize_) return;

    // 2Dエフェクトの全描画
    DrawEffekseer2D();
}

void EffectManager::Terminate()
{
    if (!isInitialize_) return;

    for (auto& [name, handle] : effectMap_) {
        DeleteEffekseerEffect(handle);
    }
    effectMap_.clear();

    Effkseer_End();
    isInitialize_ = false;
}

// 修正後
bool EffectManager::Load(const std::string& name, const wchar_t* filePath, float scale)
{
    if (!isInitialize_) return false;

    if (effectMap_.contains(name)) {
        return true;
    }

    // 第2引数に scale を渡す
    int handle = LoadEffekseerEffect(filePath, scale);
    if (handle == -1) {
        return false;
    }

    effectMap_[name] = handle;
    return true;
}

int EffectManager::PlayEffect(const std::string& name, float x, float y)
{
    if (!isInitialize_) return -1;

    auto it = effectMap_.find(name);
    if (it == effectMap_.end()) {
        return -1;
    }

    // 2Dエフェクト再生
    int playHandle = PlayEffekseer2DEffect(it->second);
    if (playHandle != -1) {
        // ピクセル座標でセット（Z値は0.0f）
        SetPosPlayingEffekseer2DEffect(playHandle, x, y, 0.0f);
    }

    return playHandle;
}

void EffectManager::StopEffect(int playHandle)
{
    if (playHandle != -1) {
        StopEffekseer2DEffect(playHandle);
    }
}

void EffectManager::SetPosition(int playHandle, float x, float y)
{
    if (playHandle != -1) {
        SetPosPlayingEffekseer2DEffect(playHandle, x, y, 0.0f);
    }
}

bool EffectManager::IsPlaying(int playHandle) const
{
    if (playHandle == -1) return false;
    // 0が再生中、-1が停止中/存在しない
    return IsEffekseer2DEffectPlaying(playHandle) == 0;
}
