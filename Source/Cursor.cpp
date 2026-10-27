#include "Cursor.h"
#include "DxLib.h"

namespace
{
    constexpr int offsetX = -10; // �E�B���h�E�g�̑������A���֊񂹂�
    constexpr int offsetY = -1;  // �e�L�X�g�x�[�X���C���̔�����
}

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

    // �J�[�\���摜���ǂݍ���
    mHandle = LoadGraph("Resource/Cursor4.png");
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