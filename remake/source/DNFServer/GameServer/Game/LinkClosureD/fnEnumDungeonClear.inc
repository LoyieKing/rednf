// ---------------------------------------------------------------------------
// ORIG T 0x08678166 sz 0x4e（报告全文已读）：ctx 首槽为 InterfacePacketBuf*，
// put_short(dungeonIdx) → put_byte(diff) → 返回 1。
// 消费点：CUser::SendDungeonClearList 经 enumDungeonClear 回调（CUser.h 已修正拼写）。
// ---------------------------------------------------------------------------
// [b28d-solo-LinkClosureD] DIFF→N 封顶 | N7+P75（ORIG 0x08678166）：4.4 对
// `userData += 4` 恒「解引用在前 + addl mem 在后」，4.1 为 lea 前置写回+旧值
// 解引用（后缀 *p++ 发射形）；引用形 `InterfacePacketBuf**& p` 引入引用槽
// 24→29 insn 反劣；movswl/movsbl 目标 %eax vs ORIG %edx（N7，本 TU 未路由
// 4.4.6，P230 限定）。语义等价（先取旧值再按 4 字节前移）。
int fnEnumDungeonClear(short dungeonIdx, char diff, char* userData)
// [b28c-L23 真语义 bug 修正 + N7/P75 残余] ①已修游标步长：ORIG @0x0867817c
// `lea 0x4(%eax),%edx; mov %edx,0x10(%ebp)` 表明回调游标按**指针槽 4 字节**前移，
// 原码 `userData++`（char* 步长 1）使第 2 条起读错偏移。②残余 9 行 = 4.4 对
// `p += 4` 恒发单条 `addl $4,mem`（4.1 为 lea+mov 两条，探针 /tmp/l23/p2.cpp
// gA/gB/gD 四形穷举不可造）+ 两处实参装载寄存器差（ORIG `movswl→%edx`、
// 4.4 `movswl→%eax`，N7）。语义与 ORIG 一致。
{
    // ORIG：lea 0x4→写回形参槽 + 旧值解引用（后缀 ++ 语义，4.1 发射写回）。
    // b28c-l23 真语义 bug 修正：ORIG @0x0867817c 为 `lea 0x4(%eax),%edx;
    // mov %edx,0x10(%ebp)` ⇒ 游标按**指针槽 4 字节**前移（枚举多条目时每条占
    // 一个 4 字节槽）；原写法 `userData++`（char* 步长 1）使第 2 条起读错偏移。
    // 探针 /tmp/l23/p2.cpp gA/gB/gE：4.4 恒"解引用在前 + addl 在后"（无法产出
    // 4.1 的 lea 前置 + 双写回），余 1 条为跨版本指令选择差（P75 族）。
    // ORIG 0x0867817c 为后缀 `*p++` 发射形（lea 0x4 前置写回 + 旧值解引用）。
    // 4.4 无槽别名不可得：引用形 `InterfacePacketBuf**& p` 引入引用槽
    //（lea 0x10(%ebp) → -0x10，+5 insn 反劣 24→29，b28d-S120 实测）；
    // 具名 `p` 形写回的是局部槽非形参槽且 +1 槽。本形语义与 ORIG 全等
    //（先取旧值再按 4 字节前移），残余 = P75 指令调度 + N7 寄存器角色。
    InterfacePacketBuf* buf = *reinterpret_cast<InterfacePacketBuf**>(userData);
    userData += 4;
    buf->put_short(dungeonIdx);
    buf->put_byte(diff);
    return 1;
}