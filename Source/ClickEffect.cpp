#include "ClickEffect.h"
#include "EffectManager.h"

namespace TofuGame {

    ClickEffect::ClickEffect(std::string effectName)
        : effectName_(std::move(effectName))
    {
    }

    void ClickEffect::spawn(const Vector2& clickPosition)
    {
        // GetInstance() でインスタンスを取得して再生
        EffectManager::GetInstance().PlayEffect(effectName_, clickPosition.x, clickPosition.y);
    }

} // namespace TofuGame