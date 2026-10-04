// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x0821e1f4 orig/ours=134/179 ndiff-blocks=31
//   帧差 sub $234 vs $25c + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x234,%esp | lea    -0x21c(%ebp),%eax | lea    -0x21c(%ebp),%eax
int Dispatcher_AssertManager::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0821e1f4：0x214 连续缓冲（Packet_Assert_Manager_Info 头 + 负载数据区）。
    // 负载布局：category@+0xa / desc[256]@+0xe / field10e@+0x10e / field110@+0x110
    // / detail[0x100]@+0x114；总长 0x214。
    // [G1Asig] ORIG 段直证：`sub $0x234,%esp; lea -0x21c(%ebp),%eax; mov %eax,(%esp);
    // call <ctor>` —— 单实参 ctor 直调（this=&buffer），**无 placement new / operator new**；
    // 且各字段访问一律内联折叠（读=`mov -0x212(%ebp),%eax`、取址=`lea -0x21c(%ebp);
    // add $0xe`），无 view/引用/指针具名槽。原 `new (buffer) …` 形产 operator new + null
    // 检查 + EH 保存（我方 179 insn / 帧 0x25c vs ORIG 133 / 0x234）。
    Packet_Assert_Manager_Info header;               // -0x21c，sizeof 0x214，POD（尾无 dtor call）
    // 2026-09-11 K1DispG1：ORIG 读一律 `mov -0x212(%ebp),%eax`（disp 单条）、取址
    // `lea -0x21c(%ebp); add $0xe`（两步）⇒ 原码是**完整类型具名成员**访问。权威头
    // （Packet_Assert_Manager_Info.h）只有 m_data，故在本块内定义同布局 packed 视图
    // 并逐点就地 cast（零额外槽；不改 .h）。
    if (!packet.get_int(header.m_category))
    {
        return LineFunc(0xd275, __PRETTY_FUNCTION__, 0, 0);
    }
    if (header.m_category > 0xff)
    {
        return LineFunc(0xd279, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_binary(header.m_desc,
                           header.m_category))
    {
        return LineFunc(0xd27a, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_short(header.m_f10e))
    {
        return LineFunc(0xd27d, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_int(header.m_f110))
    {
        return LineFunc(0xd27f, __PRETTY_FUNCTION__, 0, 0);
    }
    // ORIG 段直证：两处判断各自重读 `mov -0x10c(%ebp),%eax`（无具名中间量）。
    if (header.m_f110 > 0xff ||
        header.m_f110 < 0)
    {
        return LineFunc(0xd280, __PRETTY_FUNCTION__, 0, 0);
    }
    if (header.m_f110 > 0)
    {
        if (!packet.get_binary(
                header.m_detail,
                header.m_f110))
        {
            return LineFunc(0xd283, __PRETTY_FUNCTION__, 0, 0);
        }
    }
    else
    {
        memset(header.m_detail, 0, 0x100);
    }
    // ORIG 0821e42e..0x0821e460：`lea -0x21c,%ebx`（&header 保活）→ ServerGroup →
    // GetServerProxy 链式 receiver（无 proxy 具名槽）。
    GlobalData::s_statistic_proxy_mgr->GetServerProxy(user->GetServerGroup())
        ->SendPacket(reinterpret_cast<char*>(&header), 0x214);
    return 0;
}