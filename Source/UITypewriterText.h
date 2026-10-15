#pragma once
#include <string>
#include <mbstring.h>
#include "DxLib.h"
#include "FloatingMotion.h"

class UI;

class UITypewriterText 
{
public:

    void Initialize(UI* _pUI);
    void Update();

    void Start()
    {
        mVisibleByteCount = 0;
        mFrameTimer = 0;
        mIsFinished = false;
    }

    // 全文表示へスキップ（キー入力時用）
    void Skip();

    std::string GetVisibleText();   // 描画する分の文字列を返す

    void SetFrameInterval(int _interval) { mFrameInterval = _interval; }

    bool IsFinished() const { return mIsFinished; }

private:
    UI* mpUI = nullptr;

    // std::string mFullText;          // 描画する全ての文字列

    size_t mVisibleByteCount = 0;   // 表示対象のバイト数
    int mFrameInterval = 4;         // 次の文字が出るまでの待機フレーム
    int mFrameTimer = 0;            // タイマー
    bool mIsFinished = false;       // 文字送りが終わったかどうかを示す
};