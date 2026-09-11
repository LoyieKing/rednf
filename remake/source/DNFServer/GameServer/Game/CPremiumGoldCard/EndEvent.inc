// ---------------------------------------------------------------------------
// ORIG 0x0811c508：清事件标志并打日志（__PRETTY_FUNCTION__，line 0x1a）。
void CPremiumGoldCard::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x1a, 0);
    tr("CPremiumGoldCard Event End!");
}