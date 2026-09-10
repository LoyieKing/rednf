void CMemberExpRewardEvent::StartEvent(Word_Param param)
{
    SetEventFlag(true);
    m_ratio = (unsigned int)param.m_duration;
    cMyTrace(__FUNCTION__, 29, 0)("Member Exp Reward Event! ratio(%d)", param.m_duration);
