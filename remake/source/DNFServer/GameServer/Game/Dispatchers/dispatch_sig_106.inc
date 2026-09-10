int Dispatcher_SecurityCardDisuse::dispatch_sig(CUser* user, PacketBuf& packet)
{
    (void)packet;                                          // ORIG 不读包体
    if (user->get_state() != 3)
    {
        return LineFunc(0xa5dc,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!user->getSecurityCard()->isActivate())
    {
        return LineFunc(0xa5e3,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!user->getSecurityCard()->isCertified())
    {
        return LineFunc(0xa5e6,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if ((unsigned int)user->getSecurityCard()->getFailCnt() > 2)   // cmp $0x2; seta（无符号 >2）
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xa5, 0x7a);
        return 0;
    }
    PacketGuard guard;                                     // 0x0858dd4c
    guard.put_header(1, 0xa5);
    guard.put_byte(1);
    guard.finalize(true);
    user->Send(guard);
    DB_SecurityCardDisuse::makeRequest(user->get_acc_id());
    user->getSecurityCard()->init();                       // 0x085fdd68
    return 0;                                              // guard 析构 0x0858de80
