// ---- HellPartyDifficulty（T 0x0826d116）----
// 命中槽位：result.value00=hell[idx].value4、result.value04=hell[idx].field0（交叉写）。
void HellPartyDifficulty::process(RewardData_Param param, RewardData_Result& result)
{
    // b28d-20: ORIG 0x0826d116 — 两早退岛（js / jge）+ 有符号 sz 比较；size 先算
    // 存 -0x10、idx 存 -0xc（ORIG 槽序），两次 operator[] 各自重算 vector 地址（P183）。
    int sz = static_cast<int>(m_data.m_basicReward.m_hellDatas.size());
    int idx = param.m_party->get_member_count() - 2;
    if (idx < 0)
        return;
    if (idx >= sz)
        return;
    result.m_value00 = m_data.m_basicReward.m_hellDatas[idx].m_value4;
    result.m_value04 = m_data.m_basicReward.m_hellDatas[idx].m_field0;
}