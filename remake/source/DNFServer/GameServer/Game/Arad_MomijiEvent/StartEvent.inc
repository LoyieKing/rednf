// ORIG 0x0819e7a2：cMyTrace(PRETTY, 0x14, 0)("[MOMIJI_EVENT] Event Start.")
// 在前，SetEventFlag(true) 在后。
void Arad_MomijiEvent::StartEvent()
{
    cMyTrace trace(__PRETTY_FUNCTION__, 0x14, 0);
    trace("[MOMIJI_EVENT] Event Start.");
    SetEventFlag(true);
}