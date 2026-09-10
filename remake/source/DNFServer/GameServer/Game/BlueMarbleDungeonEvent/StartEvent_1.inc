void BlueMarbleDungeonEvent::StartEvent(Word_Param /*param*/)
{
    SetEventFlag(true);
    cMyTrace trace(__FUNCTION__, 0x1c, 0);
    trace("BlueMarbleDungeonEvent Start!");
