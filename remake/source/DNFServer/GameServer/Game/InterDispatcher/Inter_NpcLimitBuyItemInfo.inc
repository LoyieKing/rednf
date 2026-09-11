// ======== Inter_NpcLimitBuyItemInfo（B1：生命周期/reset；业务方法待类型域补齐） ========
// [b22判词·N-EH] Inter_NpcLimitBuyItemInfo::C1Ev（ORIG 0x084e6502，报告汇编+Ghidra C 全读）
// 主体逐指令同构 ORIG：ISigDispatcher::C1 → 置 vptr(&vtable+0x8) → pair<int,int>@+8::C1
// → Inven_Item@+0x1d::C1 → Inven_Item@+0x5a::C1 → this->reset()。orig25/our40 全差在
// 4.4.7 对 4 个可能抛的 sub-ctor 生成 EH cleanup 块（push %esi/%ebx + `jmp <landing>` +
// ~C1 pair + _Unwind_Resume，15 条），ORIG 编译器同位置无任何 landing pad。函数体不可
// 复现（编译器版本决策），语义等价。
// [b28c-32] verdict=AE：P216 reset() throw() 消 ~ISigDispatcher cleanup（40→25=ORIG）。
Inter_NpcLimitBuyItemInfo::Inter_NpcLimitBuyItemInfo()
{
    reset();
}