// ============================================================================
// SendPartyMemberTagChracInfo —— ORIG T 0x085be05a（报告 CParty/SendPartyMemberTagChracInfo.md）
// ============================================================================
void CParty::SendPartyMemberTagChracInfo()
{
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0x19e);

    int countIdx = packet.get_index();
    packet.put_short(0);

    int tagCount = 0;
    for (int local_14 = 0; local_14 <= 3; ++local_14)
    {
        CUser* user = get_user(local_14);
        // [b28d-53 归零] P223② 新实证：ORIG 两守卫共落同一 join（+0xa2）且 then
        // 尾有 `jmp <++i>; nop` 岛。复现形 = 扁平化「守卫1 continue + 守卫2 正向
        // if{…; continue;}」——嵌套 if 与负极性 `!call()` 均被 jump-thread/多产
        // xor+岛（实测 82/87 条）。此形 84=84 全等（ORIG 0x085be05a）。
        if (user == 0)
            continue;
        if (user->MakeTagCharacInfo(packet))
        {
            ++tagCount;
            continue;
        }
    }

    packet.put_short(countIdx, tagCount);
    packet.finalize(true);
    send_to_party(packet);
}