// ORIG 0x08124d2a T dispatch_sig —— 行动点 DB 加载写入 CActionPointManager。
// 报告 docs/class_func_reports/APSystem__Inter_LoadActionPoint/dispatch_sig.md 全文转写。
int APSystem::Inter_LoadActionPoint::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user == 0 || user->get_state() == 0 || user->getCurCharacR() == 0)
        return 0x203;
    // ORIG 0x08124d6e：包基址落 -0xc 具名槽后判空；08124d7e-87 段 `mov 形参,%eax;
    // lea 0x8d264(%eax),%edx; mov -0xc(%ebp),%eax` —— this-lea 先于实参装载仅在
    // 实参为「同型结构指针局部解引用 lvalue」时出现（probe lea1 f7；char*+cast
    // 实参会反序且退化 add）。
    _SIG_LOAD_ACTION_POINT* pPkt = reinterpret_cast<_SIG_LOAD_ACTION_POINT*>(data);
    if (pPkt == 0)
        return 0x208;
    user->m_actionPoint.Set(*pPkt);
    user->EnableCharacInfo((CACHE_SECTION)8);
    return 0;
}