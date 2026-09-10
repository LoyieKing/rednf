void CMemberExpRewardEvent::EndEvent()
{
    SetEventFlag(false);
    m_ratio = 10;
    cMyTrace trace(__FUNCTION__, 36, 0);
    trace("Member Exp Reward Event");
