void DisPatcher_UseCoin::send(CUser* user, ParamBase& param)
{
    stUseCoinParam& out = static_cast<stUseCoinParam&>(param);
    if (out.m_errNo == 0x7fffffff)
        return;
    {
        if (out.m_errNo == 0)
        {
            PacketGuard guard;
            guard.put_header(1, 0x2c);
            guard.put_byte(1);
            guard.put_short(out.m_8);
            guard.finalize(true);
            user->Send(guard);
        }
        else
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x2c, out.m_errNo & 0xff);
        }
    }
