// 报告反编译：SetEventFlag(true) → m_ratio=10 → trace。
void CMemberExpRewardEvent::StartEvent()
{
    SetEventFlag(true);
    m_ratio = 10;
    cMyTrace trace(__FUNCTION__, 22, 0);
    trace("Member Exp Reward Event Start!");
}