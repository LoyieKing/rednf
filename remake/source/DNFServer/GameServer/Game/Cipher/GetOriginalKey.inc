void Cipher::GetOriginalKey(unsigned char* dst)
{
    std::memcpy(dst, m_keyBuf, m_totalKeyLength);
