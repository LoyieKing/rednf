bool CDataManager::hasSpecialCharacterExceptSpace(const char* name)
{
    for (size_t i = 0; i < strlen(name); ++i)
    {
        const unsigned char* p = (const unsigned char*)(name + i);
        if (_isOneByteCharacter(*p))
        {
            for (std::vector<InvalidCharData::stRangeElement<
                     unsigned char> >::const_iterator it =
                     m_invalidChar.m_charRange.begin(),
                     itEnd = m_invalidChar.m_charRange.end();
                 it != itEnd; ++it)
            {
                const InvalidCharData::stRangeElement<unsigned char>* r =
                    &(*it);
                if (r->m_min <= *p && r->m_max > *p)
                {
                    return true;
                }
            }
            for (std::vector<unsigned char>::const_iterator it =
                     m_invalidChar.m_charList.begin(),
                     itEnd = m_invalidChar.m_charList.end();
                 it != itEnd; ++it)
            {
                if (*it == *p)
                {
                    return true;
                }
            }
        }
        else
        {
            unsigned char buf[2] = { 0 };
            buf[0] = p[1];
            buf[1] = p[0];
            const unsigned short w = *(const unsigned short*)buf;
            for (std::vector<InvalidCharData::stRangeElement<
                     unsigned short> >::const_iterator it =
                     m_invalidChar.m_shortRange.begin(),
                     itEnd = m_invalidChar.m_shortRange.end();
                 it != itEnd; ++it)
            {
                const InvalidCharData::stRangeElement<unsigned short>* r =
                    &(*it);
                if (r->m_min <= w && w <= r->m_max)
                {
                    return true;
                }
            }
            for (std::vector<unsigned short>::const_iterator it =
                     m_invalidChar.m_shortList.begin(),
                     itEnd = m_invalidChar.m_shortList.end();
                 it != itEnd; ++it)
            {
                if (*it == w)
                {
                    return true;
                }
            }
            ++i;  // 双字节消耗两位（asm +0x30a 公共 +1 之外再 +1）
        }
    }
    return false;
