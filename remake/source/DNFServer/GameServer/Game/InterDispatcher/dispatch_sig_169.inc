int Inter_DisconnectMonitorServer::dispatch_sig(
    CUser* /*user*/, char* data, int /*len*/)
{
    // ORIG +0x06..+0x11：仅一个 int* 局部槽（-0xc），`*packet` 直接作实参
    // （再具名 int 局部会多两级落栈）。
    int* packet = reinterpret_cast<int*>(data);
    TimerConnectMonitorServer::registNextTimer(30L, 0, *packet);
    return 0;
}