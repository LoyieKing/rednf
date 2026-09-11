void CSchoolMgr::SendBroadcastUserIn(CUser* user)
{
    const char* channelMsg = g_scriptStringManager_.findString(
        4, "channel_connect", 0);
    if (channelMsg != 0)
    {
        char buf[255] = {0};
        sprintf(buf, channelMsg, user->getCurCharacName(),
                G_CEnvironment()->get_channel_no());
        PacketGuard guard;
        guard.put_header(0, 0xc);
        guard.put_byte(0);
        guard.put_short(0);
        guard.put_byte(0);
        guard.put_int(strlen(buf));
        guard.put_str(buf, strlen(buf));
        guard.finalize(true);
        CSchoolMember member(user);
        G_GameWorld()->send_group(guard, &member);
    }
}