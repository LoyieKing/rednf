// ===================== hasSpecialCharacter =====================
// [b28d-solo-CDataManagerMethods] IDENTICAL（orig=235 our=235 ndiff=0）。
// 关键修法：四循环改 const_iterator + for-init 缓存 end（P96/P13）、双字节值
// 改 2 字节 union 视图逐字节写（movw $0 + 两个 movb + movzwl）、while 改
// for+else 内 ++i（复现两条 addl $1）、比较方向 `r.m_max >= p[0]`（翻 jb）。
// ORIG 0x08363140：逐字符扫描（宽字符步进 2）。单字节走 m_invalidChar 的
// uchar 区间表(+0x0c)/单字表(+0x00)；双字节拼 big-endian u16 后走 ushort
// 区间表(+0x24)/单字表(+0x18)。命中即 true；扫完返回 false。
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
}