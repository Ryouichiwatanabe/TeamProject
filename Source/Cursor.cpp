#include "Cursor.h"
#include "DxLib.h"

Cursor::Cursor()
{
}

Cursor::~Cursor()
{
}

void Cursor::Initialize()
{
    // Windowsのマウスカーソルを非表示
    SetMouseDispFlag(FALSE);

    // カーソル画像を読み込む
    mHandle = LoadGraph("Resource/Cursor.png");
}

void Cursor::Update()
{
    // 今は特に処理なし
}

void Cursor::Draw()
{
    if (mHandle == -1)
        return;

    int mouseX;
    int mouseY;

    GetMousePoint(&mouseX, &mouseY);

    const int offsetX = -10;
    const int offsetY = -1;

    DrawGraph(
        mouseX + offsetX,
        mouseY + offsetY,
        mHandle,
        TRUE
    );
}

void Cursor::Finalize()
{
    if (mHandle != -1)
    {
        DeleteGraph(mHandle);
        mHandle = -1;
    }

    // Windowsのマウスカーソルを再表示
    SetMouseDispFlag(TRUE);
}