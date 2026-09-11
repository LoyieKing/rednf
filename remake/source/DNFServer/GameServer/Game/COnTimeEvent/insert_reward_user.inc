// ORIG @0x081bb9dc。双日志 + 已领过判定。
// 汇编要点：① 两个 256 数组为 { '\0' } 初始化（rep stos 内联，非 call memset）；
// ② 比较 setg（get_ontime_id() > last_recv_idx()，先求左操作数）；
// ③ insert 实参右→左内联求值 get_ontime_id→get_charac_no→get_acc_id；
// ④ insert 后先 set_ontime_last_recv_idx(get_ontime_id()) 再 set_update；
// ⑤ else 分支求值序 get_charac_no→get_acc_id→NumberToString→last_recv→ontime_id。
// [b28c-lane17] fresh：158=158 助记符全等 ⇒ NEAR；残差为互斥块内同类 cMyTrace/
// char[0x100] 临时槽深方向与 callee-saved 分配差（N82附/N86 族），
// 调用点差集与槽位集无缺项 ⇒ 语义完成、byte 层 N 封顶。
bool COnTimeEvent::insert_reward_user(CUser* user)
{
    if (user == 0)
        return false;

    char logDir[256] = { '\0' };
    sprintf(logDir, "./log/%s/OnTimeEvent", G_CEnvironment()->get_file_name());

    register unsigned int characNo = user->get_charac_no(-1);
    register char* accStr = NumberToString(user->get_acc_id(), 0);
    // [b28d-51] P221/P227：互斥 sibling 块同类日志对象槽深方向 —— 具名对象 4.4 反
    // （首块 -0x20/次块 -0x28），改全表达式临时链复现 ORIG（首块 -0x28/次块 -0x20）。
    CMyFileLog(__PRETTY_FUNCTION__, 0x75)(
        logDir, "On Time Event:On Request Reward m_id(%s) charac_no(%u)",
        accStr, characNo);

    if (get_ontime_id() > user->get_ontime_last_recv_idx())
    {
        m_rewardUsers.insert(user->get_acc_id(), user->get_charac_no(-1),
                             get_ontime_id());
        user->set_ontime_last_recv_idx(get_ontime_id());
        user->set_update_ontime_last_recv_idx();
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x120, 0);
        return true;
    }
    else
    {
        char errDir[256] = { '\0' };
        sprintf(errDir, "./log/%s/OnTimeEvent",
                G_CEnvironment()->get_file_name());

        register unsigned int characNo2 = user->get_charac_no(-1);
        register char* accStr2 = NumberToString(user->get_acc_id(), 0);
        register unsigned int lastIdx = user->get_ontime_last_recv_idx();
        register unsigned int ontimeId = get_ontime_id();
        CMyFileLog(__PRETTY_FUNCTION__, 0x89)(
            errDir,
            "On Time Event:Reward(no.%u user-%u)error m_id(%s)charac_no(%u)",
            ontimeId, lastIdx, accStr2, characNo2);
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x120, 0x12);
        return false;
    }
}