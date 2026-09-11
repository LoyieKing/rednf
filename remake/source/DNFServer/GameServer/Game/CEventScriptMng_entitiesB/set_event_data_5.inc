// ============ CAccountFirstLoginDaily ============
// 派生自 CAccountFirstLogin（group1，FixEventEntitiesA）。
// ORIG ctor 0x0811110a W / D1 0x08114c80 / D0 0x08114cb0。
// 不新增成员，+0x1c 继承自 CAccountFirstLogin。
// ORIG T 0x810dfc6：直接委托基类（CAccountFirstLogin::set_event_data）。
void CAccountFirstLoginDaily::set_event_data(int key, InGameEventScript* script)
{
    CAccountFirstLogin::set_event_data(key, script);
}