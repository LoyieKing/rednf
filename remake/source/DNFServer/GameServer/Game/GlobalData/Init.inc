bool CPCRoomServerProxy::Init()
{
    if (!InitTcp()) return 0;
    return 1;
