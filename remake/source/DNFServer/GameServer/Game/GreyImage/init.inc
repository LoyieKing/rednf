void GreyImage::init(int width, int height, unsigned char fill)
{
    reset();
    m_width = width;
    m_height = height;
    // ORIG 0x80df574：int×int 直接 imul mem 形（(size_t) cast 诱发寄存器中转）。
    m_image = new unsigned char[width * height];
    memset(m_image, fill, width * height);
}