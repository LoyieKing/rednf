// ORIG 0x0819e80e：cMyTrace(PRETTY, 0x1f, 0)("[MOMIJI_EVENT] Event End.")
// 在前，SetEventFlag(false) 在后。
void Arad_MomijiEvent::EndEvent()
{
    cMyTrace trace(__PRETTY_FUNCTION__, 0x1f, 0);
    trace("[MOMIJI_EVENT] Event End.");
    SetEventFlag(false);
}