void BlueMarbleDungeonEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace trace(__FUNCTION__, 0x22, 0);
    trace("BlueMarbleDungeonEvent End!");
