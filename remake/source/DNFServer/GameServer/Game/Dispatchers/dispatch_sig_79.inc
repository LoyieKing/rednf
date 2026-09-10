int Dispatcher_ReplyGuildInvite::dispatch_sig(CUser* user, PacketBuf& packet)
{
    unsigned char reply = 0;
    if (user != 0 && user->get_state() > 2 && user->getCurCharacR() == 0)
    {
        if (!packet.get_byte(reply))
        {
            return LineFunc(
                0xa27c,
                __PRETTY_FUNCTION__,
                0, 0);
        }
        if (G_GameWorld()->IsIntegratedPvPBaseChannel() != 0)
        {
            PacketGuard response;
            response.put_header(1, 0x9b);
            response.put_byte(0);
            response.put_int(99);
            response.finalize(true);
            user->Send(response);
            return 0;
        }
        const int secuRet =
            GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0xc);
        if (secuRet != 0)
        {
            reply = static_cast<unsigned char>(
                GlobalData::s_pSecuProtectionField->GetOppositeErr(secuRet));
        }
        const unsigned int characNo = user->getCurCharacNo();
        CGuildServerProxy* proxy =
            GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup());
        proxy->SendReplyGuildInvite(characNo, reply);
        return 0;
    }
    return LineFunc(
        0xa278,
        __PRETTY_FUNCTION__,
        0, 0);
