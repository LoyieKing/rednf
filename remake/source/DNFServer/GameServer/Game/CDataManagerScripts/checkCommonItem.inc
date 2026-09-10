bool stBroadCastItemScript::checkCommonItem(int itemIdx) const
{
    bool b = false;
    if (b)
    {
        /* ORIG 空 then：守卫恒假，仅复现 4.1 的块布局 */
    }
    else
    {
        std::vector<int>::const_iterator it = m_vec0.begin();
        std::vector<int>::const_iterator e = m_vec0.end();
        for (; it != e && (b ^= 1, b); it++)
        {
            const int* p = &(*it);
            while (b)
            {
                if (*p == itemIdx)
                {
                    return true;
                }
                b = false;
            }
        }
    }
    return false;
