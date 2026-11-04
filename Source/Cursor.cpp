#include "Cursor.h"

#include "Master.h"
#include "ResourceManager.h"    // LoadGraph()

#include "InputManager.h"
#include "Mouse.h"  // GetX(),GetY()

namespace
{
    // -- ホットスポットの調整に用いる定数 -- //
    constexpr int offsetX = -10;
    constexpr int offsetY = -1;  
}

void Cursor::Initialize()
{
    mHandle = Master::mpResource->LoadGraphics("Resource/UI/Cursor/Cursor.png");
    if(mHandle != -1)
        SetMouseDispFlag(FALSE);    // Windowsのマウスカーソルを非表示
}

void Cursor::Draw()
{
    if (mHandle == -1)
        return;

    const auto& mouse = InputManager::GetInstance().GetMouse();
    int x = mouse.GetX();
    int y = mouse.GetY();

    DrawGraph(
        x + offsetX,
        y + offsetY,
        mHandle,
        TRUE
    );
}

void Cursor::Finalize()
{
    SetMouseDispFlag(TRUE); // Windowsのマウスカーソルを再表示
}