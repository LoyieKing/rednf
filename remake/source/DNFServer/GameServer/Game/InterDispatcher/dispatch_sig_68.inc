// ORIG 0x084cf55a T dispatch_sig —— 个人商店加载应答转发给 PrivateStoreMgr。
// 报告 docs/class_func_reports/Inter_LoadPrivateStore/dispatch_sig.md 全文转写。
int Inter_LoadPrivateStore::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() <= 2)
        return 0;
    // ORIG +0x27：包指针先落具名局部（-0xc），call 之后再 `mov -0xc(%ebp),%edx` 填槽
    // （直接把 reinterpret_cast 作实参会让 gcc 提前放进 %ebx 并省掉栈槽）。
    private_store::SIG_PRIVATE_STORE* pPkt =
        reinterpret_cast<private_store::SIG_PRIVATE_STORE*>(data);
    private_store::GetInstancePrivateStoreMgr()->LoadPrivateStoreResponse(user, pPkt);
    return 0;
}