int Dispatcher_BingoQuiz::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stBingoQuiz& out = static_cast<stBingoQuiz&>(param);
    out.m_result = check_error(user, msg);
    if (out.m_result >= 1)
    {
        return 0;
    }
    if (out.m_result < 0)
    {
        LogManager::logFormat(1, "PacketDispatcher_Impl_2.cpp",
                              __PRETTY_FUNCTION__,
                              0x1f0, "Dispatcher Check_Error No(%d)", out.m_result);
        return LineFunc(0x1f1,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // ORIG GetRepeatEvent(0x6a) → BingoEvent
    BingoEvent* event = reinterpret_cast<BingoEvent*>(
        GlobalData::s_event_manager->GetRepeatEvent(0x6a));
    if (event == 0)
    {
        out.m_result = 0x13;
    }
    else
    {
        out.m_result =
            event->reqBingoQuizMark(*user, *(int*)((char*)&msg + 0xd));
    }
    out.m_bingoData =
        *reinterpret_cast<int*>(user->getBingoData());
    return 0;
