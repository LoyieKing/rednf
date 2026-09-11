void CSchoolMatchEvent::EndEvent()
{
    // ORIG 0x0827423c：SetEventFlag(false) + 联赛匹配关闭 + 日志 + 清频道表。
    SetEventFlag(false);
    CLeagueMatch::SetMatchAble(false);
    cMyTrace trace(__PRETTY_FUNCTION__, 0x3f, 0);
    trace("CSchoolMatchEvent End!");
    m_channels.clear();
}