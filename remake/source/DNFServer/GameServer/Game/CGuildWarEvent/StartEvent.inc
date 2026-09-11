void CGuildWarEvent::StartEvent()
{
    // ORIG xor 形：call;xor $0x1;test;je 尾 ⇒ if (!IsEventing(0)) { body } 块形。
    if (!IsEventing(0))
    {
        SetEventFlag(true);
        m_duration = 5;
        m_field0c = 0;
        if (G_GameWorld()->GetChannelType() == 6)
        {
            G_TimerQueue()->InsertTimer(
                TimerEntry::OBJ_TYPE_2, 0, TIMER_MESSAGE_0x62,
                (int)m_duration * 0xe10 - 600, 10, 0);
            GlobalData::s_guild_proxy_mgr
                ->GetServerProxy((ENUM_SERVER_GROUP)0)
                ->SendGuildWarStart(
                    (unsigned char)((CEnvView*)G_CEnvironment())->m_gcNo);
            cMyTrace(__PRETTY_FUNCTION__, 0x27, 0)(
                "Guild War Event Start! time(%d)", 5);
        }
    }
}