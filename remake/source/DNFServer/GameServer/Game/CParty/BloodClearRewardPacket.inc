// ============================================================================
// CParty::BloodClearRewardPacket —— ORIG 0x85b8bf8：0xd1 血轮奖励包
// （四组 int 对 + 组数，物品表按 0x3d 步进）。
// ============================================================================
void CParty::BloodClearRewardPacket(int a, int b, int c, Inven_Item const* items1,
                                    int d, Inven_Item const* items2, int e,
                                    Inven_Item const* items3, int f,
                                    Inven_Item const* items4)
{
    PacketGuard packet;
    packet.put_header(0, 0xd1);
    packet.put_byte(b);
    // P0：ORIG 085b8c38 `add $0xb24,%eax` 直接取 &m_battleField 调 getBloodMaxRound，
    // 不缓存 bf 指针（无槽往返）。
    packet.put_byte(m_battleField.getBloodMaxRound());
    packet.put_byte(c);
    // ORIG：四组独立顺序循环（counter 分槽 -0x18/-0x14/-0x10/-0xc，非 2D 数组合并）。
    for (int i = 0; i < c; ++i)
    {
        packet.put_int(
            // ORIG BloodClearRewardPacket.md 085b8c75/7b `imul $0x3d +
            // mov 0x2(%eax)`：Inven_Item pack(1) sizeof==0x3d，+0x02 → m_addInfo。
            items1[i].m_addInfo);
        packet.put_int(items1[i].get_add_info());
    }
    packet.put_byte(d);
    for (int i = 0; i < d; ++i)
    {
        packet.put_int(items2[i].m_addInfo);
        packet.put_int(items2[i].get_add_info());
    }
    packet.put_byte(e);
    for (int i = 0; i < e; ++i)
    {
        packet.put_int(items3[i].m_addInfo);
        packet.put_int(items3[i].get_add_info());
    }
    packet.put_byte(f);
    for (int i = 0; i < f; ++i)
    {
        packet.put_int(items4[i].m_addInfo);
        packet.put_int(items4[i].get_add_info());
    }
    packet.finalize(true);
    m_member[a].m_pUser->Send(packet);
}