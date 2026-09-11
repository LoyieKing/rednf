// 经 GiveGrowCreatureEvent vtable+0x34 槽（IsEventing，ORIG vtable 0x08b4bae0 实证）
// 判事件开启；失败路径返回 1。
// b28c-L20: 第一段 GetRepeatEvent 右值内联（P0 无槽）；正极性 `if (IsEventing(0))`（ORIG
//   0x081e872b 无 xor，`test %al; je <mov $1 岛>`）；第二段具名 ev（-0xc 槽，cmpl/je）。
//   残余 4 行 = 尾调实参装载 user 寄存器 %eax(ORIG)↔%edx(4.4) 对调（N82附：多实参 call
//   首值 callee-saved 对调；限定调用/具名指针/裸调用七形穷举无效）。
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
}