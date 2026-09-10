void AttendanceEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace trace(__PRETTY_FUNCTION__, 0x23, 0);
    trace("Attendacne Event End!");
