int Dispatcher_GrowthCreatureChangeInfinityCreature::check_error_event(CUser* user)
{
    if (GlobalData::s_event_manager->GetRepeatEvent(0x67)->IsEventing(0))  // 0x081e8729 虚槽 +0x34
    {
        // b28d-D0: cast 移到赋值点（receiver 为具名派生指针 ⇒ 4.4 实参先于 this 装载，
        //   复现 ORIG 0x081e874d `mov 0xc(%ebp),%eax; mov %eax,0x4(%esp); mov -0xc(%ebp),%eax`；
        //   调用点 cast（static_cast<派生*>(ev)->m()）恒 this 先 + edx 中转，探针 /tmp/d0_probe2.cpp g1/g2）。
        GiveGrowCreatureEvent* ev = static_cast<GiveGrowCreatureEvent*>(GlobalData::s_event_manager->GetRepeatEvent(0x67));
        if (ev != 0)
            return ev->CheckEventInfo(*user);
    }
    return 1;
