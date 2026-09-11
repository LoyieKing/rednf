// ORIG 0x0812001c：SetEventFlag(true) + cMyTrace(__FUNCTION__,0x18,0)；
// Word_Param 参数未使用（Ghidra C 仅 this 参与）。
void LevelUpBefore70LvEvent::StartEvent(Word_Param /*param*/)
{
    SetEventFlag(true);
    cMyTrace trace(__FUNCTION__, 0x18, 0);
    trace("LevelUpBefore70Lv Event start! [monitor request]");
}