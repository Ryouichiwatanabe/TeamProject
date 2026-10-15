#pragma once
#include <string>
#include <mbstring.h>
#include "DxLib.h"
#include "FloatingMotion.h"

class TypewriterText {
public:
    TypewriterText(int frameInterval = 4)
        : mFrameInterval(frameInterval), mFrameTimer(0), mVisibleByteCount(0), mIsFinished(false) {
    }

    // 文字列をセットして開始
    void SetText(const std::string& text) {
        mFullText = text;
        mVisibleByteCount = 0;
        mFrameTimer = 0;
        mIsFinished = mFullText.empty();
    }

    void Update() {
        if (mIsFinished) {
            return;
        }

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

    void Draw(int x, int y, unsigned int color, int fontHandle = -1) const {
        std::string sub = mFullText.substr(0, mVisibleByteCount);
        int drawY = y + static_cast<int>(mMotion.GetOffsetY());

        if (fontHandle != -1) {
            DrawStringToHandle(x, drawY, sub.c_str(), color, fontHandle);
        }
        else {
            DrawString(x, drawY, sub.c_str(), color);
        }
    }

    // 全文表示へスキップ（キー入力時用）
    void Skip() {
        mVisibleByteCount = mFullText.size();
        mIsFinished = true;
    }

    bool IsFinished() const { return mIsFinished; }

private:
    std::string mFullText;
    size_t mVisibleByteCount; // 表示対象のバイト数
    int mFrameInterval;       // 次の文字が出るまでの待機フレーム
    int mFrameTimer;          // タイマー
    bool mIsFinished;

    FloatingMotion mMotion;   // 浮遊アニメーション
};