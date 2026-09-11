// ORIG T 0x08637d64 (_ZN21TimerFinishItemEffect36_processSepecialEffectContinuousItemEP5CUserj)
// ⚠ 私有方法，TimerDispatcher.h 该类内需补声明
//   `void _processSepecialEffectContinuousItem(CUser*, unsigned int);`（NEEDS_LOCAL_STATIC）
// [b28d-30] ORIG `mov 0x10(%ebp),%eax; cmp $0x1cd1,%eax; jne` 载入+寄存器比较：
// 原码是单 case switch（P208/P251 应用）。探针 /tmp/probe30_b.cpp f6 实证
// 4.4 对 `switch(a){case K:}` 逐条复现该形；if 形恒折 mem 直比（旧 N18b 判词作废）。
void TimerFinishItemEffect::_processSepecialEffectContinuousItem(CUser* user,
