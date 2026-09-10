int Inter_LoadOnlinePreliminaryMemberMatchList::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    // [b26N7] 尾部虚调用 callee 寄存器 ORIG vfn→%ecx vs 4.4 恒 %eax（N7 探针
    // 实证）；ndiff 12 封顶。
    if (user->get_state() <= 2)
        return 0;
    if (user->getCurCharacR() == 0)
        return 0;
    reinterpret_cast<charac_expand::CData*>(
        user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)5))->loadData(user, data);
    return 0;
