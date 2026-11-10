#pragma once
#include "ScreenConfig.h"

// 湯気エフェクトのステージ別「位置」「範囲」設定ヘッダー
namespace EffectSteamConfig
{
    // 1ステージ分の湯気設定データ構造体
    struct StageSteamParam
    {
        float fCenterX;      // 湯気が出る中心X座標
        float fCenterY;      // 湯気が出る中心Y座標
        float fSpawnRadiusX; // 横の範囲（中心から左右±何pxから出るか）
        float fSpawnMinY;    // 縦の範囲・上端（中心から上方向何pxか）
        float fSpawnMaxY;    // 縦の範囲・下端（中心から下方向何pxか）
        int   nWispCount;    // 同時に立ち上る湯気の筋の本数（0にすると湯気なし）
    };

    // 画面中央の基準値
    inline constexpr float HALF_RATIO = 0.5f;
    inline constexpr float SCREEN_CENTER_X = ScreenConfig::SCREEN_WIDTH * HALF_RATIO; // 960.0f
    inline constexpr float SCREEN_CENTER_Y = ScreenConfig::SCREEN_HEIGHT * HALF_RATIO; // 540.0f

    // 湯気を出さないステージ用の定数（本数 0）
    inline constexpr StageSteamParam NO_STEAM = {
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0
    };

    // =========================================================================
    // ★ ステージごとの位置・範囲の定数定義（ここを書き換えて調整！）
    //   { 中心X, 中心Y, 横範囲(半径X), 縦範囲(上端Y), 縦範囲(下端Y), 湯気の本数 }
    //   ※湯気がいらないステージは「= NO_STEAM;」にするだけ！
    // =========================================================================

    // ★チュートリアル (Stage 0：湯気なし)
    inline constexpr StageSteamParam STAGE_0 = NO_STEAM;

    // ★ステージ 1（湯気なし）
    inline constexpr StageSteamParam STAGE_1 = NO_STEAM;

    // ステージ 2
    inline constexpr StageSteamParam STAGE_2 = {
        SCREEN_CENTER_X,          // 中心X (960.0f)
        SCREEN_CENTER_Y + 80.0f,  // 中心Y (620.0f)
        320.0f,                   // 横の範囲（左右±320px）
        -40.0f,                   // 縦の範囲（上端）
        100.0f,                   // 縦の範囲（下端）
        22                        // 湯気の本数
    };

    // ステージ 3
    inline constexpr StageSteamParam STAGE_3 = {
        SCREEN_CENTER_X,          // 中心X
        SCREEN_CENTER_Y + 60.0f,  // 中心Y
        360.0f,                   // 横の範囲
        -50.0f,                   // 縦の範囲（上端）
        110.0f,                   // 縦の範囲（下端）
        24                        // 湯気の本数
    };

    // ステージ 4
    inline constexpr StageSteamParam STAGE_4 = {
        SCREEN_CENTER_X,          // 中心X
        SCREEN_CENTER_Y + 80.0f,  // 中心Y
        320.0f,                   // 横の範囲
        -40.0f,                   // 縦の範囲（上端）
        100.0f,                   // 縦の範囲（下端）
        22                        // 湯気の本数
    };

    // ステージ 5
    inline constexpr StageSteamParam STAGE_5 = {
        SCREEN_CENTER_X,          // 中心X
        SCREEN_CENTER_Y + 80.0f,  // 中心Y
        320.0f,                   // 横の範囲
        -40.0f,                   // 縦の範囲（上端）
        100.0f,                   // 縦の範囲（下端）
        22                        // 湯気の本数
    };

    // ステージ 6
    inline constexpr StageSteamParam STAGE_6 = {
        SCREEN_CENTER_X,          // 中心X
        SCREEN_CENTER_Y + 80.0f,  // 中心Y
        320.0f,                   // 横の範囲
        -40.0f,                   // 縦の範囲（上端）
        100.0f,                   // 縦の範囲（下端）
        22                        // 湯気の本数
    };

    // ステージ 7
    inline constexpr StageSteamParam STAGE_7 = {
        SCREEN_CENTER_X,          // 中心X
        SCREEN_CENTER_Y + 80.0f,  // 中心Y
        320.0f,                   // 横の範囲
        -40.0f,                   // 縦の範囲（上端）
        100.0f,                   // 縦の範囲（下端）
        22                        // 湯気の本数
    };

    // ★ステージ 8（湯気なし）
    inline constexpr StageSteamParam STAGE_8 = NO_STEAM;

    // ステージ番号に応じた設定を返す関数
    inline constexpr StageSteamParam GetStageParam(int _stageNumber)
    {
        switch (_stageNumber)
        {
        case 0:  return STAGE_0;
        case 1:  return STAGE_1;
        case 2:  return STAGE_2;
        case 3:  return STAGE_3;
        case 4:  return STAGE_4;
        case 5:  return STAGE_5;
        case 6:  return STAGE_6;
        case 7:  return STAGE_7;
        case 8:  return STAGE_8;
        default: return NO_STEAM;
        }
    }
}