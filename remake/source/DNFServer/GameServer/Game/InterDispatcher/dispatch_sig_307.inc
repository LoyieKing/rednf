// [b28d-lane2] verdict=DIFF ndiff=167（ORIG 0x084e8ce4）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-7。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=93（ORIG 0x84e8ce4）：N22 cmp 操作数方向+N18a 单段 regmove+N82/P112 帧槽平移。
int Inter_LoadEventLevelupSupport::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user == 0 || data == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__,
            0x6dee, 5)("[%s] pUser or p is null.",
              __PRETTY_FUNCTION__);
        return 0x6def;
    }

    Inter_LoadEventLevelupSupportEntryPktView* pPkt =
        reinterpret_cast<Inter_LoadEventLevelupSupportEntryPktView*>(data);

    user->m_levelupEvents.clear();
    user->m_levelupEvents =
        ARAD::Singleton<LevelupSupportEventManger>::Get()->GetDefaultEventInfo();

    // [A02-r5 复核 2026-09-11] ORIG 条件为无符号序（`cmp i,size; seta`）⇒ `(int)` cast 撤销；
    //   比较轴按 ORIG setle 取有符号（m_level(u16) 提升为 int vs get_charac_level() 返回 int）。
    for (int local_28 = 0;
         local_28 < user->m_levelupEvents.size();
         ++local_28)
    {
        if (user->m_levelupEvents[local_28].m_level <=
            user->get_charac_level())                  // setle（有符号，ORIG 直证）
        {
            user->m_levelupEvents[local_28].m_state = 1;
        }
    }

    for (int local_24 = 0; local_24 < pPkt->m_count; ++local_24)
    {
        if (user->m_levelupEvents[local_24].m_level ==
            pPkt->m_entries[local_24].m_level)         // ORIG `movzwl 0x4(%eax,%edx,4)`
        {
            user->m_levelupEvents[local_24].m_state =
                pPkt->m_entries[local_24].m_state;     // ORIG `movzwl 0x6(%edx,%ecx,4)`
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

    if (pPkt->m_count == 0)
    {
        for (unsigned int local_20 = 0;
             local_20 < user->m_levelupEvents.size();
             ++local_20)
        {
            DB_SaveEventLevelupSupport::makeRequest(
                static_cast<int>(user->GetUID()),
                static_cast<int>(user->getCurCharacNo()),
                static_cast<int>(user->m_levelupEvents[local_20].m_level),
                static_cast<int>(user->m_levelupEvents[local_20].m_state));
        }
    }

    LevelupSupportEventManger::SendCharacState(user, 0);
    return 0;
}