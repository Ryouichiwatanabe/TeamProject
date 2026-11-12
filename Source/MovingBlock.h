#pragma once
#include "StageBlock.h"
#include <DxLib.h>

class Player;

// �v���C���[���悹�ĉ����ړ����鏰�u���b�N
class MovingBlock : public StageBlock
{
public:
    ~MovingBlock() override = default;

    void Init() override;
    void Update(float _deltaTime) override;
    void Draw() override;

protected:
    // �摜���\�[�X�i�p�X��ς���ƌ����ڂ�ύX�\�j
    const char* GetModelFilename() const override
    {
        return "Resource/GameObject/StageBlock/Stage.png";
    }

private:
    Player* FindPlayerOnTop(const VECTOR& blockPos); // ��ɏ���Ă���v���C���[�̎擾

private:
    // �ړ��ݒ�
    static constexpr float MOVE_RANGE = 200.0f;       // �ړ����i�Г���px�j
    static constexpr float MOVE_SPEED = 2.0f;         // �ړ����x
    static constexpr bool  IS_VERTICAL_MOVE = false;  // true�ŏ㉺�ړ��Afalse�ō��E�ړ�

    VECTOR mvStartPos = VGet(0.0f, 0.0f, 0.0f); // �����ʒu
    bool   mbStartPosSaved = false;
    float  mfMoveTimer = 0.0f;
};