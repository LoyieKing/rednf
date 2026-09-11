// ORIG 0x084e263e T dispatch_sig —— 线上预选成员数据 DB 加载（扩展数据槽 4）。
// 报告 docs/class_func_reports/Inter_LoadOnlinePreliminaryMember/dispatch_sig.md 全文转写。
int Inter_LoadOnlinePreliminaryMember::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    // [b26N7] 尾部虚调用 callee 寄存器 ORIG vfn→%ecx vs 4.4 恒 %eax（N7 探针
    // 实证）；ndiff 12 封顶。
    if (user->get_state() <= 2)
        return 0;
    if (user->getCurCharacR() == 0)
        return 0;
    reinterpret_cast<charac_expand::CData*>(
        user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)4))->loadData(user, data);
    return 0;
}