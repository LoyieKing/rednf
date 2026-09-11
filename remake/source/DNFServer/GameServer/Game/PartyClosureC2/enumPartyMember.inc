// ===========================================================================
// CParty::enumPartyMember —— T 0x085b669a
// 报告缺失标记：无独立报告；objdump 逐指令转写（onClearMap 依赖闭环）。
// i=0..3：无效槽跳过；callback 返回 false 时提前结束遍历。
// ===========================================================================
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
}