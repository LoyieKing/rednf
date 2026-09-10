void CPCRoomFatigueEvent::EndEvent()
{
    // ORIG 0x082676ce：SetEventFlag(false) + cMyTrace(__FUNCTION__, 0x21, 0)。
    SetEventFlag(false);
    cMyTrace tr(__FUNCTION__, 0x21, 0);
    tr("PC Room Fatigue Event End!");
