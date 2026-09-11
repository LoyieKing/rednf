// ==== Dispatcher_FatigueAttendance::check_error (ORIG 0x081db310) ====
int Dispatcher_FatigueAttendance::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x081db310（报告 Dispatcher_FatigueAttendance/check_error.md 全文已读）：
    // 0xc(%ebp)=user 判空 → -1；msg 死存储@-0xc（O0 保留初始化 store）；
    // getCurCharacR()==0（(bool) sete 物化）→ -1；state!=3（setne）→ 99；
    // is_eventing 链式单表达式（mng 结果 %eax 直传，无局部槽）。
    if (user == 0)
    {
        return -1;
    }
    const MSG_BASE* pMsg = &msg;   // ORIG 死存储 -0xc(%ebp)（判空之后）
    (void)pMsg;
    if ((bool)(user->getCurCharacR() == 0))
    {
        return -1;
    }
    if ((bool)(user->get_state() != 3))
    {
        return 99;
    }
    if (!reinterpret_cast<EventClassify::CEventScriptMng*>(
            G_CDataManager()->get_event_script_mng())->is_eventing(0x56))
    {
        return 7;
    }
    return 0;
}