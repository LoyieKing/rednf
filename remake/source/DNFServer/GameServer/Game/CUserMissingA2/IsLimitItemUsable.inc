// ----------------------------------------------------------------------------
// ORIG T 0x08688c70 _ZN5CUser17IsLimitItemUsableEi：
// 命中条目按 isUsable() 判定（真返 1/假返 0）；无条目视为可用返 1。
// ----------------------------------------------------------------------------
bool CUser::IsLimitItemUsable(int itemIdx)
{
    for (std::vector<LimitItemUsage>::iterator it = m_limitItemUsage.begin(),
         e = m_limitItemUsage.end(); it != e; it++)
    {
        if (it->m_field15 == itemIdx)
        {
            if (it->isUsable())
                return 1;
            return 0;
        }
    }
    return 1;
}