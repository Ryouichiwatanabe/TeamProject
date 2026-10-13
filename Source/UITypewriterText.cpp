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

    // 文字送りの処理
    if (mVisibleByteCount < mFullText.length()) {
        unsigned char c = static_cast<unsigned char>(mFullText[mVisibleByteCount]);

        // 全角文字（先行バイト）なら2バイト進め、半角なら1バイト進める
        if (_ismbblead(c)) {
            // 安全のため終端チェック
            if (mVisibleByteCount + 2 <= mFullText.length()) {
                mVisibleByteCount += 2;
            }
            else {
                mVisibleByteCount = mFullText.length();
            }
        }
        else {
            mVisibleByteCount += 1;
        }

        // 最後まで表示し終えたか判定
        if (mVisibleByteCount >= mFullText.length()) {
            mVisibleByteCount = mFullText.length();
            mIsFinished = true;
        }
    }
}

std::string UITypewriterText::GetVisibleText()
{
    return mpUI->GetText().GetText().substr(0, mVisibleByteCount);
}