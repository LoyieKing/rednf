// ORIG execute 0x084ab29c：GM 校验通过后向 guild 服务器发解散请求（0x1df 流）、
// 清本地公会键并回发 0x3d 包，同时向 monitor 广播 Packet_Monitor_Notice_Guild_Dismiss。
// [round5-GmCmds_C 复核] 本件已在 **AE** 档（不在 check_tu 的 DIFF 列表）：全函数
// 仅数据地址/别名差（p167 34=34，MISSING/EXTRA 全为 C2/D2↔C1/D1 同址别名，P93）；
// ORIG 0x084ab333 起 `mov -0xc(%ebp),%eax`（guildKey 具名局部跨 set 清零复用）等
// 槽序与 4.1 逐条同形 ⇒ 无旋钮、零改动。
// [C4A0Nine 2026-09-11 复验] n1_v=AE（165/166，clean 归一口径 identical）；strict 残差
//   纯为 .o 未链接 reloc/大地址表示（`mov 0x0,%eax`+`$0x8c7fa63` vs 链接后绝对地址），
//   p167 差集 CLEAR ⇒ 前波 lsEi/lsEj 问题已随实参类型修正收口，无剩余可造项。
void CDeleteGuild::execute()
// [P8S2 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 NEAR 系 objdump 无 -r 的假残差
{
    CUser* user = GetUser();
    if (!user->isGMUser())
    {
        return;
    }
    if (user->get_state() != 3)
    {
        return;
    }
    // P48 no-CSE：ORIG 三处各自 fresh get_charac_guildkey()（if 判零 / guard<< /
    // notice.m_guildKey）。
    if (static_cast<unsigned int>(user->get_charac_guildkey()) == 0)
    {
        return;
    }

    // 向 DB 队列发解散流：0x1df + (uid, characNo, guildkey)。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("GameMaster.cpp", 0x68d), true);
    // ORIG 0x084ab333：guildKey 在 guard 构造后立即求值并缓存具名局部（-0xc 槽往返，
    // P0），供 notice.m_guildKey 复用；且必须早于下方 set_charac_guildkey(0)——否则
    // 解散通知包里的公会键恒为 0（真语义 bug，原实现即栽在此）。
    unsigned int guildKey =
        static_cast<unsigned int>(user->get_charac_guildkey());
    // ORIG：`(*guard)->operator<<(x)`（operator* de()）标识调用形态。
    (*guard)->operator<<(0x1df);
    (*guard)->operator<<(user->GetUID());
    (*guard)->operator<<(user->getCurCharacNo());
    // [L11·round3] p167 直证：ORIG 该处为 `_ZN12CStreamGuardlsEi`（int 实参），
    // 我方 `static_cast<unsigned int>` 发 `lsEj` ⇒ MISSING lsEi ×1 / EXTRA lsEj ×1。
    (*guard)->operator<<(static_cast<int>(user->get_charac_guildkey()));
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);

    user->set_guild_create_right(0);
    user->set_charac_guildkey(0);

    PacketGuard packet;
    packet.put_header(0, 0x3d);
    packet.finalize(true);
    user->Send(packet);

    Packet_Monitor_Notice_Guild_Dismiss notice;
    notice.m_guildKey = guildKey;
    // ORIG 0x084ab467..0x084ab492：GetServerProxy 结果 eax 直通 receiver（无 proxy
    // 具名槽），&notice/packetSize 先物化 %ebx/%esi 跨调用保活（P52 链式 receiver +
    // R2L 实参独占）；size 取包内 packetSize（P171 照抄）。
    GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup())
        ->SendPacket(reinterpret_cast<char*>(&notice), notice.packetSize);
}