int DisPatcher_DeleteItem::read(PacketBuf& packet, MSG_BASE& msg)

{
    MSG_DELETE_ITEM& m = *reinterpret_cast<MSG_DELETE_ITEM*>(&msg);
    if (!packet.get_byte(m.m_itemSpace))                   // +0x0d
    {
        return LineFunc(0xae6,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(*reinterpret_cast<char*>(&m.m_count)))  // +0x0e（signed char→char& 适配）
    {
        return LineFunc(0xae7,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!(m.m_count < 0x65))
    {
        return LineFunc(0xaeb,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    for (int i = 0; i < (char)m.m_count; ++i)
    {
        if (!packet.get_short(m.m_opType[i]))              // +0x0f+2i
        {
            return LineFunc(0xaf0,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (!packet.get_short(m.m_position[i]))            // +0xd7+2i
        {
            return LineFunc(0xaf1,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (!packet.get_int(m.m_itemIdx[i]))               // +0x19f+4i
        {
            return LineFunc(0xaf2,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (!packet.get_int(m.m_countArr[i]))              // +0x32f+4i
        {
            return LineFunc(0xaf3,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (m.m_countArr[i] < 1)
        {
            return LineFunc(0xaf5,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
    }
    // 线格式尾部校验和（超对象 sizeof，无具名字段）
    if (!packet.get_int(*reinterpret_cast<int*>(reinterpret_cast<char*>(&m) + 0x4bf)))
    {
        return LineFunc(0xaf8,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
// [b28d-29] setcc-fold + N18a：bool 物化（xor $1 vs cmpb/setne）与单段直落 callee-saved 不可造；语义集合全等  (ORIG 0x081c1ad6 insn 154->160)
}
