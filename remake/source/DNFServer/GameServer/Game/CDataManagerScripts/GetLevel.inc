// STExpertJobScript::GetLevel（TSV 修复发明符号：_ZN17STExpertJobScript8GetLevelEj，ORIG 0x08693ba0）。
// ORIG：遍历 vector<ExpertJobExp>，返回首个 v[i].m_exp > exp 的 i+1；无则返回 size()。
int STExpertJobScript::GetLevel(unsigned int exp)
{
    // ORIG 0x8693ba6：i 于函数级声明（-0xc）；0x8693bee 退出 fall-through
    // 直接 mov -0xc(%ebp),%eax 返回 i（循环尾 i==size，不再 call size）。
    size_t i;
    for (i = 0; i < m_expVec.size(); ++i)
    {
        if ((unsigned int)m_expVec[i].m_exp > exp)
            return (int)i + 1;
    }
    return (int)i;
}