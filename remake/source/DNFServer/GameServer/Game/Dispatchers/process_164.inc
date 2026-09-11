// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081e2664 orig/ours=78/110 ndiff-blocks=20
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    0x10(%ebp),%eax | mov    %eax,-0x10(%ebp) | mov    %eax,-0xc(%ebp)
int Dispatcher_CompatibilityIndex::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081e2664：local = check_error(user, msg, param)（不写 param）；
    // >0 返 0；<0 LineFunc(0x58f8,err,0)；==0 构造 Packet_Stat_Compatibility_Index，
    // 逐字段拷 msg 值后经 statistic 代理 SendPacket(pkt, 0x1e)。
    // [G1Dproc 2026-09-10] ORIG 第一句即 `mov 0x10(%ebp),%eax; mov %eax,-0x10(%ebp)`
    //   ⇒ msg 指针槽的声明在 check_error 之前（槽序 -0x10=msg、-0xc=err）。
    const MSG_COMPATIBILITY_INDEX* m =
        reinterpret_cast<const MSG_COMPATIBILITY_INDEX*>(&msg);
    int err = check_error(user, msg, param);
    if (err > 0)
    {
        return 0;
    }
    if (err < 0)
    {
        return LineFunc(0x58f8,
                        __PRETTY_FUNCTION__,
                        err, 0);
    }
    Packet_Stat_Compatibility_Index pkt;   // ORIG W ctor 0x0822c61e（局部帧 -0x2e 起）
    // [G1Dproc 2026-09-10] ORIG 0x081e2664 逐偏移直证（pkt 基址 = -0x2e(%ebp)）：
    //   +0x0a acc_id（0x081e26d2 `mov %eax,-0x24(%ebp)`）、
    //   +0x10 *(int*)(msg+0x10)（-0x1e）、+0x0f msg+0xe（-0x1f）、+0x0e msg+0xd（-0x20）、
    //   +0x14 u16@msg+0x14（-0x1a）、+0x16 u16@msg+0x16（-0x18）、
    //   +0x1a u16@msg+0x1a（-0x14）、+0x18 u16@msg+0x18（-0x16）、
    //   +0x1c msg+0x1c（-0x12）、+0x1d msg+0x1d（-0x11）。
    //   原 +0x6/+0xa/+0xb/+0xc/+0x10/+0x12/+0x14/+0x16/+0x18/+0x19 全部少 4（整块错位），
    //   且 int@msg+0x10 与两个 byte 的语句顺序照 ORIG（+0x10 → +0x0f → +0x0e）。
    // [DispA3 2026-09-11] P161/P183：ORIG 0x081e26e8..0x081e2745 每字段为单条**直接寻址**
    //   store（`mov %eax,-0x24(%ebp)` / `mov %al,-0x1f(%ebp)` / `mov %ax,-0x1a(%ebp)`，
    //   无 lea、无 `char* p` 指针槽）⇒ 原码是 packed 结构**具名成员**直存；裸 char* 取址
    //   生 lea+间接写（我方 110 insn vs ORIG 78）。
    pkt.m_accId = (unsigned int)user->get_acc_id();                   // +0x0a
    pkt.m_cpuClock = m->m_10;                        // +0x10
    pkt.m_cpuNum = (unsigned char)m->m_0e;           // +0x0f（ORIG 源 msg+0xe）
    pkt.m_cpuVendor = (unsigned char)m->m_0d;        // +0x0e（ORIG 源 msg+0xd）
    pkt.m_ram = m->m_14;                             // +0x14
    pkt.m_videoVendor = m->m_16;                     // +0x16
    pkt.m_videoRam = m->m_1a;                        // +0x1a
    pkt.m_videoDevice = m->m_18;                     // +0x18
    pkt.m_os = m->m_1c;                              // +0x1c
    pkt.m_osBit = m->m_1d;                           // +0x1d
    GlobalData::s_statistic_proxy_mgr->GetServerProxy(static_cast<ENUM_SERVER_GROUP>(0))
        ->SendPacket(reinterpret_cast<char*>(&pkt), 0x1e);

    return 0;
}