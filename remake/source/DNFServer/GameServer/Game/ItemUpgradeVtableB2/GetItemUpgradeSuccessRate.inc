// ============================================================================
// expert_job::CAlchemist::GetItemUpgradeSuccessRate @ 0x085d1360
//   return (float)g_alchemistInfo / *(float*)0x08cbfa3c (= 10000.0f)
// ============================================================================
float expert_job::CAlchemist::GetItemUpgradeSuccessRate()
{
    // ORIG .rodata 0x08cbfa3c 为**字面量** 10000.0f（实字节 00 40 1c 46；无符号）；
    // [R3a15 2026-09-12 真语义修复] 原注释/字面量误写 9840.0f（与
    //   CItemUpgrade_Separate::_GetUpgradeInfo 同一误标源）⇒ 按 ORIG 复刻 10000.0f。
    //   原 `static const float
    // divisor` 是发明符号（_ZZN10expert_job10CAlchemist25GetItemUpgradeSuccessRateEvE7divisor，
    // ORIG 全 ELF 无）⇒ 改回内联字面量，指令形同为 flds。
    return (float)g_alchemistInfo[0] / 10000.0f;   // 合并后 int[2]；低半区 [0] 为原标量
}