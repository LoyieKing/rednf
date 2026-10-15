void PacketBuf::put_packet(const Inven_Item& item)
{
    // [b28d-solo-PacketBuf] 本行触发 RandomOption::put_packet_random_option<PacketBuf>
    // 实例（ORIG 0x858e3ec）verdict=AE（p167 15/15 CLEAR，133=133）。修法落在
    // Inven_Item.h 模板体：P235 具名数组视角 `m_slots[i]`（消 reinterpret_cast
    // 视图的 lea 链，复现 ORIG `add 0x8(%ebp)` + `movzbl (%eax,%ecx,1)`）
    // + P308 位域视角 `m_changedSlot.m_bits.m_low2`（复现 `movzbl; and $3; movzbl`）。
    item.m_random.m_random.put_packet_random_option(*this);

    if (G_GameWorld()->IsIntegratedPvPBaseChannel() &&
        G_CDataManager()->GetUpgradeRevisionPvPChannel() == 0)
    {
        put_byte(0);
    }
    else
    {
        put_byte(item.m_upgradeSep.GetUpgradeSeparate());
    }

    put_byte(item.m_upgradeSep.IsTradeRestriction());
}