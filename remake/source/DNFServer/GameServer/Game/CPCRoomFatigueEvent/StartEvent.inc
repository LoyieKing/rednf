void CPCRoomFatigueEvent::StartEvent()
{
    // ORIG 0x0826762a：SetEventFlag(true) + cMyTrace(__FUNCTION__, 0x13, 0)。
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x13, 0);
    tr("PC Room Fatigue Event Start!");
