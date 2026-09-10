int CHARAC_LOAD_MERCENARY::GetCompetitionPeriodSecond(char periodIdx) const
{
    if (!CheckCompetitionPeriodIndex(periodIdx))
        return 0;
    // 【b28c-13 清绑】原 register asm("esi") 绑定按 P0 全域禁用，已删除。
    // ORIG 0x081b2f06：`call G_CDataManager; mov 0xa8dc(%eax),%esi`（右操作数
    // 先物化并升格 callee-saved，跨下一次 call 保活）⇒ P226 裸语句表达式形
    // 复现值升格（P200 方向：值作普通表达式直传，不经具名中间局部）。
    // P243 同族（整数乘）：ORIG 0x081b2efd 先求 m_field0（→%esi 跨 call 保活）再
    // 求 vec[...].first（→%eax），`imul %esi,%eax` ⇒ 源写「m_field0 * first」。
    return (int)G_CDataManager()->m_mercenarySystem.m_field0
           * G_CDataManager()->m_mercenarySystem
                 .m_vec14[static_cast<int>(periodIdx)].first;
