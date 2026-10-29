#include "UIButton.h"
#include "UI.h"
#include "UITransform.h"
#include "InputManager.h"

void UIButton::Initialize(UI* _pUI)
{
    mpUI = _pUI;
}

bool UIButton::IsMouseInside()
{
    if (mpUI == nullptr)
        return false;

    UITransform& transform = mpUI->GetTransform();

    VECTOR center = transform.GetCenterPosition();
    VECTOR size = transform.GetSize();
    VECTOR scale = transform.GetScale();

    // 描画(DrawRotaGraph3)と同じく拡縮後の大きさで判定する
    size.x *= scale.x;
    size.y *= scale.y;

    int mouseX = 0;
    int mouseY = 0;

    GetMousePoint(&mouseX, &mouseY);

    return
        mouseX >= center.x - size.x * 0.5f &&
        mouseX <= center.x + size.x * 0.5f &&
        mouseY >= center.y - size.y * 0.5f &&
        mouseY <= center.y + size.y * 0.5f;
}

bool UIButton::IsClicked()
{
    if (!IsMouseInside())
        return false;

    return InputManager::GetInstance()
        .GetButtonDown(Button::Confirm);
}