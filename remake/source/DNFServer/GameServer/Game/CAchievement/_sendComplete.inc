void CAchievement::_sendComplete(unsigned int id, int titleIdx,
                                 ENUM_TITLE_BOOK_CATEGORY category, int slot)
{
    stAchievement trig = _getTrigger(id);

    PacketGuard packet;
    packet.put_header(0, 0x167);
    packet.put_int((int)trig.m_id);
    packet.put_int((int)category);
    packet.put_int(slot);
    packet.put_int(titleIdx);
    // ORIG 0x828c436：GetCharacExpandData 结果不缓存局部变量（edx 直接传 getTitleSlotR）
    Inven_Item titleSlot =
        ((CTitleBook*)m_user->GetCharacExpandData(
             (ENUM_CHARAC_EXPAND_TYPE)14))
            ->getTitleSlotR(category, slot);
    packet.put_short((int)titleSlot.m_fieldb);
    packet.finalize(true);
    m_user->Send(packet);
}