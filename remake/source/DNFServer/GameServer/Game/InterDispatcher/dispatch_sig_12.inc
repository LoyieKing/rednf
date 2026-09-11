// ORIG 0x08124db0 T dispatch_sig —— 行动点空载/当日奖励检查。
// 报告 docs/class_func_reports/APSystem__Inter_EmptyActionPoint/dispatch_sig.md 全文转写。
int APSystem::Inter_EmptyActionPoint::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user == 0 || user->get_state() == 0 || user->getCurCharacR() == 0)
        return 0x214;
    // ORIG +0x3f：`mov 0x10(%ebp),%eax`+`mov %eax,-0xc(%ebp)`+`cmpl $0x0,-0xc(%ebp)`
    // ⇒ 包基址先落具名局部再判空（直接比 `data` 会退化为 cmp 参数槽）。
    char* pPkt = data;
    if (pPkt == 0)
        return 0x219;
    APSystem::CUserProc::SetTodayActionAndCheckMedalReward(user);
    user->EnableCharacInfo((CACHE_SECTION)8);
    return 0;
}