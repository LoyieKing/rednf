void IMethod::WordToBytes(unsigned int src, unsigned char* dst)
{
    *dst = (unsigned char)(src >> 24);
    ++dst;
    *dst = (unsigned char)(src >> 16);
    ++dst;
    *dst = (unsigned char)(src >> 8);
    ++dst;
    *dst = (unsigned char)src;
