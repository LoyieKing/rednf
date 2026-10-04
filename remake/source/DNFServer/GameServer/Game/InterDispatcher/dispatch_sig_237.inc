// [b28d-lane2] verdict=DIFF ndiff=152（ORIG 0x084e48c2）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+4。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=162（ORIG 0x84e48c2）：P226 具名 bool/char 落槽+N18a 单段 regmove+N82/P112 帧槽平移。
int Inter_Antibot_ProcessProtectMsg::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_Antibot_ProcessProtectMsgPktView* pPkt = reinterpret_cast<Inter_Antibot_ProcessProtectMsgPktView*>(data);
    CUser* target =
        getUser(pPkt->m_uid);   // msg+0x2 uid
    if (target == 0 || target->get_state() == 0)
        return 0;

    // [R14a21] ORIG 0x84e4911 `movb $0x0,-0x1b; movw $0x0,-0x1a` 前置清零 ⇒ 声明=0
    // + 分支内独立赋值（P197③）。ORIG 0x84e491b 段为
    // `movzwl (%eax),%eax; movzwl %ax,%eax; cmp $0x13,%eax; je +0xa1; cmp $0x14,%eax;
    //  je +0x73; cmp $0x12,%eax; je +0x8a; jmp +0xb2`（**寄存器** cmp 链、无 eventType 槽）
    // ⇒ 原码是 `switch (pPkt->m_cmd)`（P251 无槽多路比较、P324 叶块物理序=书写序
    // 0x14/0x12/0x13/default）；我方旧形（具名 short + if/else 链）产 `mov %ax,-0x1e`
    // 槽 + 三条 `cmpw $imm,-0x1e(%ebp)` 内存直比。
    char protstatLevel = 0;
    // ORIG 0x84e4950 `movzwl 0x6(%eax),%eax` + 0x84e499c `movzwl -0x1a(%ebp),%edi`：
    // 该槽全程按**零扩展**读取（无 movswl）⇒ 原码是 unsigned short（§T1/T2 直证）。
    unsigned short extra = 0;
    switch (pPkt->m_cmd)
    {
    case 0x14:
        protstatLevel = (char)pPkt->m_protstatLevel;
        extra = pPkt->m_extra;
        break;
    case 0x12:
        protstatLevel = (char)pPkt->m_protstatLevel;
        extra = pPkt->m_extra;
        break;
    case 0x13:
        protstatLevel = 0;
        extra = pPkt->m_extra6;           // short@0x6
        break;
    default:
        protstatLevel = 0;
        extra = 0;
        break;
    }

    // P328⑩/P227：ORIG 0x84e497e..0x84e49aa 先求 extra→%edi、protstatLevel→%esi、uid→%ebx
    // 再 ctor ⇒ 全表达式临时链。
    cMyTrace(__PRETTY_FUNCTION__, 0x5f3b, 0)(
        "[SAFEMODE]uid : %d    protstatLevel : %d    eventType : %d\n",
        // [R14a21] 冗余整型 cast 删除（P41/P162）：ORIG 0x84e4983 `mov 0x2(%eax),%ebx`
        // 与 0x84e499c `movzwl -0x1a(%ebp),%edi` 均为**单段直落**；`(int)uint` /
        // `(int)(unsigned short)` 双转换会各多发一条 `mov`/`movzwl %ax,%reg`。
        pPkt->m_uid,
        (int)protstatLevel, (int)extra);

    if (protstatLevel == 1 || protstatLevel == 8)
    {   // 开启保护通知
        PacketGuard packet;
        packet.put_header(0, 0x1ee);
        packet.put_byte(1);
        packet.finalize(true);
        target->Send(packet);
    }
    if (protstatLevel == 0)
    {   // 关闭保护通知
        PacketGuard packet;
        packet.put_header(0, 0x1ee);
        packet.put_byte(0);
        packet.finalize(true);
        target->Send(packet);
    }
    return 0;
}