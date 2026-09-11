// ---------------------------------------------------------------------------
// hasSpecialCharacterExceptSpace(char const*) —— ORIG T 0x08362e0e
//   对 name 每个位置：
//     单字节（_isOneByteCharacter）：先查 InvalidCharData.m_charRange(+0x0c →
//       CDM+0x86d0) 区间表（lo <= c 且 c < hi 命中），再查 m_charList(+0x00 →
//       CDM+0x86c4) 精确表；
//     双字节：word = c[i]|c[i+1]<<8，先查 m_shortRange(+0x24 → CDM+0x86e8)
//       （lo <= word 且 word <= hi 双闭），再查 m_shortList(+0x18 → CDM+0x86dc)
//       精确表；任一命中返回 1，否则继续，扫完返回 0。
// ---------------------------------------------------------------------------
// [b28d-solo-CDataManagerTailB] AE：四处循环改「begin 先、end 后并缓存」
//   （for 初始化逗号声明 it, itEnd）；range 循环经 `&(*it)` 具名指针读
//   m_min/m_max（ORIG -0x14/-0xc 指针槽）；精确表查找由 std::find 改手写迭代器
//   循环；第二条件写 `r->m_max > *p`（P196 方向）。ORIG 0x08362e0e。
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
}