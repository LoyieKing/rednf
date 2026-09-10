bool uncompress_zip(char* out, unsigned int& outLen, const char* in, unsigned int inLen)
{
    if (inLen == 0)
    {
        outLen = 0;
        return true;
    }
    unsigned long raw =
        static_cast<unsigned long>(*reinterpret_cast<const unsigned int*>(in)) &
        0x3fffffff;
    int ret = uncompress(reinterpret_cast<unsigned char*>(out), &raw,
                         reinterpret_cast<const unsigned char*>(in + 4),
                         static_cast<unsigned long>(inLen));
    outLen = static_cast<unsigned int>(raw);
    if (ret != 0)
        return false;
    return true;
