// EndEvent @ 0x080c9d30：置事件标志 + 日志。
void BingoEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace trace(__PRETTY_FUNCTION__, 0x3f, 0);
    trace("BingoEvent Event End!");
}