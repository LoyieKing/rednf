void CCharacterDayEvent::EndEvent()
{
    // ORIG 0x0810a5ec：SetEventFlag(false) + cMyTrace(__FUNCTION__,0x25,0)(...)。
    SetEventFlag(false);
    cMyTrace(__FUNCTION__, 0x25, 0)("Character Day Event End!");
}
