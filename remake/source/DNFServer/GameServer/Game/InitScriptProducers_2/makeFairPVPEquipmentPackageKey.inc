int makeFairPVPEquipmentPackageKey(int job, int idx)
{
    // ORIG 0x088945ad：单 int 槽 -0x4，`mov %ax,-0x4`（job 低16）→
    // `mov %ax,-0x2`（idx 高16）→ `mov -0x4,%eax` 整体回读。
    // 两次 16-bit store 到同一 32-bit 槽 ⇒ P180 union 视图逐半字写（移位/or 形
    // 被 4.4 生 movzwl+shl+or，int 局部赋 ushort 会走整 dword store 均不可）。
    union { int u; unsigned short v[2]; } key;
    key.v[0] = (unsigned short)job;
    key.v[1] = (unsigned short)idx;
    return key.u;
