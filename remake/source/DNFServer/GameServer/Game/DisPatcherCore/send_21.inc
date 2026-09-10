void DisPatcher_DropItem::send(CUser* user, ParamBase& param)

{
    stDropItemParam& out = static_cast<stDropItemParam&>(param);
    if (out.m_errNo != 0)
    {
        if (out.m_c == 1)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x32, out.m_errNo & 0xff);
        }
        else
        {
            PacketGuard guard;
            guard.put_header(1, 0x32);
            guard.put_byte(0);
            guard.put_byte(out.m_errNo);
            guard.put_byte(static_cast<char>(out.m_8));
            guard.finalize(true);
            user->Send(guard);
        }
    }
}
