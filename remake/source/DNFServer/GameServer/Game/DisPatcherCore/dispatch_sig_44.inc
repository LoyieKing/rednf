// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 239（ORIG 418 / ours 367），残差=P211/N18a 中转/store 序/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
int Dispatcher_FrameLagStatistics::dispatch_sig(CUser* user, PacketBuf& packet)

{
#define FLLF(line) \
    return LineFunc((line), __PRETTY_FUNCTION__, 0, 0)

    Packet_Frame_Lag_Statistic_Add pkt;
    // 【P156 保留项：线格式序列化缓冲 + ORIG 别名算址】本包是纯 wire 缓冲区，
    // ORIG（0x082173cd/0x08217550/0x08217699/0x082176ec）以「包基址 + 常数/变数
    // 累加」的两步形态算址（行 stride 0x38、尾表 +0x170/+0xa 与行区重叠别名）。
    // 实测改写成 `pkt.m_body[i]` 具名成员索引后，GCC4.4 O0 会把算址折叠成单条
    // lea（419→375 insn），使本函数由 NEAR 退为 DIFF —— 按验收口径保留原形态。
    // [SigB] P0/P183：但 `char* base` 这个**具名槽**本身 ORIG 没有——ORIG 每次现场
    // `lea -0x19b(%ebp),%ecx` 重算（0x082173cd 等），我方原写把基址缓存进 -0x1c(%ebp)
    // （+0x017 `mov %eax,-0x1c(%ebp)` 为 ORIG 全函数 0 处）。同理 `proxy` 具名槽
    // （-0x18）ORIG 无：ORIG 0x08217768 `lea -0x19b(%ebp),%ebx` 把 &pkt 保活于 callee-saved，
    // 尾部 `mov %ebx,0x4(%esp)`。故此处把两处具名临时内联回表达式（保留两步算址形）。

    if (!packet.get_int(pkt.m_fieldA))
        FLLF(0xc8fd);
    if (!packet.get_byte(pkt.m_field0e))
        FLLF(0xc8fe);
    if (!packet.get_byte(pkt.m_field0f))
        FLLF(0xc8ff);
    if (!packet.get_short(pkt.m_field12))
        FLLF(0xc901);
    if (!packet.get_short(pkt.m_field14))
        FLLF(0xc902);
    if (!packet.get_short(pkt.m_field16))
        FLLF(0xc903);
    if (!packet.get_short(pkt.m_field18))
        FLLF(0xc905);
    if (!packet.get_byte(pkt.m_field1a))
        FLLF(0xc906);
    if (!packet.get_int(pkt.m_field1b))
        FLLF(0xc907);
    if (!packet.get_byte(pkt.m_field1f))
        FLLF(0xc909);
    if (!packet.get_short(pkt.m_field20))
        FLLF(0xc90a);
    if (!packet.get_short(pkt.m_field22))
        FLLF(0xc90b);

    // ---- 行循环（ORIG -0x14）：行内四 short + 6×(int+bin[4]) ----
    for (int r = 0; r <= 5; ++r)
    {
        // [b28d-solo-DisPatcherCore] P167 真 bug：ORIG 为**4 个独立 get_short(short&)
        // 调用点**（Ghidra C：单层 r 循环体内 4 条连续调用，LineFunc 0xc914..0xc917），
        // 原内层 c 循环把 4 个调用点折成 1 个（p167 MISSING get_shortERs×3 + LineFunc×3）
        // ⇒ 按 ORIG 展开为 4 条，dst = &pkt + r*0x38 + 0x24 + {0,2,4,6}。
        if (!packet.get_short(*reinterpret_cast<short*>(
                reinterpret_cast<char*>(&pkt) + r * 0x38 + 0x24)))
            FLLF(0xc914);
        if (!packet.get_short(*reinterpret_cast<short*>(
                reinterpret_cast<char*>(&pkt) + r * 0x38 + 0x24 + 2)))
            FLLF(0xc915);
        if (!packet.get_short(*reinterpret_cast<short*>(
                reinterpret_cast<char*>(&pkt) + r * 0x38 + 0x24 + 4)))
            FLLF(0xc916);
        if (!packet.get_short(*reinterpret_cast<short*>(
                reinterpret_cast<char*>(&pkt) + r * 0x38 + 0x24 + 6)))
            FLLF(0xc917);
        for (int j = 0; j <= 5; ++j)         // ORIG -0x10
        {
            if (!packet.get_int(*reinterpret_cast<int*>(
                    reinterpret_cast<char*>(&pkt) + ((r * 7 + j + 4) << 3) + 0xc)))
                FLLF(0xc91f);
            if (!packet.get_binary(
                    reinterpret_cast<char*>(&pkt) + ((r * 7 + j + 4) << 3) + 0x10, 4))
                FLLF(0xc920);
        }
    }

    // [SigB] P328⑤：ORIG 0x08217638 `lea -0x15(%ebp),%eax` 直接传址，全函数无
    // `movb $0x0,-0x15` ⇒ 原码 tailCount 未预清（读失败即 LineFunc 返回，无行为差）。
    signed char tailCount;
    if (!packet.get_byte(*reinterpret_cast<char*>(&tailCount)))
        FLLF(0xc925);

    // ---- 尾表（ORIG -0xc）：读侧算址 &pkt+0xa+(i+0xb8)*2，缺项清零 &pkt+0xa+i*2 ----
    for (int i = 0; i <= 5; ++i)
    {
        if (i < (int)tailCount)
        {
            if (!packet.get_byte(
                    reinterpret_cast<char*>(&pkt)[0x170 + i]))
                FLLF(0xc92a);
            if (!packet.get_short(*reinterpret_cast<short*>(
                    reinterpret_cast<char*>(&pkt) + 0xa + ((i + 0xb8) << 1))))
                FLLF(0xc92b);
        }
        else
        {
            reinterpret_cast<char*>(&pkt)[0x170 + i] = 0;
            *reinterpret_cast<short*>(
                reinterpret_cast<char*>(&pkt) + 0xa + (i << 1)) = 0;
        }
    }

    GlobalData::s_statistic_proxy_mgr->GetServerProxy(
        user->GetServerGroup())->SendPacket(
            reinterpret_cast<char*>(&pkt), 0x186);
    return 0;
#undef FLLF
}
