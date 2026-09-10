Dispatcher_GrowthCreatureChangeInfinityCreature::~Dispatcher_GrowthCreatureChangeInfinityCreature() {}
// 经 GiveGrowCreatureEvent vtable+0x34 槽（IsEventing，ORIG vtable 0x08b4bae0 实证）
// 判事件开启；失败路径返回 1。
// b28c-L20: 第一段 GetRepeatEvent 右值内联（P0 无槽）；正极性 `if (IsEventing(0))`（ORIG
//   0x081e872b 无 xor，`test %al; je <mov $1 岛>`）；第二段具名 ev（-0xc 槽，cmpl/je）。
//   残余 4 行 = 尾调实参装载 user 寄存器 %eax(ORIG)↔%edx(4.4) 对调（N82附：多实参 call
//   首值 callee-saved 对调；限定调用/具名指针/裸调用七形穷举无效）。
