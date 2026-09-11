unsigned char GreyImage::getAlpha(int x, int y)
{
    return m_image[m_width * y + x];
}