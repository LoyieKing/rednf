// ORIG 0x0813f72a T dispatch_sig —— 祭坛角色数据 DB 加载写入管理器。
// 报告 docs/class_func_reports/advancealtar__Inter_Load/dispatch_sig.md 全文转写。
int advancealtar::Inter_Load::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user == 0 || user->get_state() == 0 || user->getCurCharacR() == 0)
        return 0x2de;  // ORIG 0x0813f764 `mov $0x2de,%eax`
    // ORIG 0x0813f76b-0813f78e：包基址落 -0xc 判空后 `mov 0xc,%eax; lea 0x8df60(%eax),%edx;
    // mov -0xc(%ebp),%eax` —— this-lea 先于实参装载需实参为同型结构指针局部解引用
    // （probe lea1 f7；const 版本 set(const T&) 亦兼容）。
    const _CharacAdvanceAltarDbData* pPkt =
        reinterpret_cast<const _CharacAdvanceAltarDbData*>(data);
    if (pPkt == 0)
        return 0x2e3;
    user->m_advanceAltar.set(*pPkt);
    user->EnableCharacInfo((CACHE_SECTION)10);
    return 0;
}