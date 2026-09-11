// ----------------------------------------------------------------------------
// ORIG T 0x0814432a _ZN8WongWork10CBossTower16SendRewardPacketEv：
// 逐有效成员发奖励预告包(0,0x1e3)：put_int(m_rewardExp[i])；对全部 4 个奖励块
// （无论有效性）：put_byte(m_memberReward[j].m_count) 后逐件
// put_int(item.m_addInfo)+put_int(item.get_add_info())；finalize 后 Send。
// 恒返 1。
// ----------------------------------------------------------------------------
// [b28d-54 N] ORIG 0x0814432a：帧 0x30→0x40 + 槽平移（N82）+ 寄存器角色（N7）。
// [b28d-solo-CBossTower] AE：去 `Inven_Item& reward` 具名引用（P157/P183 每字段
//   重算 imul 寻址）+ invalid 裸短路条件 ⇒ DIFF→AE。
bool CBossTower::SendRewardPacket()
{
    for (int i = 0; i <= 3; ++i)
    {
        CUser* member = m_party->get_user(i);
        // ORIG 0x08144378..0x08144384：短路物化岛无 bool 槽 ⇒ 裸条件（P0/P226）。
        if (member == 0 || m_party->checkValidUser(i) != 1)
            continue;

        PacketGuard packet;
        packet.put_header(0, 0x1e3);
        packet.put_int(m_rewardExp[i]);              // this+(i+4)*4+8 = +0x18
        for (int j = 0; j <= 3; ++j)
        {
            packet.put_byte(m_memberReward[j].m_count);   // +j*0x268+0x28
            for (int k = 0; k < m_memberReward[j].m_count; ++k)
            {
                // P157/P183：ORIG 每字段重算完整 imul 寻址，无具名引用缓存。
                packet.put_int(m_memberReward[j].m_items[k].m_addInfo);
                packet.put_int(m_memberReward[j].m_items[k].get_add_info());
            }
        }
        packet.finalize(true);
        member->Send(packet);
    }
    return 1;
}