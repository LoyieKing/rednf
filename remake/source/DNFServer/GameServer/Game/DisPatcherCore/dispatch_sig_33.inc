int DisPatcher_RequestGuildWarInfo::dispatch_sig(CUser* user, PacketBuf& /*packet*/)

{
    // ORIG 0x0820439a（报告全文转写）：短路 `||` 反判（state<2 或 cur==0 早退）——
    // `cur==0` 检查为分支双 immediates bool 物化形（`jne→mov $0;fall mov $1;jmp`+
    // `test %al; je body`），&& 正向形在 4.4 恒产单 je 直跳不等形。
    if (user->get_state() < 2 || user->getCurCharacR() == 0)
    {
        return 0;
    }
    Packet_Notice_Guild_War_Info pkt;
    pkt.m_uid = user->GetUID();
    pkt.m_characNo = user->get_charac_no(-1);
    // ORIG `mov %eax,-0x156; lea -0x164(%ebp),%ebx`：&pkt 在 proxy 链**之前**物化
    // 直落 callee-saved %ebx（技2+技13 声明位置=落点时机）；proxy 返回 %eax 短程直落。
    // 链式调用不留 proxy 槽（同 PingStat 定律：ORIG 返回 %eax 直落 this 位）。
    register char* pPkt = reinterpret_cast<char*>(&pkt);
    GlobalData::s_guild_proxy_mgr
        ->GetServerProxy(user->GetServerGroup())->SendPacket(pPkt, sizeof(pkt));
    return 0;
}
