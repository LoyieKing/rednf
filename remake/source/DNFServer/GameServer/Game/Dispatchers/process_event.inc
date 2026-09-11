// ORIG 0x081e863a（报告 process_event.md）[随行依赖]，结构同上，失败返回 0。
// b28c-L20: 同 check_error_event 形，失败返回 0。
int Dispatcher_GrowthCreatureChangeInfinityCreature::process_event(CUser* user)
{
    if (GlobalData::s_event_manager->GetRepeatEvent(0x67)->IsEventing(0))  // 0x081e8667 虚槽 +0x34
    {
        // b28d-D0: 同 check_error_event，cast 前置到赋值点复现 ORIG 0x081e868b 实参先装载形。
        GiveGrowCreatureEvent* ev = static_cast<GiveGrowCreatureEvent*>(GlobalData::s_event_manager->GetRepeatEvent(0x67));
        if (ev != 0)
            return ev->processEvent(*user);
    }
    return 0;
}