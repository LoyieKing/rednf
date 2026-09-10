void Arad_MomijiEvent::EndEvent()
{
    cMyTrace trace(__PRETTY_FUNCTION__, 0x1f, 0);
    trace("[MOMIJI_EVENT] Event End.");
    SetEventFlag(false);
