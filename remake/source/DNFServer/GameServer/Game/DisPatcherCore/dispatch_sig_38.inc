// [b28d-29] P0 清绑（原 register asm 绑定已删，N96）：清绑后 4.4 恒槽化/单段直落 callee-saved 不可造 ⇒ N18a/N96 封顶（绑定形为唯一通路，P0 禁）。
// [b28d-solo-DisPatcherCore] 复核 AE：ORIG 65 / ours 65 insn 全等（AE 口径，重定位归一）。
int Dispatcer_polic_one_to_one_chat_disable::dispatch_sig(CUser* user, PacketBuf& packet)

{
    // ORIG 0x082272ce（报告 Dispatcer_polic_one_to_one_chat_disable/dispatch_sig.md）：
    // state>2 且当前角色存在 → 读包组 Packet_Disable_User_OneToOneChat_Police；
    // GM 发起时回填 characNo 并经 Monitor 代理 SendTcpPacket(0x30)。
    // ORIG +0x12..+0x34：4.1 将 !(state>=3 && ptr) 物化为 %al（双常量岛 jle→mov$1/
    // jne→mov$0 + test;je），跳过路径 `mov $0,%eax; jmp end`。等价 skip-flag 写法。
    // [b28d-solo-DisPatcherCore] P0：ORIG `test %al,%al` 无槽 ⇒ 原码是内联 || 表达式；
    // 具名 bool skip 使 4.4 落 -0x9 槽（movb+cmpb）并把 pkt 对象槽整体顶浅 1B
    //（-0x39 vs ORIG -0x38）与帧 +0x10。
    if (user->get_state() <= 2 || user->getCurCharacR() == 0)
    {
        return 0;
    }
    {
        Packet_Disable_User_OneToOneChat_Police pkt;
        // ORIG +0x44/+0x5b：无 nameLen 临时槽——`lea -0x38,%eax; add $0xe` 直传
        // get_int(pkt.m_nameLength)，get_str 第 3 参 `mov -0x2a(%ebp),%eax`（= pkt+0xe）。
        packet.get_int(*reinterpret_cast<unsigned int*>(
            reinterpret_cast<char*>(&pkt) +
            offsetof(Packet_Disable_User_OneToOneChat_Police, m_nameLength)));
        packet.get_str(pkt.m_name, static_cast<int>(sizeof(pkt.m_name)),
                       static_cast<int>(pkt.m_nameLength));
        if (user->isGMUser())
        {
            pkt.m_gmId = user->getCurCharacNo();
            // ORIG：GetServerProxy 返回值寄存器直传 SendTcpPacket（无 proxy 具名槽）。
            // P52/P200：&pkt 作实参直传，跨 GetServerGroup/GetServerProxy 存活 → 4.4 落 %ebx。
            GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup())
                ->SendTcpPacket(reinterpret_cast<char*>(&pkt), sizeof(pkt));
        }
    }
    return 0;
}
