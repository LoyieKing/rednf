bool SameBuffer(unsigned char* a, unsigned long size, unsigned char* b)
{
    for (unsigned long i = 0; i < size; ++i)
    {
        if (a[i] != b[i])
        {
            return false;
        }
    }
    return true;
}