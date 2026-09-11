// ============================================================================
// importDimensionActivationData —— ORIG T 0x89160f0
// ORIG 结构（/tmp/importDimAct.md 汇编+Ghidra C 双段实证）：
// 外层节名 = 函数级静态 __lookup map<char const*,int,less_TCHAR> +
// __init_lock(SpinLock) + __inited 双检贯穿 insert 建表链（5 条：
// "[play day count]"0x3488→"[coin info]"0x348d→"[section]"0x34b7→
// "[select group]"0x34ca→"[give group]"0x34ff），建表后 find 派发；
// coin info 难度 = 第二张静态 __lookup_0（4 条：[easy]0x3499→0、
// [medium]0x349e→1、[hard]0x34a3→2、[ultimate]0x34a8→3、未命中保持
// 入口 -1 且**照常**执行公共尾 insert），两表键均 less_TCHAR 大小写无关。
// 数据落点：[play day count]→out+0x00（ScanInt 返回值直写，不查 flag）；
// coinMap(+0x04) insert(coinType, DimensionCoinInfo{+0=首 ScanInt int、
// +4=二 ScanInt!=0 的 setne 字节})；[section] 装配 EventRewardSection 入
// m_rewardData.m_vec0(+0x1c)；[select group] 按 makeFairPVPEquipmentPackageKey
// (getCharacterJob,idx) 入 m_mapC(+0x28)；[give group] 按 (job,growIdx) 入
// m_map24(+0x40)。节内 ScanStr 失败 → return false；EOF(ScanType 失败) → true。
// ============================================================================
// b28d-21 复验 — ORIG 0x089160f0（0x0890c1f0 段为同 TU 邻居）。p167 CLEAR（175=175）。
// 本轮修真语义 bug + 归位形态，指令数 1143 vs 1122 → 1143 vs 1128（norm 1127 vs 1128）：
// ① **[section] 循环条件方向反**（真 bug）：ORIG 089169da `movzbl ok; xor $1; test;
//    je 记录块` ⇒ 原码 `if (!ok) break;`；旧写 `if (ok) break;` 在成功读到记录时反而
//    停止读 m_field1/m_field2/count/items。
// ② 9 处静态表守卫改负极性 `if (!__inited) { insert } else { 派发 }`（ORIG 089163b5/
//    0891642a/08916958/08916ac7/08916fa6/089166e9/08916755/089167c1/0891682a 均
//    `movzbl; xor $1; test; je 派发`）⇒ 各补回 1 条 xor + 1 条块尾 jmp。
// ③ 两张表的 guard 循环头改正极性 `if (__inited) { find } else { 锁 }`（ORIG 08916272/
//    089165b1 `movzbl; test; je 锁`，无 xor）。
// ④ 两处公共尾改 `if (!__inited) { leave; inited=1; continue; } break;`（ORIG 08916891/
//    0891735b `movzbl; xor $1; test; je …`）。
// ⑤ `int v = 0;` 单变量跨节复用（ORIG Ghidra local_1ac@-0x1a8：prologue 清零 +
//    coin-info used + section count + give-group value），旧拆 4 个局部致全帧槽号漂移。
// ⑥ 两串声明移到 load 检查之后（ORIG 08916130/0891613e ctor 在 load 后）；EOF 改
//    `break` + 循环外 `return true`（P164 尾部共享块）；`int first;` 去预清；
//    give-group 死存补具名 `dead20`（ORIG 0891710f `mov %eax,-0x1c`）。
// 残差 1 条（我方多）：块序差异（items 构造/tokenizeStr 段位置）+ 槽号整体深 4
//（N95/N126 版本首槽基址常数）+ 1 条尾 jmp ⇒ N82/N95 封顶。
// [SweepInitProd9 本轮] fresh 读数：origN 1143 / ourN 1141（gap +2）；jcc 155/156、
// 目标块差异 9 段（delete/insert 成对，均为静态表守卫块与 [section]/[give group]
// 内层循环的 nop 中继岛落点差，jcc 方向无互换）⇒ 维持 N82/N95，无真反。
// [SweepInitProd9 本轮·回边方向审计] ORIG backward 16 / 我方 14：差的 2 条 = 4.4
// 把 4 条 continue（leave 点 0x494d/0x4c8b/0x4e99/0x5960）合并到 2 个共享尾块
// （0x5974→jmp 4773=+0x8e 主表 guard 头；0x4ea6→jmp 4ab1=+0x3cc 子表 guard 头），
// ORIG 为 4 条独立 jmp 直达同目标 ⇒ **无回边丢失、无「退出循环」误写**。
// [C03CryptoInit68 复核·2026-09-10] p167 双向差集 175=175；MISSING 全为 D2/C2 与
// 我方 D1/C1 的**同址别名伪差**（`_ZNSsD2Ev`/`_ZNSsC2Ev`、`_ZN19EventRewardItemInfoD2Ev`、
// `_ZNSaIcED2Ev`、`_ZN18EventRewardSectionD2Ev`、`_ZNSt3map…C2Ev`、`SpinLockC2Ev`、
// `_ZNSt4pairIKPKciEC2I…` 等），EXTRA 侧为其 C1/D1 对应物 ⇒ 无真语义缺口（P93）。
bool importDimensionActivationData(const char* path,
