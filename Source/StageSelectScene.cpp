#include "StageSelectScene.h"

#include "Master.h"
#include "SceneManager.h"
#include "DxLib.h"
#include "InputManager.h"
#include "SoundManager.h"
#include "StageClearData.h"
#include "StageSelectData.h"
#include "CollectibleData.h"
#include "CsvLoader.h"

#include <string>


void StageSelectScene::Initialize()
{
    // 前フレームのマウス左クリック状態を取得
    mbPreviousMouseLeft =
        (GetMouseInput() & MOUSE_INPUT_LEFT) != 0;

    // 最初はステージ1を選択状態にする
    mnSelectedStage = 0;

    // ステージ選択画面の背景画像を読み込む
    mBackgroundHandle =
        LoadGraph(
            "Resource/StageSelect/StageSelectMaterial/notClear.png"
        );

    // 各ステージのクリア画像を読み込む
    //
    // [0] → Stage1
    // [1] → Stage2
    // ...
    // [7] → Stage8
    for (int i = 0; i < 8; i++)
    {
        mStageClearHandles[i] =
            LoadGraph(
                StageSelectData::GetClearImageFilename(
                    i + 1
                ).c_str()
            );
    }

    // 各ステージで使用する3種類のアイテム画像を読み込む
    // [ステージ番号 - 1][アイテム番号]
    // 取得済みのアイテムだけをステージ選択画面の下側に表示する
    for (int stageIndex = 0; stageIndex < 8; stageIndex++)
    {
        for (int itemIndex = 0; itemIndex < 3; itemIndex++)
        {
            mCollectibleHandles[stageIndex][itemIndex] =
                LoadGraph(
                    GetCollectibleFilename(
                        stageIndex + 1,
                        itemIndex
                    ).c_str()
                );
        }
    }
}


void StageSelectScene::Update(float _deltaTime)
{
    int mouseX = 0;
    int mouseY = 0;

    // 現在のマウス座標を取得
    GetMousePoint(
        &mouseX,
        &mouseY
    );

    // 現在のマウス左クリック状態を取得
    bool currentMouseLeft =
        (GetMouseInput() & MOUSE_INPUT_LEFT) != 0;

    // 左クリックした瞬間だけtrueにする
    bool mouseLeftDown =
        currentMouseLeft &&
        !mbPreviousMouseLeft;

    if (mouseLeftDown)
    {
        // 8ステージすべてのクリック範囲を確認する
        for (int i = 0; i < 8; i++)
        {
            if (!IsMouseInsideStageButton(
                i,
                mouseX,
                mouseY))
            {
                continue;
            }

            // 選択したステージを保存
            mnSelectedStage = i;

            // 選択したステージ番号をSceneManagerへ渡す
            Master::mpSceneManager->SetStageNumber(
                i + 1
            );

            // ゲームシーンへ移動
            Master::mpSceneManager->RequestScene(
                SCENE_TYPE::GAME_SCENE
            );

            break;
        }
    }

    // 次のフレーム用に現在のクリック状態を保存
    mbPreviousMouseLeft =
        currentMouseLeft;

    Scene::Update(_deltaTime);
}


void StageSelectScene::Draw()
{
    // ステージ選択画面の背景を描画
    DrawBackground();

    // クリアしていないステージに、
    // 取得済みのアイテムを表示する
    DrawStageCollectibleImages();

    // 完全クリア済みのステージに、
    // 完成した料理画像を表示する
    DrawStageClearImages();

    // Sceneが管理しているオブジェクトなどを描画
    Scene::Draw();
}


void StageSelectScene::DrawBackground()
{
    if (mBackgroundHandle == -1)
        return;

    // 今回は1920×1080固定なので、
    // 背景画像を画面いっぱいにそのまま描画する
    DrawExtendGraph(
        0,
        0,
        1920,
        1080,
        mBackgroundHandle,
        TRUE
    );
}

void StageSelectScene::DrawStageClearImages()
{
    for (int i = 0; i < 8; i++)
    {
        int stageNumber = i + 1;

        // 完全クリアしたステージだけ完成料理画像を表示
        if (!StageClearData::IsStageCleared(stageNumber))
            continue;

        int clearGraphHandle = mStageClearHandles[i];

        if (clearGraphHandle == -1)
            continue;

        const StageClearImage& image =
            mStageClearImages[i];

        DrawExtendGraph(
        static_cast<int>(
            image.x + StageSelectLayout::CLEAR_IMAGE_OFFSET_X[i]
        ),
        static_cast<int>(
            image.y + StageSelectLayout::CLEAR_IMAGE_OFFSET_Y[i]
        ),
        static_cast<int>(
            image.x + image.width + StageSelectLayout::CLEAR_IMAGE_OFFSET_X[i]
        ),
        static_cast<int>(
            image.y + image.height + StageSelectLayout::CLEAR_IMAGE_OFFSET_Y[i]
        ),
            clearGraphHandle,
            FALSE

        );
    }
}


void StageSelectScene::DrawStageCollectibleImages()
{
    // 8ステージ分を確認する
    for (int stageIndex = 0;
        stageIndex < 8;
        stageIndex++)
    {
        int stageNumber =
            stageIndex + 1;

        // 完全クリア済みなら、
        // 料理画像が表示されるためアイテムは表示しない
        if (StageClearData::IsStageCleared(
            stageNumber))
        {
            continue;
        }

        const StageButton& button =
            mStageButtons[stageIndex];

        // 3種類のアイテムを確認する
        for (int itemIndex = 0;
            itemIndex < 3;
            itemIndex++)
        {
            // まだ取得していないアイテムは表示しない
            if (!StageClearData::IsItemCollected(
                stageNumber,
                itemIndex))
            {
                continue;
            }

            int graphHandle =
                mCollectibleHandles
                [stageIndex][itemIndex];

            if (graphHandle == -1)
                continue;

            int imageWidth = 0;
            int imageHeight = 0;

            GetGraphSize(
                graphHandle,
                &imageWidth,
                &imageHeight
            );

            if (imageWidth <= 0 ||
                imageHeight <= 0)
            {
                continue;
            }

            float itemX =
                button.x +
                button.width *
                (0.20f + itemIndex * 0.30f) +
                StageSelectLayout::ITEM_OFFSET_X
                [stageIndex][itemIndex];

            float itemY =
                button.y +
                button.height * 0.88f +
                StageSelectLayout::ITEM_OFFSET_Y
                [stageIndex][itemIndex];

            // アイテム画像の大きさを揃えるため、
            // 縦横の大きい方を基準にする
            int maxImageSize =
                (imageWidth > imageHeight)
                ? imageWidth
                : imageHeight;

            float itemScale =
                (button.width * 0.20f) /
                static_cast<float>(maxImageSize);

            int drawItemWidth =
                static_cast<int>(
                    imageWidth * itemScale
                    );

            int drawItemHeight =
                static_cast<int>(
                    imageHeight * itemScale
                    );

            DrawExtendGraph(
                static_cast<int>(
                    itemX - drawItemWidth / 2
                    ),
                static_cast<int>(
                    itemY - drawItemHeight / 2
                    ),
                static_cast<int>(
                    itemX + drawItemWidth / 2
                    ),
                static_cast<int>(
                    itemY + drawItemHeight / 2
                    ),
                graphHandle,
                TRUE
            );
        }
    }
}


bool StageSelectScene::IsMouseInsideStageButton(
    int _stageIndex,
    int _mouseX,
    int _mouseY)
{
    // 不正なステージ番号の場合
    if (_stageIndex < 0 ||
        _stageIndex >= 8)
    {
        return false;
    }

    const StageButton& button =
        mStageButtons[_stageIndex];

    // 1920×1080の画面座標をそのまま使用して、
    // ステージカードの範囲内か判定する
    return
        _mouseX >= button.x &&
        _mouseX <= button.x + button.width &&
        _mouseY >= button.y &&
        _mouseY <= button.y + button.height;
}


void StageSelectScene::Finalize()
{
    // タイトルBGMを停止
    SoundManager::GetInstance()->StopBGM();

    // ステージ選択背景画像を解放
    if (mBackgroundHandle != -1)
    {
        DeleteGraph(mBackgroundHandle);
        mBackgroundHandle = -1;
    }

    // 各ステージのクリア画像を解放
    for (int i = 0; i < 8; i++)
    {
        if (mStageClearHandles[i] != -1)
        {
            DeleteGraph(
                mStageClearHandles[i]
            );

            mStageClearHandles[i] = -1;
        }
    }

    // ステージ選択画面で使用した
    // アイテム画像をすべて解放
    for (int stageIndex = 0;
        stageIndex < 8;
        stageIndex++)
    {
        for (int itemIndex = 0;
            itemIndex < 3;
            itemIndex++)
        {
            if (mCollectibleHandles
                [stageIndex][itemIndex] != -1)
            {
                DeleteGraph(
                    mCollectibleHandles
                    [stageIndex][itemIndex]
                );

                mCollectibleHandles
                    [stageIndex][itemIndex] = -1;
            }
        }
    }
}