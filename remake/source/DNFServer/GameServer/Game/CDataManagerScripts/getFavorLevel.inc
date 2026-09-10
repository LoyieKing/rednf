int CNPCScriptList::getFavorLevel(unsigned short favorExp) const
{
    // ORIG 0x8581701-0x8581703：cmp %eax,%ebx; setge ⇒ `favorExp >= max` 正向
    // 早退 4（0x858170a 尾岛），循环体内 0x8581728 直接 cmp 参数槽 -0x1c。
    if (favorExp >= getMaxFavorValue())
        return 4;
    for (int i = 1; i <= 3; ++i)
    {
        if (m_npcCommon.m_favorExp[i] >= favorExp)
            return i;
    }
    return 4;
