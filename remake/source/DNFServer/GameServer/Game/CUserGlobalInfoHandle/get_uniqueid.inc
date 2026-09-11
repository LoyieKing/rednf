// ============================================================================
// CUserGlobalInfoHandle::get_uniqueid —— ORIG 0x086ad910 T size 0x4e
// 报告：docs/class_func_reports/CUserGlobalInfoHandle/get_uniqueid.md（## 汇编 +
// ## 反编译 C 两段已完整阅读；Ghidra C 的 local_8 == 我方 idx）。
// [P225 2026-09-09] ORIG 0x086ad924..2d 为 4.1 的 bool 物化岛
// `cmpl $0xffffffff,-0x4(%ebp); sete %al; test %al,%al; je`；4.4 对裸 `if` 恒折
// `cmpl $-1,-0x4(%ebp); jne` 直跳（少 sete+test 两条，DIFF orig24/our22）。按
// P225（CVillageAttacked/CDropItem 实证）改写 `while (idx == -1) { …; break; }`
// 后 setCC 四段逐条复现；残留 1 条 nop（4.4 循环出口对齐，§6.1/N112 剥离口径）
// ⇒ verdict=AE orig24/our24。
// 历史：[N96 清绑] 已删 `register bool failed __asm__("al")`（P0 限定②/N96 全域禁绑，
// P248 尾条：任何情形不得恢复）。
// 读数：清绑后 if 形 TU [identical=1 ae=4 合计5 near=0 diff=1 missing=2]；本形
//       [identical=1 ae=6 合计7 near=0 diff=1 missing=2]，其中 diff=1 为 N123 撞名
//       `_Z41__static_initialization_and_destruction_0ii`（本 TU 真副本 0x086ad978
//       16 条助记符序列 16=16 全等），missing=2 为 P93 同址别名 C2/D2。
// [N6S2NegB 2026-09-11 复核] ORIG 0x086ad94a `je` 目标前 / 函数尾各有 1 条对齐 nop，
// 我方物理 25 行 vs ORIG 24 ⇒ 归一口径（compare_common.strip_nop_padding）已剥 ⇒
// n1_v verdict=AE orig=24 our=24。消 nop 穷举（/tmp/probe_gid/p4..p12 + 446/444 后端）：
// 裸 if=p5 `cmpl;jne`、具名 bool=p4/p7（多 -0x9 槽 + cmpb）、while+break=p6、while+goto=p11、
// do-while=p12、while(true)+if(!=)break=p8（jne+回跳）——**全部复现该 nop**，无一形产出
// 无 nop 的 `sete/test/je`；该 nop 为 4.4 对物化条件的固有产物，源码不可消。
// 结论：保持 while+break 形（唯一复现 ORIG 0x086ad924..2d 四段的形），本件已到 .o 口径上限。
// ============================================================================
int CUserGlobalInfoHandle::get_uniqueid()
{
    int idx = find_uniqueid();
    // [P225 while+break] ORIG 0x086ad924 `cmpl $0xffffffff,-0x4(%ebp); sete %al;
    // test %al,%al; je` 是 4.1 的 bool 物化后条件分支；4.4 对裸 `if` 恒折
    // `cmpl $-1,-0x4(%ebp); jne` 直跳。按 P225（CVillageAttacked/CDropItem 实证）
    // 改用 `while (cond) { body; break; }` 精确复现 setCC+test+je 四段物化。
    while (idx == -1)
    {
        m_next = 1;
        idx = find_uniqueid();
        break;
    }
    m_next = idx + 1;
    return idx;
}