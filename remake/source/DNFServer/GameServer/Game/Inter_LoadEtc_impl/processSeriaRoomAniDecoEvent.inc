void Inter_LoadEtc::processSeriaRoomAniDecoEvent(seriaRoom_AniDeco info, CUser& user)
{
    // [b28d-solo-Inter_LoadEtc_impl] verdict=AE 59/59：去 ev 局部（P134/P183 寄存器直传
    // 虚调用）+ 正极性 test;je 尾（P224）+ now/lastPlay 内联（%esi/%ebx 保活）+ 嵌套正向
    // if（消尾部 jmp+nop）。
    // [b28d-solo-Inter_LoadEtc_impl] ORIG 0x084e8430：GetRepeatEvent 返回值寄存器直传
    // 虚调用（P134/P183；无 ev 局部、无 null 检查），正极性 test;je 尾（P224）。
    if (GlobalData::s_event_manager->GetRepeatEvent(0x69)->IsEventing(0))
    {
        user.setSeriaRoomAniDecoInfo(info);
        SeriaRoomAniDecoEvent* seria = static_cast<SeriaRoomAniDecoEvent*>(
            GlobalData::s_event_manager->GetRepeatEvent(0x69));
        if (seria != 0)
        {
            seria->SendEventInfo(user);                 // ORIG T 0x0827b99a
            // ORIG 0x084e84a3：now/lastPlay 经 %esi/%ebx 跨 G_CEnvironment 保活（无槽，P0）
            if (CheckDailyScheduleTime(
                    G_CEnvironment()->m_serverEnvir.m_fatigueTime,
                    user.getCurCharacLastPlayTick(),
                    GlobalData::s_systemTime_.getCurSec()))
                seria->SendAniDecoItem(user);           // ORIG T 0x0827b75a
        }
    }
}