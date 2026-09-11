void CTitleBook::_putItemData(PacketGuard& packet, short slot,
                              Inven_Item& item)
{
    packet.put_short((int)slot);
    packet.put_int(item.m_addInfo);
    packet.put_int(item.m_addInfo2);
    packet.put_byte((int)GetIntegratedPvPItemAttr(item) & 0xff);
    packet.put_short((int)item.m_fieldb);
    packet.put_byte((int)item.m_field0);
    if (G_GameWorld()->IsEnchantRevisionChannel())
    {
        packet.put_int(0);
    }
    else
    {
        packet.put_int(item.m_fieldd);
    }
    packet.put_byte((int)item.m_amp.getAbilityType() & 0xff);
    packet.put_short((int)item.m_amp.getAbilityValue() & 0xffff);
}