int Dispatcher_ComboSkillExtensionQuickSlotReset::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_COMBO_SKILL_EXTENSION_QUICK_SLOT_RESET& req =
        *reinterpret_cast<MSG_COMBO_SKILL_EXTENSION_QUICK_SLOT_RESET*>(&msg);
    if (!packet.get_byte(reinterpret_cast<char&>(req.m_0d)))
    {
        return LineFunc(0x372, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}
