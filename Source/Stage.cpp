#include "Stage.h"

#include "CsvLoader.h"

// Obj生成用
#include "Master.h"
#include "SceneManager.h"
#include "Scene.h"
#include "ObjectManager.h"
#include "Player.h"

// 生成するObj関係
#include "Player.h"
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

void Stage::Load(const std::string& _filename)
{
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
                (x * GameConfig::CELL_SIZE) + GameConfig::CELL_SIZE,
                (y * GameConfig::CELL_SIZE) + GameConfig::CELL_SIZE,
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
            {
                // Collectibleを生成
                auto collectible = objectManager
                    ->CreateObject<Collectible>();

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