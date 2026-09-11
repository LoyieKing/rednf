// [b28c-32] verdict=NEAR：N7 + movzbl 两段（ORIG %eax 中转 vs 4.4 直装 %ebx）（48=48）。
// [b28d-lane2] verdict=AE ndiff=0（ORIG 0x084c89b2）：混合残差；gap0。
int Inter_MonitorCallUserInfo::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    if (G_CGameManager() == 0)
    {
        // ORIG 族 0x084c89ff：null 支 trace 槽 -0x2c（深）、主支 -0x1c（浅）。
        // 技巧3：null 支临时链式形占深槽（同 CutOffCallUserInfo 闭合形）。
        cMyTrace(__PRETTY_FUNCTION__,
                 0x2220, 5)(
            "Inter_MonitorCallUserInfo::dispatch_sig()  G_CGameManager() is null");
    }
    else
    {
        // ORIG 0x084c89ff：pkt 物化在主路径；group 两次独立读（P48 无 CSE），
        // ORIG 0x084c8a2a/0x084c8a58：`movzbl 0xa(%eax),%eax; movzbl %al,%ebx`
        // 两步零扩展（unsigned int 槽 = char→int 宽度转换先落 %eax 再转 %ebx）。
        Inter_MonitorCallUserInfoPktView* pPkt = reinterpret_cast<Inter_MonitorCallUserInfoPktView*>(data);
        // [b28d-lane2] P226/P48：ORIG 0x084c8a2a/0x084c8a58 两处均为
        // `movzbl 0xa(%eax),%eax; movzbl %al,%ebx`（uchar→uint 显式转换两段装载）。
        // 具名 register 局部使 4.4 融合为 `movzbl 0xa(%eax),%ebx`（N18a 变体）；
        // 改「内联实参表达式」后 4.4 复现两段（探针 /tmp/l2probe/p14.cpp f7 实证），
        // 第二处经 cMyTrace 全表达式临时链以 %ebx 跨 ctor 保活（P227）。
        G_CGameManager()->Send_userinfos_to_upper_server(
            (unsigned char)(unsigned int)pPkt->m_a);
        // ORIG 0x084c8a58：重读无 CSE（P48 两次独立装载）。
        cMyTrace(__PRETTY_FUNCTION__,
                 0x2230, 0)(
            "%d Server Call UserInfo Arrived\n",
            (unsigned char)(unsigned int)pPkt->m_a);
    }
    return 0;
}