int Dispatcher_BingoReward::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x0825e68c：param+4 = check_error(user,msg)；user!=0 时 param+8 =
    // *(int*)user->getBingoData()；>0：logFormat(0x19a,"Dispatcher Check_Error No(%d)")
    // + LineFunc(0x19b)；<1 且 >=0：getBingoData()+6/+5 置 1，取 repeat 事件 0x6a
    // 非空则 sendMaxMatchLineReward(*user) + sendDecorationObjectCnt(1)，恒返 0。
    stBingoReward& p = static_cast<stBingoReward&>(param);              // -0x10（ORIG 缓存 param）
    p.m_result = check_error(user, msg);                                // P0 无 err 局部，直存后重读
    if (user != 0)
    {
        // [b28c-30] P211 字段侧同型化：目标 m_bingoData 为 unsigned int
        // （PacketTypes.h:2092 冻结头）⇒ 源 cast 同步 unsigned，复现 ORIG
        // 单段直落 `mov (%eax),%edx`（int 源异型恒插 `mov %eax,%edx` 中转）。
        p.m_bingoData = *reinterpret_cast<unsigned int*>(user->getBingoData());
    }
    if (p.m_result > 0)
    {
        return 0;
    }
    if (p.m_result < 0)
    {
        LogManager::logFormat(
            1, "PacketDispatcher_Impl_2.cpp",
            __PRETTY_FUNCTION__,
            0x19a, "Dispatcher Check_Error No(%d)",
            p.m_result);
        return LineFunc(0x19b,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // [b28c-30] P82 具名视图：ORIG 0x0825e772/0x0825e787 为 `movb $0x1,0x6(%eax)`
    // /`movb $0x1,0x5(%eax)` disp 直写；reinterpret_cast<uchar*>[k] 下标形反产
    // `add $k,%eax; movb $1,(%eax)`（各 +1 条）。P48 无 CSE：两次独立 getBingoData。
    static_cast<BingoCheckView*>(user->getBingoData())->m_b6 = 1;
    static_cast<BingoCheckView*>(user->getBingoData())->m_b5 = 1;
    BingoEvent* event =
        reinterpret_cast<BingoEvent*>(GlobalData::s_event_manager->GetRepeatEvent(0x6a));
    if (event != 0)
    {
        event->sendMaxMatchLineReward(*user);
        user->sendDecorationObjectCnt(1);
    }
    return 0;
