void SeriaRoomAniDecoEvent::StartEvent(Word_Param)      // 0x0827b54c
{
    SetEventFlag(true);
    cMyTrace trace(
        __PRETTY_FUNCTION__, 0x1e, 0);
    trace("SeriaRoomAniDecoEvent Event start! [monitor request]");
