void COnTimeEvent::EndEvent()
{
    // ORIG @0x081bb91a：SetEventFlag(false) 在 cMyTrace 构造前调用。
    SetEventFlag(false);
    cMyTrace trace(__PRETTY_FUNCTION__, 0x4d, 0);
    trace("COnTimeEvent Event End!");
}