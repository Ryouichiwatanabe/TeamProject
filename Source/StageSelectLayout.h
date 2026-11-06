#pragma once

namespace StageSelectLayout
{
    // ステージ数
    constexpr int STAGE_COUNT = 8;

    // アイテム数
    constexpr int ITEM_COUNT = 3;

    // 完成料理画像の位置調整

    // 完成料理画像のX方向の位置補正
    // +で右、-で左
    const float CLEAR_IMAGE_OFFSET_X[STAGE_COUNT] =
    {
        // Stage1
        4.0f,

        // Stage2
        15.0f,

        // Stage3
        15.0f,

        // Stage4
        18.0f,

        // Stage5
        6.0f,

        // Stage6
        15.0f,

        // Stage7
        15.0f,

        // Stage8
        17.0f
    };


    // 完成料理画像のY方向の位置補正
    // +で下、-で上
    const float CLEAR_IMAGE_OFFSET_Y[STAGE_COUNT] =
    {
        // Stage1
        12.0f,

        // Stage2
        12.0f,

        // Stage3
        12.0f,

        // Stage4
        12.0f,

        // Stage5
        12.0f,

        // Stage6
        12.0f,

        // Stage7
        12.0f,

        // Stage8
        12.0f
    };

    // 取得アイテム画像の位置調整

    // アイテム画像のX方向の位置補正
    // +で右、-で左
    const float ITEM_OFFSET_X[STAGE_COUNT][ITEM_COUNT] =
    {
        // Stage1
        { 1.5f, 2.5f, 6.0f },

        // Stage2
        { 8.0f, 10.0f, 12.0f },

        // Stage3
        { 17.0f, 15.0f, 17.0f },

        // Stage4
        { 17.0f, 17.0f, 17.0f },

        // Stage5
        { 0.0f, 4.0f, 6.0f },

        // Stage6
        { 19.0f, 19.0f, 21.0f },

        // Stage7
        { 15.0f, 15.0f, 17.0f },

        // Stage8
        { 17.0f, 15.0f, 20.0f }
    };


    // アイテム画像のY方向の位置補正
    // +で下、-で上
    const float ITEM_OFFSET_Y[STAGE_COUNT][ITEM_COUNT] =
    {
        // Stage1
        { 0.0f, 0.0f, 0.0f },

        // Stage2
        { 0.0f, 0.0f, 0.0f },

        // Stage3
        { 0.0f, 0.0f, 0.0f },

        // Stage4
        { 0.0f, 0.0f, 0.0f },

        // Stage5
        { 0.0f, 0.0f, 0.0f },

        // Stage6
        { 0.0f, 0.0f, 0.0f },

        // Stage7
        { 0.0f, 0.0f, 0.0f },

        // Stage8
        { 0.0f, 0.0f, 0.0f }
    };
}