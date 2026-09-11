// ORIG 0x080c83f4：SetEventFlag(false) + cMyTrace(PRETTY, 0x23, 0)
//   ("Attendacne Event End!")（ORIG 原文拼写 Attendacne）。
void AttendanceEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace trace(__PRETTY_FUNCTION__, 0x23, 0);
    trace("Attendacne Event End!");
}