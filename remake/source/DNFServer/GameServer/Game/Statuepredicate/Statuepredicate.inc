// [b22判词·N族] ORIG C1 0x8a6c186 在置 3 成员前先 call
// _ZNSt14unary_functionI22pieceQuestRewardStatusbEC1Ev（0x8a6c180，trivial 空基类
// ctor）并以 sub $0x18 预留其调用帧；我方 GCC 视 std::unary_function 为空基类、
// ctor 平凡 → 省略该 call 与对应帧（P0：函数体无额外局部）。三成员 mov 与 ORIG
// 完全同址同序（+0x0/+0x4/+0x8）。空基类 ctor 无副作用，语义等价，不可复现
// （需以非平凡基类才能强制发 call，会改类语义）。
// [b28c-27] 复核同上：4.4 对 std::unary_function（空基类 trivial ctor）恒省 call
// 与对应帧，ORIG 4.1 O0 发射；不可翻（同 P215 边界）。
// [b28d-lane15] 复核维持 N：ORIG 15 insn / 我方 13（TSV ndiff=11）；唯一残差即
// unary_function::C1 call（ORIG 0x08a6c192）与随之的 sub $0x18 帧（4.4 恒省）。
// 三成员 mov 与 ORIG 同址同序（+0x0/+0x4/+0x8）。封顶。
// [A24TailBatch2 2026-09-10 四次复核·N 维持] 二次 nm 双侧核对：
//   ORIG `08a6c180 00000005 W _ZNSt14unary_functionI22pieceQuestRewardStatusbEC1Ev`
//   （另有同址 C2）；我方 /tmp/tu_Statuepredicate.o **无该符号** ⇒ 4.4 判
//   std::unary_function（libstdc++ 空类、无用户 ctor）trivial，C1 恒省 call + 无帧
//   （P71/P215 边界：要复现需改库头使其非平凡=禁改，或造非平凡基类=改语义）。
//   同族差：ORIG 0x08a6c197 载入 eax 一次并复用供 3 次 store（4.1 的 dest 缓存形），
//   我方每字段重读 0x8(%ebp)（4.4 恒重读）⇒ 该 2 条亦跨编译器寄存器分配差（N18a 族）。
//   语义等价（空基类 ctor 零副作用）。判定：不可翻，封 N。
// [P2-S4 2026-09-11 五次复核·N 维持] 依指派核查 call 目标：ORIG 0x08a6c192 `call
//   0x8a6c180` = `_ZNSt14unary_functionI22pieceQuestRewardStatusbEC1Ev`，该目标体
//   `push %ebp; mov %esp,%ebp; pop %ebp; ret`（0x8a6c180..0x8a6c184，共 5 字节，空函数）。
//   我方 TU 侧 nm 无同名符号（4.4 视空类无用户 ctor 为 trivial，恒省 call 与 `sub $0x18`
//   帧；初始化列表已显式写 `std::unary_function<...>()` 仍被省略）。要发该 call 只能把
//   基类改成非平凡 ⇒ 改类语义（禁）或改 .h（禁）。维持不可达：ORIG 15 / 我方 14。
Statuepredicate::Statuepredicate(pieceQuestRewardStatus status)
    : std::unary_function<pieceQuestRewardStatus, bool>(),
      m_appendageType(status.m_appendageType),
      m_valueType(status.m_valueType),
      m_value(status.m_value)
{
}