int DisPatcher_CreateCharac::read(PacketBuf& packet, MSG_BASE& msg)

{
    // 入口具名视图引用：ORIG +0x06 `mov 0x10(%ebp),%eax; mov %eax,-0xc(%ebp)` —
    // msg 指针唯一槽 -0xc（所有读走该槽 reload，技13 声明位置=落栈时机）。
    MSG_CREATE_CHARAC_VIEW& v = *reinterpret_cast<MSG_CREATE_CHARAC_VIEW*>(&msg);
    memset(&v, 0, 0x30);
    if (!packet.get_byte(v.m_slot))
    {
        return LineFunc(0x749,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // ORIG +0x6c..+0x80：两独立装载两次判（先无符号上限 `cmp $0xb; jg` 岛、
    // 再重载 `test %al; jns` pass）= `a>11 || (char)a<0` 短路\|\|形。
    if ((signed char)v.m_slot > 0xb || (signed char)v.m_slot < 0)
    {
        return LineFunc(0x74b,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (v.m_slot == 0xb)
    {
        return LineFunc(0x763,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // +0x0e 名字长度 int（m_padE[4] 起点，ORIG 逐字节读同一槽）
    // ORIG 081c0297 `mov 0xe(%eax),%eax` 单步 disp ⇒ 该 4B 区为真实 int 字段。
    // 冻结头 PacketTypes.h 内 m_padE[4] 无法字段化（已报 Main），本 TU 用同地址视图。
    if (!packet.get_int(v.m_nameLen))
    {
        return LineFunc(0x766,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (v.m_nameLen < 4 || 0x12 < v.m_nameLen)
    {
        return LineFunc(0x76a,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_str(v.m_name, 0x1d, v.m_nameLen))
    {
        return LineFunc(0x76e,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(v.m_name[0x1d]))                 // +0x2f 末字节标志
    {
        return LineFunc(0x780,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (v.m_name[0x1d] < 0)
    {
        return LineFunc(0x782,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
