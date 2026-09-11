int Dispatcher_SaveGameOption_ChattingEmoticon::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_SAVE_GAME_OPTION_CHATTING_EMOTICON& data =
        reinterpret_cast<MSG_SAVE_GAME_OPTION_CHATTING_EMOTICON&>(msg);
    if (!packet.get_short(data.m_count))
    {
        return LineFunc(0x2934,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    for (int i = 0; i < static_cast<int>(data.m_count); ++i)
    {
        MSG_SAVE_GAME_OPTION_CHATTING_EMOTICON::_slot_String slot;
        if (!packet.get_short(*reinterpret_cast<short*>(&slot.m_slot)))
        {
            return LineFunc(0x293a,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        // ORIG 0x081cf409：strLen@-0x30（远槽、无预清零 dead store）、
        // strBuf@-0x2a 紧贴 slot(-0x16) 下方；get_int 实参 `lea -0x30` 直落。
        // 声明序翻转（技巧21 实验律）：strBuf 先声明、strLen 最后→分配最远。
        char strBuf[0x14];
        int strLen;
        if (!packet.get_int(strLen))
        {
            return LineFunc(0x2940,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (!packet.get_str(strBuf, 0x14, strLen))
        {
            return LineFunc(0x2943,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        slot.m_str = strBuf;   // ORIG std::string::operator=(asStack_18, local_2e)
        data.m_slots.push_back(slot);
    }
    return 0;
}