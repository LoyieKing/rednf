// 0x08117096：清标志；trace 行号 34。
void CDeathTowerWinPointEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace trace(__PRETTY_FUNCTION__, 34, 0);
    trace("Death Tower Win Point Event End!");
}