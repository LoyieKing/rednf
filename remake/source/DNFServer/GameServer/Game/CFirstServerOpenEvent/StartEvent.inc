// ORIG 0x0811cae2：置标志；trace 行号 32。
void CFirstServerOpenEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace trace(__FUNCTION__, 32, 0);
    trace("First Server Open Event Start!");
}