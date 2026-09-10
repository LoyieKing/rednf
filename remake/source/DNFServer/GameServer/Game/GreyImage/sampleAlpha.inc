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
