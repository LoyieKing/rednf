// ORIG 0x850c656：m_capacity==0 或 m_capacity>capacity 拒绝；仅 m_capacity<capacity
// 升级（==拒绝）。ORIG 汇编 test/je→mov$0 + Ghidra C 双证，旧注释语义相反。
bool CCargo::UpgradeCargo(int capacity)   // b28d-Main：与 CUserCharacInfo.h:263 声明同步（P292 bool callee）
{
    // [b28c-16] P222 注（两形互试）/同族 isUpgradable 实证：ORIG 第二段
    // `cmp; jl +0x31(升级块在后); fall mov $0; jmp end` ⇒ 源形必须是负极
    // `>= → return 0` + 升级体落尾块；正向 `if (<) {…} return 0` 4.4 反产
    // jge + 升级体 fall-through。
    if (m_capacity == 0 || m_capacity > capacity)
        return 0;
    if (m_capacity >= capacity)
        return 0;
    m_capacity = capacity;
    return 1;
}