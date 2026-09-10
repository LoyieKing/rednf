void CCharacterDayEvent::StartEvent()
{
    // ORIG 0x0810a53c：SetEventFlag(true) + cMyTrace(__FUNCTION__,0x17,0)(...)。
    SetEventFlag(true);
    cMyTrace(__FUNCTION__, 0x17, 0)("Character Day Event Start!");
}
