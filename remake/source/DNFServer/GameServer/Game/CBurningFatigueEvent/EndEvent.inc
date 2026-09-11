void CBurningFatigueEvent::EndEvent()
{
    // ORIG 0x810a09a：SetEventFlag(false) + 日志。
    SetEventFlag(false);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x21, 0);
    tr("Burning Fatigue Event End");
}