#include "Collectible.h"

#include "Transform.h"
#include "Graphic.h"
#include "Collider.h"

Collectible::Collectible(const std::string& _filename,int _itemIndex)
    : mFilename(_filename),
    mItemIndex(_itemIndex)
{
}

void Collectible::Init()
{
    // �擾�A�C�e���̃^�O��ݒ�
    SetTag(Tag::COLLECTIBLE);

    // �v���C���[�Ƃ̏Փ˂����m����
    auto collider = GetModule<Collider>();
    if (collider != nullptr)
    {
        collider->AddCollisionTag(Tag::PLAYER_CELL);
    }
}

void Collectible::InitComponent()
{
    // ���W�E��]�E�g�嗦
    AddModule<Transform>();

    // �擾�A�C�e���̉摜
    // ���摜�p�X�͎��ۂ̃t�@�C�����ɍ��킹�ĕύX����
    AddModule<Graphic>("Resource/Cursor2.png");

    // �����蔻��
    AddModule<Collider>();

    auto collider = GetModule<Collider>();
    if (collider != nullptr)
    {

        collider->SetHalfSize(VGet(25.0f, 25.0f, 0.0f));
    
    }
}

void Collectible::Update(float _deltaTime)
{
    // �|�[�Y���͏������Ȃ�
    if (_deltaTime <= 0.0f)
    {
        return;
    }

    // �擾���o��
    if (mbIsCollectEffect)
    {
        mCollectTime += _deltaTime;

        auto transform = GetModule<Transform>();
        if (transform != nullptr)
        {
            float progress =
                mCollectTime / mCollectDuration;

            float scale = 1.0f;

            // �O���F�����傫���Ȃ�
            if (progress < 0.35f)
            {
                float effectProgress =
                    progress / 0.35f;

                scale = 1.0f + effectProgress * 0.15f;
            }
            // �㔼�F�������Ȃ��ď�����
            else
            {
                float effectProgress =
                    (progress - 0.35f) / 0.65f;

                scale = 1.15f * (1.0f - effectProgress);
            }

            transform->SetScale(
                VGet(scale, scale, 1.0f)
            );
        }

        // ���o�I��
        if (mCollectTime >= mCollectDuration)
        {
            mbIsCollected = true;
            Destroy();
        }

        return;
    }

    // �擾�ς݂Ȃ牽�����Ȃ�
    if (mbIsCollected)
    {
        return;
    }

    // GameObject�̍X�V
    GameObject::Update(_deltaTime);

    auto collider = GetModule<Collider>();
    if (collider == nullptr)
    {
        return;
    }

    // �v���C���[�ɐG�ꂽ��擾���o�J�n
    if (collider->IsColliding(Tag::PLAYER_CELL))
    {
        mbIsPickedUp = true;
        mbIsCollectEffect = true;
        mCollectTime = 0.0f;

        return;
    }

    // �A�C�e���̏㉺�ړ�
    mMoveTime += _deltaTime;

    auto transform = GetModule<Transform>();
    if (transform != nullptr)
    {
        VECTOR position = transform->GetPosition();

        // �ŏ��̈ʒu���㉺�ړ��̊�ʒu�Ƃ��ĕۑ�
        if (!mbIsBasePositionSet)
        {
            mBaseY = position.y;
            mbIsBasePositionSet = true;
        }

        position.y =
            mBaseY + sinf(mMoveTime * 3.0f) * 5.0f;

        transform->SetPosition(position);
    }
}

void Collectible::Draw()
{
    // �擾�ς݂Ȃ�`�悵�Ȃ�
    if (mbIsCollected)
    {
        return;
    }

    GameObject::Draw();
}