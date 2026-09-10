void CBurningFatigueEvent::StartEvent(Word_Param /*param*/)
{
    // ORIG 0x810a048：SetEventFlag(true) + 日志。
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x1b, 0);
    tr("Burning Fatigue Event Start");
