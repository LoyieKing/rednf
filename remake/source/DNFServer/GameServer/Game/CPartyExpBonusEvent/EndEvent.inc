// ORIG T 0x08116cf8：清事件标志并输出 "Party Exp Bonus Event End!"。
void CPartyExpBonusEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace trace(__PRETTY_FUNCTION__, 0x1b, 0);
    trace("Party Exp Bonus Event End!");
}