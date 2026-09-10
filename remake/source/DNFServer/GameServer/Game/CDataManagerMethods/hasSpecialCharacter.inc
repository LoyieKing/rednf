bool CDataManager::hasSpecialCharacter(const char* name)
{
    for (size_t i = 0; i < strlen(name); ++i)
    {
        const unsigned char* p =
            reinterpret_cast<const unsigned char*>(name + i);
        if (_isOneByteCharacter(p[0]))
        {
            for (std::vector<InvalidCharData::stRangeElement<unsigned char> >::
                     const_iterator range = m_invalidChar.m_charRange.begin(),
                     rangeEnd = m_invalidChar.m_charRange.end();
                 range != rangeEnd; ++range)
            {
                const InvalidCharData::stRangeElement<unsigned char>& r =
                    *range;
                if (r.m_min <= p[0] && r.m_max >= p[0])
                {
                    return true;
                }
            }
            for (std::vector<unsigned char>::const_iterator ch =
                     m_invalidChar.m_charList.begin(),
                     chEnd = m_invalidChar.m_charList.end();
                 ch != chEnd; ++ch)
            {
                if (*ch == p[0])
                {
                    return true;
                }
            }
        }
        else
        {
            union stShortValueView
            {
                unsigned short m_value;
                unsigned char m_bytes[2];
            } shortValue;
            shortValue.m_value = 0;
            shortValue.m_bytes[0] = p[1];
            shortValue.m_bytes[1] = p[0];
            const unsigned short value =
                *reinterpret_cast<const unsigned short*>(&shortValue);
            for (std::vector<InvalidCharData::stRangeElement<
                     unsigned short> >::const_iterator range =
                     m_invalidChar.m_shortRange.begin(),
                     rangeEnd = m_invalidChar.m_shortRange.end();
                 range != rangeEnd; ++range)
            {
                const InvalidCharData::stRangeElement<unsigned short>& r =
                    *range;
                if (r.m_min <= value && value <= r.m_max)
                {
                    return true;
                }
            }
            for (std::vector<unsigned short>::const_iterator ch =
                     m_invalidChar.m_shortList.begin(),
                     chEnd = m_invalidChar.m_shortList.end();
                 ch != chEnd; ++ch)
            {
                if (*ch == value)
                {
                    return true;
                }
            }
            ++i;
        }
    }
    return false;
