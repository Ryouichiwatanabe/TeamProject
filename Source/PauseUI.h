#pragma once
#include "UI.h"
#include <string>

class OperationGuideUI;

class PauseUI : public UI
{
public:
    void Init() override;
    void Update() override;

    void OnPauseInput();
    void AddMenuContent(UI* _ui);
    void SetResumeOnly(bool _resumeOnly) { mbResumeOnly = _resumeOnly; }

private:
    enum class State { Menu, Guide };

    // ★追加：付箋1枚分の設定をまとめる構造体
    struct StickyNoteDesc
    {
        const char* graphPath;  // 付箋画像
        const char* text;       // 表示する文字
        VECTOR      position;   // 画面中央からのずれ
        float       scale;      // 拡大率
        float       rotation;   // 傾き（ラジアン：時計回りが＋）
    };

    void ChangeState(State _next);
    void UpdateMenu();
    void UpdateGuide();
    UI* CreateStickyNote(const StickyNoteDesc& _desc);  // ★CreateMenuButtonから置き換え

private:
    State mState = State::Menu;

    UI* mpPauseOverlay = nullptr;
    UI* mpMenuRoot = nullptr;
    UI* mpFridgeUI = nullptr;           // ★追加：冷蔵庫の背景
    UI* mpResumeUI = nullptr;
    UI* mpStageSelectUI = nullptr;
    UI* mpTitleUI = nullptr;
    UI* mpOperationButton = nullptr;
    OperationGuideUI* mpGuideUI = nullptr;

    bool mbResumeOnly = false;
};