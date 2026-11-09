#pragma once

class Cursor
{
public:
    Cursor() = default;
    ~Cursor() = default;

    void Initialize();  // 画像のロード & デフォルトカーソルを非表示
    void Draw();        // カーソルの描画
    void Finalize();    

private:
    int mHandle = -1;
};