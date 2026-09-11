// ORIG execute 0x084aa2d2：按名查物品并回发查询结果包，找到则再 GM 发放一件。
// 【b28d-2026-09-10 重判】旧 N78/N82 判词作废（gap 59→0，助记符 306/306）：
// ① info 真实尺寸 = 0xff（ORIG 慢路 `mov $0xff,%ebx` + 槽 -0x157..-0x59 双证），
//    旧 `char info[0x100]` 多 1 字节（真 bug）；
// ② ORIG 两分支各一次 `memset(info,0,0xff)` 被 4.1 内联为对齐剥离慢路
//    （0x084aa30c/0x084aa48c 各 39 条）⇒ 源码写成 `info = InfoBuf();`（POD 整体赋值，
//    4.4 -O0 发同形慢路，探针 /tmp/b28d_gmd/probe2.cpp fB4/fB5），落点非 4 对齐槽；
// ③ 嵌套 if 形（Ghidra C 同构）复现末项 `cmp mem,%eax; setl %al; test; je`；
// ④ P227 全表达式临时链令 getCurCharacName→%esi、NumberToString→%ebx 跨 ctor 保活；
// ⑤ P223 负条件 goto 复现 `js <nop>; body; jmp join; nop; join:` 尾岛。
// 残余：AddItem 实参装载段 ORIG 提前 `mov -0x28(%ebp),%eax` 并以 %edx 中转 &space/count，
// 4.4 统一用 %eax（1 条 mov 位置差，P75/N7 跨版本寄存器分配，封顶）。
// p167：49=49 CLEAR（b28c-R26 复验；MISSING C2/D2 与 EXTRA C1/D1 为同址别名伪差，
//   P93，ORIG 0x0858dd4c/0x0858de80 同址）。
// [round5-GmCmds_D 复核] 306=306、p167 49=49（MISSING/EXTRA 仍仅 C1/C2、D1/D2
//   同址别名）⇒ 无缺口；残余 = AddItem 实参装载段 1 条 `mov -0x28(%ebp),%eax`
//   的位置差（P75/N7 跨版本寄存器分配）。新钥匙逐条核过：P339/P331 要求「恒定
//   常数额外平移或槽映射互换」，本件槽图同形无平移 ⇒ 不适用；P226/P231/P248 在
//   纯槽装载实参上实测无效（bm3/p13 与 CCreateDisjointStoreCmd 同结论）⇒ 封顶。
void CItemInfoCmd::execute()
