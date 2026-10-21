#include "CollectibleData.h"

#include <array>

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
}

std::string GetCollectibleFilename(
    int _stageNumber,
    int _itemIndex)
{
    if (_stageNumber < 1 ||
        _stageNumber > STAGE_COUNT)
    {
        return "";
    }

    if (_itemIndex < 0 ||
        _itemIndex >= COLLECTIBLE_TYPE_COUNT)
    {
        return "";
    }

    return COLLECTIBLE_FILES[
        _stageNumber - 1
    ][
        _itemIndex
    ];
}