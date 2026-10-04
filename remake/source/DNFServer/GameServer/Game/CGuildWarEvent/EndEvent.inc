void CGuildWarEvent::EndEvent()
{
    if (IsEventing(0))
    {
        if (G_GameWorld()->GetChannelType() == 6)
        {
            GlobalData::s_guild_proxy_mgr
                ->GetServerProxy((ENUM_SERVER_GROUP)0)
                ->SendGuildWarEnd(
                    (unsigned char)G_CEnvironment()->m_serverEnvir.m_channelNo);
        }
        SetEventFlag(false);
        m_duration = 0;
        m_field0c = 0;
        cMyTrace(__PRETTY_FUNCTION__, 0x49, 0)("Guild War Event End");
    }
}