void GameWorld::SetWeekendBounsEvent()
{
    time_t t = GlobalData::s_systemTime_.getCurSec();
    struct tm local;
    localtime_r(&t, &local);
    // ORIG 86d12dc/86d12ec/86d12f3: mov -0x24(%ebp) = local+0x18 = tm_wday
    // b28d-lane11 新试形（P223 定案① 负条件 goto 共享尾标签串）：ORIG 尾
    // 0x086d1358/0x086d13ac 两 jmp + 0x086d13ae/0x086d13b1/0x086d13b4 三 nop 岛
    // + 0x086d13b7 fall nop；标签序 L0(0x3ac)/L3(0x3ae)/L1(0x3b1)/L2(0x3b4)。
    if ((local.tm_wday == 6 && local.tm_hour > 5) || local.tm_wday == 0 ||
        (local.tm_wday == 1 && local.tm_hour < 6))
    {
        if (!GlobalData::s_event_manager->GetRepeatEvent(0x57)->IsEventing(0))
        {
            Word_Param param;
            param.m_duration = 0;
            param.m_field2 = 0;
            GlobalData::s_event_manager->TriggerEventStart(0x57, param);
        }
        else
        {
            goto L3;
        }
        goto END;
    }
    else
    {
        if (local.tm_wday != 1)
        {
            goto L1;
        }
        if (local.tm_hour <= 5)
        {
            goto L2;
        }
        if (GlobalData::s_event_manager->GetRepeatEvent(0x57)->IsEventing(0))
        {
            GlobalData::s_event_manager->TriggerEventEnd(0x57);
            goto L0;
        }
        return;
    }
    // b28d-lane11 判词（0x086d1358/0x086d13ac/0x086d13ae/0x086d13b1/0x086d13b4
    // /0x086d13b7 实证）：if/else 保留 then 块物理前置 + 尾标签串造岛后 76=76，
    // 唯 L3(0x3ae) 岛的 jmp 被 4.4 thread 到相邻同目标 L1 岛的 jmp（P223 边界：
    // 单源 je 进入的尾岛恒合并）⇒ ndiff 封 1（N223）。
L0:;
L3:;
L1:;
L2:;
END:;
