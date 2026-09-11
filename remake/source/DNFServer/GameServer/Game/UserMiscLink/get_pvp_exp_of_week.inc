// ===========================================================================
// 4) CUser::get_pvp_exp_of_week() —— ORIG W 0x08230100（报告缺失，objdump 转写）
//    PvpResultType +0x08=m_expPoint、+0x10=m_field10（周重置基准）：
//    field10==-1 → expPoint，否则 expPoint - field10。
// ===========================================================================
int CUser::get_pvp_exp_of_week()
{
    // ORIG 0x08230100：getPVPResultRefR 每字段访问各取一次（P48 无缓存，
    // 结果经 %ebx 跨字段读）。ORIG `cmp $-1; jne 后置减形岛` + fall expPoint。
    // 【b25c】4.4 对 if(call链值==K) 恒物化 sete（探针 probe_tern2 u1），
    // 三目形才直分支（u3 与 ORIG 逐条同构）。
    return CUserCharacInfo::getPVPResultRefR()->m_field10 == -1
               ? CUserCharacInfo::getPVPResultRefR()->m_expPoint
               : CUserCharacInfo::getPVPResultRefR()->m_expPoint
                     - CUserCharacInfo::getPVPResultRefR()->m_field10;
}