// ORIG 0x084db200 T dispatch_sig —— 达人职业 DB 加载（扩展数据 loadData + 信息包）。
// 报告 docs/class_func_reports/Inter_LoadExpertJob/dispatch_sig.md 全文转写。
int Inter_LoadExpertJob::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() <= 1)
        return 0;
    if (user->getCurCharacR() == 0)
        return 0;
    // GetCharacExpandData(0) 实际对象为 expert_job::CCharacExpertJob（CData 派生，vptr+4
    // loadData 虚槽，ORIG 0x084db250 mov (%eax),%edx; add $0x4 两参虚调）；第二次独立
    // call（P48，ORIG 0x084db26a），SendExpertJobInfo 为非虚直调（ORIG 0x0832b454）——
    // 结果免槽直用（技巧37），cast 无代码。
    reinterpret_cast<expert_job::CCharacExpertJob*>(
        user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0))->loadData(user, data);
    reinterpret_cast<expert_job::CCharacExpertJob*>(
        user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0))->SendExpertJobInfo(user);
    return 0;
}