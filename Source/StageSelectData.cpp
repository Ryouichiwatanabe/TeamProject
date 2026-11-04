#include "StageSelectData.h"

std::string StageSelectData::GetClearImageFilename(
    int _stageNumber
)
{
    switch (_stageNumber)
    {
    case 1:
        return "Resource/StageSelect/StageSelectMaterial/Stage1.png";

    case 2:
        return "Resource/StageSelect/StageSelectMaterial/Stage2.png";

    case 3:
        return "Resource/StageSelect/StageSelectMaterial/Stage3.png";

    case 4:
        return "Resource/StageSelect/StageSelectMaterial/Stage4.png";

    case 5:
        return "Resource/StageSelect/StageSelectMaterial/Stage5.png";

    case 6:
        return "Resource/StageSelect/StageSelectMaterial/Stage6.png";

    case 7:
        return "Resource/StageSelect/StageSelectMaterial/Stage7.png";

    case 8:
        return "Resource/StageSelect/StageSelectMaterial/Stage8.png";

    default:
        return "";
    }
}