// TimerScheduleGuildWarEnd::dispatch_sig ORIG T 0x86331d0 (0x57f)
// [b28d-30] 复核 DIFF：巨件残差（N82/N89 族），ORIG 0x086331d0。
// [b28d-solo-TimerDispatcher] 判词 N82/N89 封顶：巨件 317 insns，帧槽 + STL 内联残差。
// [TimerDispatcher-2 2026-09-09 复核] gap=6 全为 6 条 ORIG-only `jmp`（每 case 一条
// 跳正常析构的中继；ORIG 正常路径 D1 落 catch 块之后、4.4 就地内联）⇒ N104 EH 尾块
// 相对序；两侧指令多重集其余全等（ORIG-only={'jmp':6}，OURS-only=∅）⇒ 无语义缺口。
// [TimerDispatcher-3 2026-09-10 判据①] 全 TU jcc/jmp 目标下标逐位核对：ORIG 23 条 vs
// 我方 17 条；6 条缺口全是每 case 尾 `jmp <该 case 共享 D1Ev 中继>`（目标落在 catch
// 之后），jne/je 目标逐条对应下一 case 比较与前置守卫 ⇒ 无极性反写、无语义缺口。
// [fix] ORIG 全部 6 处 case 比较读 0x10(%ebp)=**p2**（Ghidra 标 param_3 不准，
// 与本 TU TimerGlobalEffectItemBegin/Timeout 等同一模式）⇒ 旧码用 p3(0x14) 是参数用错。
bool TimerScheduleGuildWarEnd::dispatch_sig(int p1, int p2, memberIdentificationNumber_t /*p3*/)
{
    if (G_GameWorld()->GetChannelType() != 6)
    {
        return 1;
    }
    // ORIG 08633209：CEventManager::GetRepeatEvent(0x21)；vptr+0x34 虚调用判活
    // P0：ORIG 无 event 具名槽（0863320e `mov (%eax),%edx` 直用返回值）⇒ 内联表达式。
    if (!GlobalData::s_event_manager->GetRepeatEvent(0x21)->IsEventing((CUser*)0))
    {
        return 1;
    }
    if (p2 == 10)
    {
        PacketGuard packet;  // ORIG local @ -0x14(%ebp)
        packet.put_header(0, 0x6d);
        packet.put_byte(10);
        packet.finalize(true);
        G_GameWorld()->send_all(packet);
        // ORIG 086332d5：InsertTimer(OBJ_TYPE_2,0,0x62,300,5,0)
        G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                    (TIMER_MESSAGE)0x62, 300, 5, 0);
    }
    else if (p2 == 5)
    {
        PacketGuard packet;  // ORIG local @ -0x20(%ebp)
        packet.put_header(0, 0x6d);
        packet.put_byte(5);
        packet.finalize(true);
        G_GameWorld()->send_all(packet);
        // ORIG 086333a9：InsertTimer(OBJ_TYPE_2,0,0x62,60,4,0)
        G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                    (TIMER_MESSAGE)0x62, 60, 4, 0);
    }
    else if (p2 == 4)
    {
        PacketGuard packet;  // ORIG local @ -0x2c(%ebp)
        packet.put_header(0, 0x6d);
        packet.put_byte(4);
        packet.finalize(true);
        G_GameWorld()->send_all(packet);
        // ORIG 0863347d：InsertTimer(OBJ_TYPE_2,0,0x62,60,3,0)
        G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                    (TIMER_MESSAGE)0x62, 60, 3, 0);
    }
    else if (p2 == 3)
    {
        PacketGuard packet;  // ORIG local @ -0x38(%ebp)
        packet.put_header(0, 0x6d);
        packet.put_byte(3);
        packet.finalize(true);
        G_GameWorld()->send_all(packet);
        // ORIG 08633551：InsertTimer(OBJ_TYPE_2,0,0x62,60,2,0)
        G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                    (TIMER_MESSAGE)0x62, 60, 2, 0);
    }
    else if (p2 == 2)
    {
        PacketGuard packet;  // ORIG local @ -0x44(%ebp)
        packet.put_header(0, 0x6d);
        packet.put_byte(2);
        packet.finalize(true);
        G_GameWorld()->send_all(packet);
        // ORIG 08633625：InsertTimer(OBJ_TYPE_2,0,0x62,60,1,0)
        G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                    (TIMER_MESSAGE)0x62, 60, 1, 0);
    }
    else if (p2 == 1)
    {
        PacketGuard packet;  // ORIG local @ -0x50(%ebp)
        packet.put_header(0, 0x6d);
        packet.put_byte(1);
        packet.finalize(true);
        G_GameWorld()->send_all(packet);
        // ORIG 086336f9：InsertTimer(OBJ_TYPE_2,0,0x62,60,0,0)
        G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                    (TIMER_MESSAGE)0x62, 60, 0, 0);
    }
    else if (p2 == 0)
    {
        // ORIG 0863373e：TriggerEventEnd(0x21)
        GlobalData::s_event_manager->TriggerEventEnd(0x21);
    }
    return 1;
}