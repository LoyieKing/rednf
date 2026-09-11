void CMacro::RudMacro()                                 // 0x084a8686
{
    m_state = 2;                                        // running
    for (size_t i = 0; i < m_data.size(); ++i)
    {
        // b28d-D42：ORIG 0x084a8686 只调一次 operator[] 并把结果存 -0xc 槽，
        // 之后逐字段 `mov -0xc(%ebp),%eax` 重读（非 P157 的逐点重算形）；
        // 4.4 对逐点表达式改把 m_len 缓到 %ebx（push %ebx + 帧 -4）⇒ 用具名引用。
        stMacroData& cur = m_data[i];
        if (cur.m_data != 0)
        {
            SendPacket(cur.m_data, cur.m_len);
        }
    }
}