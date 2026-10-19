#include "UITypewriterText.h"

#include "UI.h"

void UITypewriterText::Initialize(UI* _pUI)
{
    mpUI = _pUI;
}

void UITypewriterText::Update()
{
    if (mIsFinished)
        return;

    // フレームカウントによるウェイト処理
    if (mFrameTimer > 0) {
        mFrameTimer--;
        return;
    }
    mFrameTimer = mFrameInterval;

    auto text = mpUI->GetText().GetText();

    // 文字送りの処理
    if (mVisibleByteCount < text.length()) {
        unsigned char c = static_cast<unsigned char>(text[mVisibleByteCount]);

        // 全角文字（先行バイト）なら2バイト進め、半角なら1バイト進める
        if (_ismbblead(c)) {
            // 安全のため終端チェック
            if (mVisibleByteCount +mBite <= text.length()) {
                mVisibleByteCount += 2;
            }
            else {
                mVisibleByteCount = text.length();
            }
        }
        else {
            mVisibleByteCount += 1;
        }

        // 最後まで表示し終えたか判定
        if (mVisibleByteCount >= text.length()) {
            mVisibleByteCount = text.length();
            mIsFinished = true;
        }
    }
}

std::string UITypewriterText::GetVisibleText()
{
    return mpUI->GetText().GetText().substr(0, mVisibleByteCount);
}

void UITypewriterText::Skip()
{
    auto text = mpUI->GetText().GetText();

    mVisibleByteCount = text.size();
    mIsFinished = true;
}