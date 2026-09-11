void CSchoolMatchEvent::StartEvent()
{
    // ORIG 0x08273f9a：SetEventFlag(true) + 日志。
    SetEventFlag(true);
    cMyTrace trace(__PRETTY_FUNCTION__, 0x15, 0);
    trace("CSchoolMatchEvent Start!");
}