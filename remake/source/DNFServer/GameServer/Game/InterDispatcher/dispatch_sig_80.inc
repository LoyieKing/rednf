// ORIG 0x084d02ee T dispatch_sig —— 生物蛋孵化结果更新（校验当前角色后透传）。
// 报告 docs/class_func_reports/Inter_UpdateCreatureItemResult/dispatch_sig.md 全文转写。
int Inter_UpdateCreatureItemResult::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    // ORIG 0x084d02xx：同 InsertCreatureItemResult —— 同型 lvalue 实参晚装填（见彼处注释）。
    SIG_CREATURE_ITEM* pPkt = reinterpret_cast<SIG_CREATURE_ITEM*>(data);
    if (user->getCurCharacR() == 0 ||
        user->getCurCharacNo() != pPkt->m_characNo)
        return 0x2e5d;
    user->getCurCharacInvenW()->GetCreatureMgrW()->UpdateCreatureItemResult(
        pPkt);
    return 0;
}