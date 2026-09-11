// ---------------------------------------------------------------------------
// ORIG 0x081174fe：SetEventFlag(false) +
// cMyTrace(__PRETTY_FUNCTION__,0x22,0)
// ("CFatigueAttendance Event End!")。
void CFatigueAttendance::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x22, 0);
    tr("CFatigueAttendance Event End!");
}