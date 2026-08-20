#ifndef DNF_GAME_GREYIMAGE_H_
#define DNF_GAME_GREYIMAGE_H_

// df_game_r Game/ GreyImage（从 GameStubs.cpp 拆出，独立 TU）。
// 布局：m_width(+0x00) / m_height(+0x04) / m_image(+0x08)。
// 实现已与 df_game_r 反编译逐函数比对（identical）。

class GreyImage
{
public:
    GreyImage();
    ~GreyImage();

    void init(int width, int height, unsigned char fill);
    void reset();

    int getWidth() const;
    int getHeight() const;

    void* getImage();
    void* getImage() const;

    unsigned char getAlpha(int x, int y);
    void setAlpha(int x, int y, unsigned char alpha);
    void setAlphaAsMax(int x, int y, unsigned char alpha);
    int sampleAlpha(int x, int y);

private:
    int m_width;
    int m_height;
    unsigned char* m_image;
};

#endif  // DNF_GAME_GREYIMAGE_H_
