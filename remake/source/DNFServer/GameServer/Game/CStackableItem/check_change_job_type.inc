// 【N90】差值主体=libstdc++4.1 iterator/find 非 inline sret 外联（ORIG 8 insns/步 vs 4.4 inline 4）；豁免不逐形追。
// P182/N82 封顶（b26 Lane27）：29 条指令全等，仅帧 0x28 vs 0x10（4.4 outgoing/
// slack 多 24B）连带 2 char 槽平移（-0xc/-0x10 vs -0x4/-0x8）；帧策略跨版本差。
// b28c-L8 fresh 复验：44 行口径 29=29、NDIFF=4，判词维持 N82（callee
// _ZNK5CItem14check_job_typeEc 定义在 CItem.cpp 0x0850d728 = 跨 TU ⇒ P210 定义前置不可用；
// P190 double framePad 反向撑帧亦不可（本例是 4.4 多撑，需减帧））。
// [b28d-N115/N82] fresh 29=29 维持 N：帧 0x28 vs 0x10 + 2 char 参数槽平移（-0xc/-0x10
// vs -0x4/-0x8）。根因 = N115：4.1 参数直写帧底（outgoing 仅实参 4B），4.4 -O0 恒预留
// 16B 对齐 outgoing 区（帧 +0x18）。P210 定义前置不可用（callee CItem::check_job_type
// 定义在 CItem.cpp = 跨 TU，迁定义违 R1）；P190 double 撑帧方向反（本例需减帧）。
// [S1Charac 2026-09-11 复核] 29=29 逐条同形（含 ORIG 0x0850ebba `mov %edx,0x4(%esp)` /
// 0x0850ebc7 `xor $0x1,%eax` / `sete %al`）；真残差仅帧 0x28↔0x10 与 2 个 char 参数槽
// （-0xc/-0x10 ↔ -0x4/-0x8）=N115（4.4 -O0 恒预留 16B 对齐 outgoing 区）。无串引用。
bool CStackableItem::check_change_job_type(char changeJob,
                                           char checkJob) const
{
    if (!check_job_type(checkJob))
    {
        return 0;
    }
    if ((char)m_arad148.m_field4 < 0)
    {
        return 1;
    }
    return (char)m_arad148.m_field4 == changeJob;
}