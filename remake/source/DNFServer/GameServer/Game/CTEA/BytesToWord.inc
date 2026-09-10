void IMethod::BytesToWord(unsigned char const* src, unsigned int& dst)
{
    dst = 0;
    dst |= (unsigned int)src[0] << 24;
    dst |= (unsigned int)src[1] << 16;
    dst |= (unsigned int)src[2] << 8;
    dst |= (unsigned int)src[3];
