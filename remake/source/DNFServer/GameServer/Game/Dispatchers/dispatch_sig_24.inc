int Dispatcher_VerifyGold::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0822b??：无 user==0 防御；cmp $0x2; setle %al; test; je（技巧9 物化形）。
    if ((bool)(user->get_state() <= 2))
    {
        return 0;
    }
    unsigned short value = 0;
    if (!packet.get_short(value))
    {
        return LineFunc(0xde71,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    PacketGuard response;
    response.put_header(1, 0x11f);
    response.put_byte(1);
    response.put_short(value);
    response.put_int(user->getCurCharacMoney());
    response.finalize(true);
    user->Send(response);
    return 0;
