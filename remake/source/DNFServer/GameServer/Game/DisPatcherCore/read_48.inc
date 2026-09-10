int DisPatcher_UseEquip::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081c0dfa：实参内联表达式（mov 0x10(%ebp);add $0xd），无指针局部 → 帧 0x18。
    // [b28d-solo-DisPatcherCore] P167 真 bug：ORIG 调 `get_byte(char&)`（_ZN9PacketBuf8get_byteERc），
    // 原写法经 uchar 字段绑到 ERh 重载。
    if (!packet.get_byte(*reinterpret_cast<char*>(
            &reinterpret_cast<MSG_DECREASE_DURABILITY&>(msg).m_0d)))
    {
        return LineFunc(0x9d0,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
