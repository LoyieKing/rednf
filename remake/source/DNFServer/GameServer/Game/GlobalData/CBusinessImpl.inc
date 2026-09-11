// P210 附注（b28 BL14 翻案，Main 2026-09-08 广播）：派生 ctor 定义前置到同
// TU 基类 ctor 之前 ⇒ 4.4 对该 callee 不可见、恢复 ORIG 保守 outgoing
// `sub $0x18`（0x082a7c3b），旧「帧舍入 N + dead pad」判词作废（pad 删）。
CBusinessImpl::CBusinessImpl()
{
    // ORIG 0x082a7c32：call IBusiness ctor + 设 vptr（编译器生成序列）。
}