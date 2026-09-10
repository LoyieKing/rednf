char* CDataManager::_subPunctuationMark(const char* src, char* dst)
{
    int out = 0;
    for (unsigned int i = 0; i < strlen(src); ++i)
    {
        const unsigned char* p = (const unsigned char*)(src + i);
        if (_isOneByteCharacter(*p))
        {
            if (std::find(m_slangName.m_charList.begin(),
                          m_slangName.m_charList.end(),
                          *p) == m_slangName.m_charList.end())
            {
                dst[out++] = (char)*p;
            }
        }
        else
        {
            // ORIG 取连续两字节组 word（低位在前）
            unsigned char buf[2] = { 0 };
            buf[0] = p[1];
            buf[1] = p[0];
            const unsigned short w = *(const unsigned short*)buf;
            if (std::find(m_slangName.m_shortList.begin(),
                          m_slangName.m_shortList.end(),
                          w) == m_slangName.m_shortList.end())
            {
                dst[out++] = (char)*p;
                dst[out++] = (char)p[1];
                ++i;  // 双字节多消耗一个输入位（asm +0x1b4 处额外自增）
            }
        }
    }
    dst[out] = '\0';
    return dst;
