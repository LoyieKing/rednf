// ORIG 0x0811700e：置标志；trace 行号 16；(%x%%) 打印 m_ratio。
// 带参 trace：m_ratio 先物化 ebx 跨 ctor 存活（P37 链式临时形态）。
void CDeathTowerWinPointEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace(__PRETTY_FUNCTION__, 16, 0)("EVENT_FORM : Death Tower Win Point Event Start! (%x%%)", m_ratio);
}