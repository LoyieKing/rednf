// ---- SendHellPartyItemStatistic —— ORIG T 0x0860fcc0 -----------------------
// dungeonDiff>4 或 scriptIdx==0 → false；否则组包经统计代理（组 0）发送 0x2a 字节。
// [b28d-solo-CKillMonsterInfo_chain 判词 · AE] ORIG 0x0860fcc0（50 条）：
// 删具名 proxy 局部、改单条链式表达式（P52）后 `lea -0x32(%ebp),%ebx` 跨
// GetServerProxy call 保活复现，push %ebx + 帧 $0x54 + 槽 -0x28/-0x32 全对齐
// （旧版具名 proxy 帧 $0x58 无 push）。50=50 助记符/槽位逐条全等，P167 CLEAR；
// 残余仅 .o 重定位地址显示（P264）。
bool CHellPartyItemStatistic::SendHellPartyItemStatistic(
    bool random, int dungeonIdx, int diff, int dieReason, char scriptIdx,
    int* statistic)
{
    if (diff > 4)
        return false;
    if (scriptIdx == 0)
        return false;

    Packet_HellParty_Statistic_Item packet;
    packet.m_hellpartyType = random ? 1u : 0u;
    packet.m_dungeonIndex = static_cast<unsigned int>(dungeonIdx);
    packet.m_dungeonDiff = static_cast<unsigned char>(diff);
    packet.m_partyCount = static_cast<unsigned char>(dieReason);
    packet.m_hellpartyDiff = scriptIdx;
    memcpy(packet.m_data, statistic, 0x18);

    // [b28d-solo-CKillMonsterInfo_chain P52] ORIG 0850fcc0：`lea -0x32(%ebp),%ebx`
    // 在 GetServerProxy call 之前物化 &packet 并跨 call 保活，proxy 结果无槽
    // 直传（`mov %eax,(%esp)`）⇒ 必须写单条链式表达式；具名 proxy 局部多
    // `mov %eax,-0xc` + 回读且丢 push %ebx（帧 0x58 vs ORIG 0x54）。
    GlobalData::s_statistic_proxy_mgr->GetServerProxy(
        static_cast<ENUM_SERVER_GROUP>(0))
        ->SendPacket(reinterpret_cast<char*>(&packet), 0x2a);
    return true;
}