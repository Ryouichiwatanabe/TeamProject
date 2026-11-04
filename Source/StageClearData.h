#pragma once

class StageClearData
{
public:
    // ステージをクリア済みとして保存する
    static void SetStageCleared(
        int _stageNumber,
        bool _isCleared
    );

    // 指定したステージをクリア済みか確認する
    static bool IsStageCleared(
        int _stageNumber
    );

    // ステージ内のアイテムを取得済みとして保存する
    // _itemIndex は 0～2 のアイテム番号
    static void SetItemCollected(
        int _stageNumber,
        int _itemIndex,
        bool _isCollected
    );

    // 指定したステージのアイテムを取得済みか確認する
    static bool IsItemCollected(
        int _stageNumber,
        int _itemIndex
    );

private:
    // 各ステージのクリア状態を保存する
    // [0] = Stage1、[7] = Stage8
    static bool mbIsStageCleared[8];

    // 各ステージで取得したアイテムを保存する
    // [ステージ番号 - 1][アイテム番号]
    static bool mbIsItemCollected[8][3];
};