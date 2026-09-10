void CParty::enumPartyMember(bool (*func)(CUser*))
{
    // 报告：docs/class_func_reports/CParty/enumPartyMember.md（T 0x085b669a）：
    // i=0..3，有效槽调用回调；回调返回 false 提前结束遍历。
    for (int i = 0; i <= 3; ++i)
    {
        if (_checkValidUser(i))
        {
            if (!func(m_member[i].m_pUser))   // xor $1/test/jne：false → ret
                return;
        }
    }
