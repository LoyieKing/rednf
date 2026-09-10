void RegistServerSchedule()
{
    // 0x50 / 0x52：固定 0 秒
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0, (TIMER_MESSAGE)0x50, 0, 0, 0);
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0, (TIMER_MESSAGE)0x52, 0, 0, 0);

    // 0x54：对齐到整半点（ORIG：1740 - 60*(tm_mon - 30*(tm_mon/15)) + (60 - tm_sec)）
    {
        std::time_t now = std::time(0);
        std::tm t;
        // P43：ORIG 0x082a04fb `mov %eax,-0x18; mov 0x4(%eax),%ecx` 经返回值指针读字段。
        std::tm* p = localtime_r(&now, &t);
        // ORIG 0x082a0504 magic 0x88888889 + `sar $0x4` = **÷30**（sar 3 才是 ÷15），
        // 随后 `v = tm_min - 30*q` ⇒ 源形 `tm_min % 30`（半点对齐；旧实现误用
        // tm_mon/15，属真语义 bug）。v 无槽（寄存器直算），故内联进 delay 表达式。
        int delay = 1740 - 60 * (p->tm_min % 30) + (60 - p->tm_sec);
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0,
                                    (TIMER_MESSAGE)0x54, delay, 0, 0);
    }

    // 0x55：TimerStatisticLevel 下一统计点距当前秒数（ORIG 0x082a0588：差在 %ebx
    // 跨 G_TimerQueue() 调用保活，无具名槽 ⇒ P24 内联实参）。
    G_TimerQueue()->InsertTimer(
        (TimerEntry::OBJ_TYPE)2, 0, (TIMER_MESSAGE)0x55,
        TimerStatisticLevel::CalcNextTime() - static_cast<int>(std::time(0)), 0, 0);

    // 0x59 / 0x5a / 0x5b / 0x61 / 0x66 / 0x89：ORIG 逐个独立 G_TimerQueue()->InsertTimer
    // （非循环；循环只发 1 个 call 点，P167 缺 5）。
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0, (TIMER_MESSAGE)0x59, 0, 0, 0);
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0, (TIMER_MESSAGE)0x5a, 0, 0, 0);
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0, (TIMER_MESSAGE)0x5b, 0, 0, 0);
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0, (TIMER_MESSAGE)0x61, 0, 0, 0);
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0, (TIMER_MESSAGE)0x66, 0, 0, 0);
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0, (TIMER_MESSAGE)0x89, 0, 0, 0);

    if (IsLightServer())
    {
        // 0x68：每条 light server notice 一条 0 秒定时器，param2=序号。
        // ORIG 读 CDataManager+0xa4 vector<stLightServerNotice_t> —「推断」布局。
        std::vector<stLightServerNotice_t>* notices =
            reinterpret_cast<std::vector<stLightServerNotice_t>*>(
                reinterpret_cast<char*>(G_CDataManager()) + 0xa4);
        for (unsigned int i = 0; i < notices->size(); ++i)
            G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0,
                                        (TIMER_MESSAGE)0x68, 0, (int)i, 0);
    }

    TimerScheduleTenSecond::registNextTimer(10);

    // 0x8d：固定 0 秒
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0, (TIMER_MESSAGE)0x8d, 0, 0, 0);

    if (!G_GameWorld()->IsPVPChannel())
        Timer_DungeonInoutOpenTime::RegistNextTimer();

    // 0x8e：delay = (channel_no % 10)^2 * ... ORIG：((c%10)*16 - (c%10))*... 见下
    {
        int c = G_CEnvironment()->get_channel_no();
        int m = c % 10;
        // ORIG 转写：eax=(m<<2)=4m；delay=(4m<<4)-4m=60m。
        int delay = 60 * m;
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0,
                                    (TIMER_MESSAGE)0x8e, delay, 0, 0);
    }

    // 0x9a：固定 0 秒（ORIG 0x82a08af，0x8e 之后注册）
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0, (TIMER_MESSAGE)0x9a, 0, 0, 0);

    // ORIG 0x82a08cf：尾部 checkContinus + loadServerState（全局 0x941f7f0）
    if (GlobalData::s_serverStateManager_ != 0)
    {
        GlobalData::s_serverStateManager_->checkContinus();
        GlobalData::s_serverStateManager_->loadServerState();
    }
