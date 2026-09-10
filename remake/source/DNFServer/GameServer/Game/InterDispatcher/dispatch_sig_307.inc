int Inter_LoadEventLevelupSupport::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_LoadEventLevelupSupportPktView* pPkt = reinterpret_cast<Inter_LoadEventLevelupSupportPktView*>(data);
    if (user == 0 || data == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__,
            0x6dee, 5)("[%s] pUser or p is null.",
              __PRETTY_FUNCTION__);
        return 0x6def;
    }

    int* const local_2c =&pPkt->m_0;

    user->m_levelupEvents.clear();
    user->m_levelupEvents =
        ARAD::Singleton<LevelupSupportEventManger>::Get()->GetDefaultEventInfo();

    // [A02-r5] 轴 B（循环变量符号性）单点实验留痕：按 A01 形改 `int` + `(int)size()`
    //（P196 前置：ORIG 该族为有符号序 `setle`，允许改 int）。**实测结果**：本件 ndiff
    // 175→174（−1 条）、TU 四数不变 ⇒ 保留；但两侧 setcc 序列仍差（OURS 首点 `setbe`
    // 经核源自**另一处** unsigned 循环，非本循环）⇒ setcc 对齐未达成，余部按 N（N82/P112
    // 帧槽 + 该 unsigned 循环的 B 轴点）封顶。
    for (int local_28 = 0;
         local_28 < (int)user->m_levelupEvents.size();
         ++local_28)
    {
        if (user->m_levelupEvents[local_28].m_level <=
            (unsigned int)user->get_charac_level())
        {
            user->m_levelupEvents[local_28].m_state = 1;
        }
    }

    for (int local_24 = 0; local_24 < *local_2c; ++local_24)
    {
        LevelupSupportEventManger::EventInfo& info =
            user->m_levelupEvents[local_24];
        if (info.m_level == (short)local_2c[local_24 + 1])
        {
            info.m_state =
                *reinterpret_cast<unsigned short*>(
                    reinterpret_cast<char*>(local_2c) + local_24 * 4 + 6);
        }
        else
        {
            LogManager::logFormat(
                1, "InterDispatcher.cpp",
                __PRETTY_FUNCTION__,
                0x6e06,
                "Inter_LoadEventLevelupSupport diff state charac_no:%d",
                user->getCurCharacNo());
        }
    }

    if (*local_2c == 0)
    {
        for (unsigned int local_20 = 0;
             local_20 < user->m_levelupEvents.size();
             ++local_20)
        {
            const unsigned short state = user->m_levelupEvents[local_20].m_state;
            const unsigned short level = user->m_levelupEvents[local_20].m_level;
            DB_SaveEventLevelupSupport::makeRequest(
                static_cast<int>(user->GetUID()),
                static_cast<int>(user->getCurCharacNo()),
                static_cast<int>(level), static_cast<int>(state));
        }
    }

    LevelupSupportEventManger::SendCharacState(user, 0);
    return 0;
