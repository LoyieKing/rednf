void CGuildWarEvent::StartEvent(Word_Param param)
{
    if (!IsEventing(0))
    {
        SetEventFlag(true);
        m_duration = param.m_duration;
        m_field0c = param.m_field2;
        if (G_GameWorld()->GetChannelType() == 6)
        {
            G_TimerQueue()->InsertTimer(
                TimerEntry::OBJ_TYPE_2, 0, TIMER_MESSAGE_0x62,
                (int)m_duration * 0xe10 - 600, 10, 0);
            GlobalData::s_guild_proxy_mgr
                ->GetServerProxy((ENUM_SERVER_GROUP)0)
                ->SendGuildWarStart(
                    (unsigned char)G_CEnvironment()->m_serverEnvir.m_channelNo);
            cMyTrace(__PRETTY_FUNCTION__, 0x39, 0)(
                "Guild War Event Start! time(%d), server(%d)",
                (int)param.m_duration,
                G_CEnvironment()->m_serverEnvir.m_channelNo);
        }
    }
}