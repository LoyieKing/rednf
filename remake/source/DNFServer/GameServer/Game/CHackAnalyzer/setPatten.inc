void Secu_AccountHacking::setPatten(AccountHackingType::T t, char const* name,
                                    unsigned int a, unsigned int b)
{
    // ORIG：a 缓存到 -0x10、b 缓存到 -0xc，但 cmpl $0x0 直测参数槽 0x14
    // （aSpill 声明保留缓存，比较直读形参 a）。
    // ORIG T 0x0827903c（setPatten.md）：未标记且已绑定用户时置位，随后按 a
    // 选择 DB 直查或经分析器加计数。
    if (m_flag[(int)t])
        return;
    if (m_userP == 0)
        return;

    m_flag[(int)t] = 1;

    // ORIG +0x66/+0x6f：两实参先早溢出到 -0x10/-0xc 栈槽（gcc callee 后续
    // 仍需引用时选择 caller-saved 缓存；此处以局部副本驱动同型）
    unsigned int aSpill = a;
    unsigned int bSpill = b;

    if (a == 0)
    {
        // ORIG：getMatchingHacktype 缓存 ebx 后再求 GetUID（P24 实参序），
        // 不引入具名中间变量。
        DB_SelectCharacMid::makeRequest((int)((CUser*)m_userP)->GetUID(), 0,
                                        name,
                                        (unsigned int)getMatchingHacktype(t),
                                        bSpill, 0);
    }
    else
    {
        ((CUser*)m_userP)->getHackAnalyzer()->addServerHackCnt(
            (CUser*)m_userP, getMatchingHacktype(t), 1, aSpill, bSpill);
    }
