void GreyImage::setAlpha(int x, int y, unsigned char alpha)
{
    m_image[m_width * y + x] = alpha;
}