int Dispatcher_SaveQuestNotify::read(PacketBuf& packet, MSG_BASE& msg)
{
    // [b28d-solo-Dispatchers] P212：ORIG 函数头即 `mov 0x10(%ebp),%eax; mov %eax,
    //   -0x10(%ebp)`（msg 指针槽首行物化）⇒ req 声明提前。
    MSG_SAVE_QUEST_NOTIFY& req = reinterpret_cast<MSG_SAVE_QUEST_NOTIFY&>(msg);
    char count = 0;
    if (!packet.get_byte(count))
    {
        return LineFunc(0x2b3,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (count < 0 || count > 5)
    {
        return LineFunc(0x2b6,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    for (int i = 0; i < (int)count; ++i)
    {
        // [b28d-solo-Dispatchers] P0/P156：ORIG 0x0825efxx `mov -0xc(%ebp),%eax;
        //   shl $0x2,%eax; add -0x10(%ebp),%eax; add $0xd,%eax`（指针算术直传
        //   get_int 目标地址，无 value 局部槽、无独立写回）⇒ 删局部改直接取址；
        //   基址经 req 槽（-0x10）而非形参。
        if (!packet.get_int(*reinterpret_cast<int*>(
                reinterpret_cast<char*>(&req) + i * 4 + 0xd)))
        {
            return LineFunc(0x2bb,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
    }
    return 0;
