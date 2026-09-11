// ORIG 0x084e4ee4 T dispatch_sig —— 击杀怪物统计 DB 加载（扩展数据槽 0xc）。
// 报告 docs/class_func_reports/Inter_LoadKillMonsterInfo/dispatch_sig.md 全文转写。
// [b28c-32] verdict=NEAR：N7 虚调用 callee 寄存器互换（35=35，助记符全等）。
int Inter_LoadKillMonsterInfo::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
        return 0x6248;
    charac_expand::CData* p = reinterpret_cast<charac_expand::CData*>(
        user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xc));
    p->loadData(user, data);
    user->EnableCharacInfo((CACHE_SECTION)9);
    return 0;
}