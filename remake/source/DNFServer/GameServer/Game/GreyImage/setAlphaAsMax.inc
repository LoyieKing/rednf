void GreyImage::setAlphaAsMax(int x, int y, unsigned char alpha)
{
    // ORIG 0x80df636：像素地址与 alpha 各取 const uchar&，call std::max<unsigned_char>
    // 模板（-O0 外联），结果引用解引用写回像素。手写比较会内联成 cmp/jae（结构差）。
    unsigned char& p = m_image[m_width * y + x];
    p = std::max(p, alpha);
}