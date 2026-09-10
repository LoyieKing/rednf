int CTEA::Decrypt(const char* in, char* result, size_t n)
{
    if (m_bInit != true)
    {
        return -1;
    }
    if (n == 0 || n % m_blockSize != 0)
    {
        return -1;
    }

    unsigned int i;
    const char* pin;
    char* presult;

    if (m_iMode == 1)
    {
        i = 0;
        pin = in;
        presult = result;
        for (; n / m_blockSize > i; i++)
        {
            DecryptBlock((const unsigned char*)pin, (unsigned char*)presult);
            Xor(presult, m_apchain);
            memcpy(m_apchain, pin, m_blockSize);
            pin = pin + m_blockSize;
            presult = presult + m_blockSize;
        }
    }
    else if (m_iMode == 2)
    {
        i = 0;
        pin = in;
        presult = result;
        for (; n / m_blockSize > i; i++)
        {
            EncryptBlock((const unsigned char*)m_apchain, (unsigned char*)presult);
            Xor(presult, pin);
            memcpy(m_apchain, pin, m_blockSize);
            pin = pin + m_blockSize;
            presult = presult + m_blockSize;
        }
    }
    else
    {
        i = 0;
        pin = in;
        presult = result;
        for (; n / m_blockSize > i; i++)
        {
            DecryptBlock((const unsigned char*)pin, (unsigned char*)presult);
            pin = pin + m_blockSize;
            presult = presult + m_blockSize;
        }
    }
    return 1;
