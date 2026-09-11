// ORIG _ZNK14CNPCScriptList16getMaxFavorValueEv W 0x8582364：
// movzwl 0x14(%eax)（m_npcCommon.m_maxFavorExp）+ movzwl %ax 零扩展，int 返回。
// ORIG _ZNK14CNPCScriptList13getFavorLevelEt 0x85816e4：favorExp 低于上限时按
// m_favorExp[1..3] 阈值表（ushort 无符号比较）返回 1..3，否则 4。
// 【P210】定义前置：调用者须在 getMaxFavorValue 定义之前，否则 4.4 按
// callee 帧精确继承 outgoing，0x34 保守大帧塌缩。
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
}