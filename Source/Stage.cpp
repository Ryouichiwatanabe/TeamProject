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
#include "Item.h"
#include "SpongeBlock.h" // キッチンスポンジのバネ床
#include "MovingBlock.h" // 溶けバターの滑る床


#include "Transform.h"

#include "GameConfig.h"

#include <DxLib.h>  // VECTOR用

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

                auto transform = goalBlock->GetModule<Transform>();
                const VECTOR baseScale = transform->GetScale();
                const VECTOR addScale = VGet(0.5f, 0.5f, 0.0f);
                transform->SetScale(VAdd(baseScale, addScale));

                transform->SetPosition(initPosition);

                break;
            }

            case 3:
            {
             
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
                auto player = objectManager->CreateObject<Player>();
                player->GetModule<Transform>()->SetPosition(initPosition);
                player->UpdatePieceTransforms();    // Pieceに反映
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
                auto item = objectManager->CreateObject<Item>(mStageNumber, tile - 9);
                item->GetModule<Transform>()->SetPosition(initPosition);
                break;

            }
            case 12:
            {
                // ★キッチンスポンジのバネ床を生成
                auto sponge = objectManager->CreateObject<SpongeBlock>();
                sponge->GetModule<Transform>()->SetPosition(initPosition);
                break;
            }
            case 13:
            {
                // ★溶けバターのツルツル滑る床を生成
                auto butter = objectManager->CreateObject<MovingBlock>();
                butter->GetModule<Transform>()->SetPosition(initPosition);
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