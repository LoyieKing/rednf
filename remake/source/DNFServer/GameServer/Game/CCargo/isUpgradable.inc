// ORIG 0x850c698：同上判定，capacity<=0x98 才可升。
// [b28d-N] 真语义 bug 修正：ORIG 首段 `test %eax,%eax; je +0x18(mov$0)` ⇒
// m_capacity==0 直接 return 0（与 `m_capacity>capacity` 共享 ret0 块）。
// 旧写 `m_capacity != 0 && m_capacity > capacity` 在 capacity∈[1,0x98] 时误返 1；
// 短路形 `==0 || >` 同时复现 ORIG 的 je/fall 共享块（P214①/P59）。
bool CCargo::isUpgradable(int capacity) const   // b28d-Main：与 CUserCharacInfo.h:266 同步（P292 bool callee）
{
    if (m_capacity == 0 || m_capacity > capacity)
        return 0;
    if (m_capacity >= capacity) return 0;
    if (capacity > 0x98) return 0;
    return 1;
}