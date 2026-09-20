// ==== Dispatcher_CeraPackageOpen::read (ORIG 0x081e76fa) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081e76fa orig/ours=95/98 ndiff-blocks=12
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    -0x10(%ebp),%eax | movl   $&_ZZN26Dispatcher_CeraPackageOpen4readER9PacketBufR8MSG_BASEE19__PRETTY_FUNCTION__,0x4(%esp) | mov    -0x10(%ebp),%e
int Dispatcher_CeraPackageOpen::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_OPEN_CERAPACKAGE& req = *reinterpret_cast<MSG_OPEN_CERAPACKAGE*>(&msg);
    if (!packet.get_short(reinterpret_cast<short&>(req.m_0e)))
    {
        return LineFunc(
            0x636b,
            __PRETTY_FUNCTION__,
            0, 0);
    }

    if (!packet.get_byte(req.m_count))
    {
        return LineFunc(
            0x636e,
            __PRETTY_FUNCTION__,
            0, 0);
    }

    if (req.m_count > 0x23)
    {
        return LineFunc(
            0x6371,
            __PRETTY_FUNCTION__,
            0, 0);
    }

    for (unsigned int i = 0; i < req.m_count; ++i)
    {
        if (!packet.get_int(req.m_items[i].m_id))
        {
            return LineFunc(
                0x6376,
                __PRETTY_FUNCTION__,
                0, 0);
        }
        if (!packet.get_byte(req.m_items[i].m_flag))
        {
            return LineFunc(
                0x6379,
                __PRETTY_FUNCTION__,
                0, 0);
        }
    }
    return 0;
}
