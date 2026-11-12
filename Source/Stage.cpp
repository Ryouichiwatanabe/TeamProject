#include "Stage.h"

#include "CsvLoader.h"

#include "Master.h"
#include "SceneManager.h"
#include "Scene.h"
#include "ObjectManager.h"
#include "Player.h"

// -- 生成するObj -- //
#include "Player.h"
#include "StageBlock.h"
#include "Goal.h"
#include "MovingBlock.h"
#include "Spike.h"
#include "Item.h"
#include "Konjac.h" 
#include "MovingBlock.h"

#include "Transform.h"

// -- 定数 -- //
#include "GameConfig.h"

#include <DxLib.h>  // VECTOR

void Stage::Load(const std::string& _filename)
{
    size_t stagePos = _filename.rfind("Stage");

    if (stagePos != std::string::npos)
    {
        stagePos += 5;

        size_t extensionPos =
            _filename.find(".csv", stagePos);

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
                // -- 床の生成 -- //
                auto stageBlock = objectManager->CreateObject<StageBlock>();
                stageBlock->GetModule<Transform>()->SetPosition(initPosition);
                break;
            }

            case 2:
            {
                // -- ゴールを生成 -- //
                auto goalBlock = objectManager->CreateObject<Goal>();
                auto transform = goalBlock->GetModule<Transform>();
                const VECTOR baseScale = transform->GetScale();
                const VECTOR addScale = VGet(0.5f, 0.5f, 0.0f);
                transform->SetScale(VAdd(baseScale, addScale));
                transform->SetPosition(initPosition);

                break;
            }

            case 4:
            {
                // -- 針を生成 -- //
                auto spikeBlock = objectManager->CreateObject<Spike>();
                spikeBlock->GetModule<Transform>()->SetPosition(initPosition);
                break;
            }

            case 5:
            {
                // -- プレイヤーの生成 -- //
                auto player = objectManager->CreateObject<Player>(initPosition);
                player->GetModule<Transform>()->SetPosition(initPosition);
                player->UpdatePieceTransforms();    // Pieceに反映
                break;
            }

            case 9:
            case 10:
            case 11:
            {
                // -- アイテムの生成 -- //
                auto item = objectManager->CreateObject<Item>(mStageNumber, tile - 9);
                item->GetModule<Transform>()->SetPosition(initPosition);
                break;
            }

            case 12:
            {
                // -- コンニャクを生成 -- //
                auto konjacBlock = objectManager->CreateObject<Konjac>();
                konjacBlock->GetModule<Transform>()->SetPosition(initPosition);
                break;
            }

            case 13:
            {
                // -- 移動する床の生成 -- //
                auto movingBlock = objectManager->CreateObject<MovingBlock>();
                movingBlock->GetModule<Transform>()->SetPosition(initPosition);
                break;
            }

            default:
                break;
            }
        }
    }
}