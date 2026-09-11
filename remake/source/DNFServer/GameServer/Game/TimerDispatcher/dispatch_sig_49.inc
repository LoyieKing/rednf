bool TimerSaveLogGamechannel::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    // CLogGameChannel 三方法为无参实例调用（ORIG 0x08550456/0x085506c6/0x085503fe）。
    GlobalData::s_pLogGameChannel->ReqSaveLog();
    GlobalData::s_pLogGameChannel->ResetInOutUser();
    GlobalData::s_pLogGameChannel->SetNextLogTimer();
    return 1;
}