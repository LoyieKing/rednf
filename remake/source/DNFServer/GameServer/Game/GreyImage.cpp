// df_game_r Game/ GreyImage 实现（独立 TU，从 GameStubs.cpp 拆出）。
// 逐函数与 df_game_r 反编译比对（identical）。
#include "GreyImage.h"
#include <cstring>

GreyImage::GreyImage()
{
    m_width = 0;
    m_height = 0;
    m_image = 0;
}

GreyImage::~GreyImage()
{
    reset();
}

void GreyImage::reset()
{
    if (m_image)
    {
        m_width = 0;
        m_height = 0;
        delete[] m_image;
        m_image = 0;
    }
}

void GreyImage::init(int width, int height, unsigned char fill)
{
    reset();
    m_width = width;
    m_height = height;
    m_image = new unsigned char[(size_t)width * height];
    memset(m_image, fill, (size_t)width * height);
}

int GreyImage::getWidth() const
{
    return m_width;
}

int GreyImage::getHeight() const
{
    return m_height;
}

void* GreyImage::getImage()
{
    return m_image;
}

void* GreyImage::getImage() const
{
    return m_image;
}

unsigned char GreyImage::getAlpha(int x, int y)
{
    return m_image[m_width * y + x];
}

void GreyImage::setAlpha(int x, int y, unsigned char alpha)
{
    m_image[m_width * y + x] = alpha;
}

void GreyImage::setAlphaAsMax(int x, int y, unsigned char alpha)
{
    unsigned char& p = m_image[m_width * y + x];
    if (alpha > p)
    {
        p = alpha;
    }
}

int GreyImage::sampleAlpha(int x, int y)
{
    int xi = x >> 16;
    int yi = y >> 16;
    if (xi < 0 || yi < 0 || xi >= m_width - 1 || yi >= m_height - 1)
    {
        return 0;
    }
    unsigned int fx = (unsigned int)x & 0xffff;
    unsigned int fy = (unsigned int)y & 0xffff;
    unsigned char a00 = getAlpha(xi, yi);
    unsigned char a10 = getAlpha(xi + 1, yi);
    unsigned char a01 = getAlpha(xi, yi + 1);
    unsigned char a11 = getAlpha(xi + 1, yi + 1);
    int top = (int)(((unsigned int)a00 * (0x10000 - fx) + (unsigned int)a10 * fx) >> 16);
    int bot = (int)(((unsigned int)a01 * (0x10000 - fx) + (unsigned int)a11 * fx) >> 16);
    return (int)(((unsigned int)top * (0x10000 - fy) + (unsigned int)bot * fy) >> 16);
}
