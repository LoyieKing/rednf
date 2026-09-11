bool TimerDetectDisconnect::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    // ORIG 0x86374bc: GetInstanceDisconnectDetecter()->DoDetectingDisconnect() 为真时 registNextTimer(); return 1
    if (disconnect_detecter::GetInstanceDisconnectDetecter()->DoDetectingDisconnect()) {
        registNextTimer();
    }
    return 1;
}