// ORIG _ZN13user_creature12CCreatureMgr18DeleteCreatureItemEii T 0x0833b606。
// 仅委托 UnregisterCreatureItem(param_2)（param_1 调用方传 item->m_addInfo2，本函数未使用）。
// 返回 void 的 ORIG 反编译，但 eax 继承 UnregisterCreatureItem 的返回值，
// 故以 bool 返回其值（匹配 SendMail 调用处的分支判断）。
bool CCreatureMgr::DeleteCreatureItem(int param_1, int param_2)
{
    return UnregisterCreatureItem(param_1);   // ORIG 读 param_1@+0xc（我们误用 param_2@+0x10）
}
