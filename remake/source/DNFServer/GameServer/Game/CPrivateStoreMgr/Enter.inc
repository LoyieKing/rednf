// ORIG T 0x85caa8a（报告 Enter.md 逐指令）：委托 LoadPrivateStore(user)，恒返 1。
int CPrivateStoreMgr::Enter(CUser* user)
{
    LoadPrivateStore(user);
    return 1;
}