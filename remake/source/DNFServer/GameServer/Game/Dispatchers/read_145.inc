// ORIG 0x081df840：get_short 读数量写入 msg.m_count（+0x10），再逐个
// get_short push_back 进 m_questIdxs；失败走 LineFunc(0x51e1/0x51e8)。
int Dispatcher_Debug_Clear_Quest::read(PacketBuf& packet, MSG_BASE& msg)
{
    MsgClearQuestIndex& req = static_cast<MsgClearQuestIndex&>(msg);
    short count;              // ORIG 0x081dfae2 无初始化 store（P31）
    if (!packet.get_short(count))
    {
        return LineFunc(0x51e1,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    req.m_count = count;
    for (int i = 0; i < req.m_count; ++i)
    {
        short value;          // 同上：get_short 出参直写，ORIG 无预清零
        if (!packet.get_short(value))
        {
            return LineFunc(0x51e8,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        req.m_questIdxs.push_back((int)value);
    }
    return 0;
}