// ORIG 0x0825e9b8 T _ZN20Dispatcher_BingoQuiz7processEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/Dispatcher_BingoQuiz/process.md 汇编+反编译 C 全文转写）
// param 为模板实参（+0x04 错误码 / +0x08 bingo 数据槽）。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x0825e9b8 orig/ours=71/73 ndiff-blocks=14
//   帧差 sub $38 vs $34 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x38,%esp | mov    0x10(%ebp),%eax | mov    %eax,-0x14(%ebp)
int Dispatcher_BingoQuiz::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x0825e9be `mov 0x10(%ebp),%eax; mov %eax,-0x14(%ebp)`（msg 具名引用槽，
    // 声明序先于 param 槽 -0x10）⇒ P0 补具名引用；0x0825ea81 `mov -0x14(%ebp),%eax;
    // mov 0xd(%eax),%eax` 直读 +0xd 具名字段（原 `(char*)&msg + 0xd` 产 add+mov 两条）。
    MSG_BINGO_QUIZ& req = static_cast<MSG_BINGO_QUIZ&>(msg);        // -0x14
    stBingoQuiz& out = static_cast<stBingoQuiz&>(param);            // -0x10
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
    if (event != 0)
    {
        out.m_result = event->reqBingoQuizMark(*user, req.m_0d);
    }
    else
    {
        out.m_result = 0x13;
    }

    out.m_bingoData =
        *reinterpret_cast<unsigned int*>(user->getBingoData());   // 字段 unsigned ⇒ 同型免 eax→edx 中转
    return 0;
}