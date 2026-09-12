// ORIG 0x0811700e：置标志；trace 行号 16；(%x%%) 打印 m_ratio。
// 带参 trace：m_ratio 先物化 ebx 跨 ctor 存活（P37 链式临时形态）。
void CDeathTowerWinPointEvent::StartEvent()
{
    SetEventFlag(true);
    // [R11fixE5] ORIG 0x08117055 串=0x8b4dd38（raw 逐字节
    // 'EVENT_REFORM : Death Tower Win Point Event Start! (%x%%)'；旧文 'EVENT_FORM'）。
    cMyTrace(__PRETTY_FUNCTION__, 16, 0)("EVENT_REFORM : Death Tower Win Point Event Start! (%x%%)", m_ratio);
}