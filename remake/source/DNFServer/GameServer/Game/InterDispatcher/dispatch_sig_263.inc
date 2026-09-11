// [b28d-lane2] verdict=DIFF ndiff=196（ORIG 0x084e3c60）：槽序/帧差(N82/P212/P221)+混合残差；gap+54。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=184（ORIG 0x84e3c60）：N18a 单段 regmove+N82/P112 帧槽平移+P30/N4 add-vs-lea。
int Inter_GuildJoinList::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() <= 2)
        return 0;

    char* const local_1c = data;

    PacketGuard packet;
    packet.put_header(1, 0x161);
    packet.put_byte(1);
    packet.put_int(*reinterpret_cast<int*>(local_1c));

    for (int local_18 = 0; local_18 < *reinterpret_cast<int*>(local_1c); ++local_18)
    {
        // [R5·InterDisp 形态收口 2026-09-11] ORIG 0x084e3cf9 `add $0x4,%eax; mov (%eax),%eax`
        //   —— 裸指针算术 + 解引用（4.4 对此恒发两步 add），非视图成员访问
        //   （后者折单条 `mov 0x4(%eax)`）。ORIG 0x084e3e8b 同形于 +0x44。
        packet.put_int(*reinterpret_cast<int*>(local_1c + local_18 * 0x44 + 0x4));
        const int local_14 = (int)strlen(local_1c + local_18 * 0x44 + 0x08);
        packet.put_int(local_14);
        packet.put_str(local_1c + local_18 * 0x44 + 0x08, local_14);
        packet.put_byte(((Inter_GuildJoinListEntryWinView*)(local_1c + local_18 * 0x44))->m_26);
        packet.put_byte(((Inter_GuildJoinListEntryWinView*)(local_1c + local_18 * 0x44))->m_27);
        packet.put_byte(((Inter_GuildJoinListEntryWinView*)(local_1c + local_18 * 0x44))->m_29);
        packet.put_byte(((Inter_GuildJoinListEntryWinView*)(local_1c + local_18 * 0x44))->m_28);
        const int local_10 = (int)strlen(local_1c + local_18 * 0x44 + 0x2a);
        packet.put_int(local_10);
        packet.put_str(local_1c + local_18 * 0x44 + 0x2a, local_10);
        packet.put_int(*reinterpret_cast<int*>(local_1c + local_18 * 0x44 + 0x44));
    }

    packet.finalize(true);
    user->Send(packet);
    return 0;
}