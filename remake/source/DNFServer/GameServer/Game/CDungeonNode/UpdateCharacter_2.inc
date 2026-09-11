// ============================================================================
// CSuccessJobNode::UpdateCharacter(SIG_DUNGEON_DATA_STATISTIC*) —— ORIG T 0x08484d4c
// m_jobPartyMap.find(stat->m_charac[0x20] 处 int)：命中 → m_count 自增；
// m_charac += stat->m_charac（Character 聚合；ORIG 为 Character::operator+= 单调用，
// m_charac 成员现为 Character，与 ORIG 一致）。
// ============================================================================
int CSuccessJobNode::UpdateCharacter(SIG_DUNGEON_DATA_STATISTIC* stat)
{
    // [b28d-solo-CDungeonNode · N107 封顶] ORIG 0x08484d4c：find sret -0x14 具名、
    // end 临时 -0x10（具名 e 时 4.4 反置，已删）；operator-> 只调一次并把
    // `&it->second.m_count` 存具名引用槽 -0xc（P0 槽往返⇒原码有具名局部）；
    // 尾段仅一次 Character::operator+=（无 m_f3c 自增，ORIG 无该指令，已删）。
    // 残差 = N107（`&stat->m_charac[0x20]`/`&stat->m_charac[0]` 取址：4.4 恒
    // `add $K,%reg; mov %reg,%reg2` vs ORIG `lea K(%eax),%ecx`；本轮 20 形探针
    // /tmp/b28d_s218_probe{,2,3}.cpp 复证不可造；第 3 处 `this+0x18` 的 eax/edx
    // 角色互换系前两处连锁）。
    // 46 vs 45（+1 = N107 的 add+mov 比 lea 多 1 条）；p167 5/5 CLEAR。
    std::map<int, JobParty>::iterator it =
        m_jobPartyMap.find(*reinterpret_cast<const int*>(&stat->m_charac[0x20]));
    if (it != m_jobPartyMap.end())
    {
        JobParty& jp = it->second;
        jp.m_count = jp.m_count + 1;
    }
    m_charac += *reinterpret_cast<const Character*>(&stat->m_charac[0]);
    return 1;
}