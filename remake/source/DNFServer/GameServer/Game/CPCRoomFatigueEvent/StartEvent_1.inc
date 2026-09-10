void CPCRoomFatigueEvent::StartEvent(Word_Param /*param*/)
{
    // ORIG 0x0826767c：SetEventFlag(true) + cMyTrace(__FUNCTION__, 0x1a, 0)。
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x1a, 0);
    tr("PC Room Fatigue Event Start!");
