// ----------------------------------------------------------------------------
// ORIG T 0x081b4198：线性查找 characNo 相同项；命中且 level 不同则更新
// level 并 alter()，返回 0；命中 level 相同也返回 0；未命中返回 1。
// 循环条件 seta（size()>i 无符号），参数首 dword 每次迭代载入 ebx（P36）。
// ----------------------------------------------------------------------------
bool UserMercenaryInfoMgr::CheckValidAppend(const CHARAC_LOAD_MERCENARY& info)
{
    size_t i = 0;
    while (m_characs.size() > i)
    {
        if (info.m_characNo == m_characs[i].m_characNo)
        {
            if (m_characs[i].m_level != info.m_level)
            {
                m_characs[i].m_level = info.m_level;
                m_characs[i].alter();
                return 0;
            }
            return 0;
        }
        ++i;
    }
    return 1;
}