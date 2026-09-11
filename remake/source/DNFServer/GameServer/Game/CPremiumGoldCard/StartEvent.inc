// ---------------------------------------------------------------------------
// ORIG 0x0811c49c：置事件标志并打日志（__PRETTY_FUNCTION__，line 0x10）。
void CPremiumGoldCard::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x10, 0);
    tr("CPremiumGoldCard Event Start!");
}