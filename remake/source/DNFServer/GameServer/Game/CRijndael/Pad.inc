int IMethod::Pad(char* data, int len, unsigned int* outError)
{
    // ORIG _ZN7IMethod3PadEPciPj @0x080b44a6（0xf7）
    if (!m_bInit)
    {
        if (outError != NULL)
        {
            *outError = 0x70000005;
        }
        return 0;
    }

    int rem = len % m_blockSize;
    if (rem == 0)
    {
        return len;
    }

    const int pad = m_blockSize - rem;
    char* end = data + len;

    switch (m_padding)
    {
    case 0:
        for (int i = 0; i < pad; ++i)
        {
            *(end++) = (char)0x00;
        }
        break;
    case 1:
        for (int i = 0; i < pad; ++i)
        {
            *(end++) = (char)0x20;
        }
        break;
    case 2:
        for (int i = 0; i < pad; ++i)
        {
            *(end++) = (char)pad;
        }
        break;
    default:
        break;
    }

    return len + pad;
