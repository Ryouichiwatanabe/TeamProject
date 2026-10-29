#pragma once
#include <string>

namespace TofuGame {

    struct Vector2 {
        float x = 0.0f;
        float y = 0.0f;
    };

    class ClickEffect {
    public:
        // 使用するEffekseerエフェクト名を指定して初期化
        explicit ClickEffect(std::string effectName = "ClickEffect");
        ~ClickEffect() = default;

        // クリック地点にEffekseerエフェクトを発生
        void spawn(const Vector2& clickPosition);

        // 必要に応じて使用エフェクトを切り替える場合
        void setEffectName(std::string effectName) { effectName_ = std::move(effectName); }

    private:
        std::string effectName_;
    };

} // namespace TofuGame