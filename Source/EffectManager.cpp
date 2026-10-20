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

    // 初期化
    if (Effekseer_Init(EffectState::MaxParticle) == -1) {
        return false;
    }

    // 2D射影設定
    Effekseer_Set2DSetting(screenWidth_, screenHeight_);

    isInitialize_ = true;
    return true;
}

void EffectManager::Update()
{
    if (!isInitialize_) return;

    // 2Dエフェクトの更新
    UpdateEffekseer2D();
}

void EffectManager::Draw()
{
    if (!isInitialize_) return;

    // 1. 保留中の頂点描画をすべて完了
    RenderVertex();

    // 2. ブレンドモードをノーマルに戻す
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    // 3. 2D解像度設定を毎回適用（3D行列によるリセット対策）
    Effekseer_Set2DSetting(screenWidth_, screenHeight_);

    // 4. DXライブラリのDirect3D 11設定をEffekseerへ同期
    RefreshDxLibDirect3DSetting();

    // 5. 2Dエフェクトの描画
    DrawEffekseer2D();

    // 6. 描画後に再度Direct3D設定を同期
    RefreshDxLibDirect3DSetting();
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

bool EffectManager::Load(const std::string& name, const wchar_t* filePath, float scale)
{
    if (!isInitialize_) return false;

    if (effectMap_.contains(name)) {
        return true;
    }

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

    // 2Dエフェクトを再生
    int playHandle = PlayEffekseer2DEffect(it->second);
    if (playHandle != -1) {
        // Z座標は 0.0f
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
    return IsEffekseer2DEffectPlaying(playHandle) == 0;
}