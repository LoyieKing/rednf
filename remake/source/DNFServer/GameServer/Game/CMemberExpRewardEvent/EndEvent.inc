// ORIG 0x081b2a5a：清标志；m_ratio 重置 10；trace 行号 36。
void CMemberExpRewardEvent::EndEvent()
{
    SetEventFlag(false);
    m_ratio = 10;
    cMyTrace trace(__FUNCTION__, 36, 0);
    trace("Member Exp Reward Event");
}