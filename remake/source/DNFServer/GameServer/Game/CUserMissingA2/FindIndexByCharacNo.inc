// ----------------------------------------------------------------------------
// 5. ORIG T 0x081b42ae sz 0x51。按 m_characs[i].m_characNo（首 dword 无符号比较）
// 线性查找；未命中返回 0xffffffff。循环条件 seta（size()>idx 无符号大于）。
// ----------------------------------------------------------------------------
unsigned int UserMercenaryInfoMgr::FindIndexByCharacNo(unsigned int characNo) const
{
    size_t idx = 0;
    while (m_characs.size() > idx)
    {
        // [CUserLane 2026-09-11] m_characNo 已 unsigned（SigTypes_C.h）⇒ 原
        //   `static_cast<unsigned int>(…)` 冗余；ORIG 0x081b42ae 该点 `mov (%eax),%eax;
        //   cmp 0xc(%ebp),%eax` 单段无中转，删 cast 后仍 AE 28/28（影子 A/B 实证）。
        if (m_characs[idx].m_characNo == characNo)
            return static_cast<unsigned int>(idx);
        ++idx;
    }
    return 0xffffffffU;
}