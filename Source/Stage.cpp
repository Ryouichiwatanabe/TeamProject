#include "Stage.h"

#include "CsvLoader.h"
#include <string>
#include <array>

// Obj生成用
#include "Master.h"
#include "SceneManager.h"
#include "Scene.h"
#include "ObjectManager.h"
#include "Player.h"

// 生成するObj関係
#include "StageBlock.h"
#include "Goal.h"
#include "MovingBlock.h"
#include "Spike.h"
#include "DisappearingBlock.h"
#include "Switch.h"
#include "SwitchBlock.h"
#include "Collectible.h"

#include "Transform.h"

#include "GameConfig.h"

#include <DxLib.h>  // VECTOR用

namespace
{
    constexpr int COLLECTIBLE_TYPE_COUNT = 3;
    constexpr int STAGE_COUNT = 8;

    using CollectibleFiles =
        std::array<std::string, COLLECTIBLE_TYPE_COUNT>;

    const std::array<CollectibleFiles, STAGE_COUNT>
        COLLECTIBLE_FILES =
    { {
        // Stage1
        {{
            "Resource/Collectible/Stage1/Negi.png",
            "Resource/Collectible/Stage1/Syouga.png",
            "Resource/Collectible/Stage1/Syouyu.png"
        }},

        // Stage2
        {{
            "Resource/Collectible/Stage2/Konbu.png",
            "Resource/Collectible/Stage2/Negi.png",
            "Resource/Collectible/Stage2/Ponzu.png"
        }},

        // Stage3
        {{
            "Resource/Collectible/Stage3/Oil.png",
            "Resource/Collectible/Stage3/Salt.png",
            "Resource/Collectible/Stage3/Water.png"
        }},

        // Stage4
        {{
            "Resource/Collectible/Stage4/Miso.png",
            "Resource/Collectible/Stage4/Negi.png",
            "Resource/Collectible/Stage4/Wakame.png"
        }},

        // Stage5
        {{
            "Resource/Collectible/Stage5/Garlic.png",
            "Resource/Collectible/Stage5/Hikiniku.png",
            "Resource/Collectible/Stage5/Toubanjan.png"
        }},

        // Stage6
        {{
            "Resource/Collectible/Stage6/Gyuniku.png",
            "Resource/Collectible/Stage6/Negi.png",
            "Resource/Collectible/Stage6/Tare.png"
        }},

        // Stage7
        {{
            "Resource/Collectible/Stage7/Mentuyu.png",
            "Resource/Collectible/Stage7/Negi.png",
            "Resource/Collectible/Stage7/Oil.png"
        }},

        // Stage8
        {{
            "Resource/Collectible/Stage8/Kukonomi.png",
            "Resource/Collectible/Stage8/Kyouninsou.png",
            "Resource/Collectible/Stage8/Milk.png"
        }}
    } };

    std::string GetCollectibleFilename(
        int _stageNumber,
        int _tile)
    {
        if (_stageNumber < 1 || _stageNumber > STAGE_COUNT)
        {
            return "";
        }

        if (_tile < 9 || _tile > 11)
        {
            return "";
        }

        return COLLECTIBLE_FILES[
            _stageNumber - 1
        ][
            _tile - 9
        ];
    }
}

std::string Stage::GetCollectibleFilename(
    int _itemIndex) const
{
    if (_itemIndex < 0 ||
        _itemIndex >= COLLECTIBLE_TYPE_COUNT)
    {
        return "";
    }

    return ::GetCollectibleFilename(
        mStageNumber,
        _itemIndex + 9
    );
}

void Stage::Load(const std::string& _filename)
{
    // ファイル名からステージ番号を取得
    size_t stagePos = _filename.rfind("Stage");

    if (stagePos != std::string::npos)
    {
        stagePos += 5;

        size_t extensionPos = _filename.find(".csv", stagePos);

        if (extensionPos != std::string::npos)
        {
            std::string stageNumber =
                _filename.substr(
                    stagePos,
                    extensionPos - stagePos
                );

            mStageNumber = std::stoi(stageNumber);
        }
    }

    mStageData = CsvLoader::Load(_filename);

    if (mStageData.empty())
    {
        OutputDebugStringA("Stage CSVが読み込めていません\n");
    }
    else
    {
        OutputDebugStringA("Stage CSV読み込み成功\n");
    }

    CreateObjects();
}

void Stage::CreateObjects()
{
    auto* objectManager =
        Master::mpSceneManager
        ->GetCurrentScene()
        ->GetObjectManager();

    // オブジェクトを生成
    for (int y = 0; y < mStageData.size(); y++)
    {
        for (int x = 0; x < mStageData[y].size(); x++)
        {
            const int tile = mStageData[y][x];
            VECTOR initPosition = VGet(
                (x * GameConfig::CELL_SIZE) + GameConfig::CELL_SIZE / 2.0f,
                (y * GameConfig::CELL_SIZE) + GameConfig::CELL_SIZE / 2.0f,
                0.0f
            );

            switch (tile)
            {
            case 0:
                // 何も生成しない
                break;

            case 1:
            {
                // 床を生成
                auto stageBlock = objectManager
                    ->CreateObject<StageBlock>();

                stageBlock->GetModule<Transform>()
                    ->SetPosition(initPosition);
                break;
            }
            case 2:
            {
                // ゴールを生成
                auto goalBlock = objectManager
                    ->CreateObject<Goal>();

                goalBlock
                    ->GetModule<Transform>()
                    ->SetPosition(initPosition);

                break;
            }

            case 3:
            {
                // 動く床を生成
                objectManager
                    ->CreateObject<MovingBlock>(
                        300.0f,
                        800.0f,
                        2.0f
                    );

                break;
            }

            case 4:
            {
                // 針を生成
                auto spikeBlock = objectManager
                    ->CreateObject<Spike>();
                spikeBlock
                    ->GetModule<Transform>()
                    ->SetPosition(initPosition);
                break;
            }

            case 5:
            {
                // リスポーン地点を設定
                /*mvRespawnPos = VGet(
                    (x * GameConfig::CELL_SIZE) + GameConfig::CELL_SIZE / 2.0f,
                    (y * GameConfig::CELL_SIZE) + GameConfig::CELL_SIZE / 2.0f,
                    0.0f
                );*/

                break;
            }

            case 6:
            {
                // 消える床を生成
                auto disappearingBlock = objectManager
                    ->CreateObject<DisappearingBlock>();
                disappearingBlock
                    ->GetModule<Transform>()
                    ->SetPosition(initPosition);

                break;
            }

            case 7:
            {
                // スイッチを生成
                auto switchBlock = objectManager
                    ->CreateObject<Switch>();

                break;
            }

            case 8:
            {
                // SwitchBlockは2回目に生成する
                break;

            }

            case 9:
            case 10:
            case 11:
            {
                std::string filename =
                    ::GetCollectibleFilename(
                        mStageNumber,
                        tile
                    );

                int itemIndex = tile - 9;

                auto collectible =
                    objectManager->CreateObject<Collectible>(
                        filename,
                        itemIndex
                    );

                collectible
                    ->GetModule<Transform>()
                    ->SetPosition(initPosition);

                break;
            }

            default:
                break;
            }
        }
    }

    // Switchを取得
    Switch* pSwitch = objectManager->FindObject<Switch>();

    if (pSwitch == nullptr)
        return;

    // SwitchBlockを生成
    for (int y = 0; y < mStageData.size(); y++)
    {
        for (int x = 0; x < mStageData[y].size(); x++)
        {
            const int tile = mStageData[y][x];

            if (tile != 8)
                continue;

            objectManager
                ->CreateObject<SwitchBlock>(
                    pSwitch
                );
        }
    }
}