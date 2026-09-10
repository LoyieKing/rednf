void SeriaRoomAniDecoEvent::EndEvent()                  // 0x0827b59e
{
    SetEventFlag(false);
    cMyTrace trace(__PRETTY_FUNCTION__, 0x24, 0);
    trace("SeriaRoomAniDecoEvent Event end!");
}
