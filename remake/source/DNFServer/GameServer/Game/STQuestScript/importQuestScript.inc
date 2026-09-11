// [b28d-solo-STQuestScript · §6.2 三件套] ORIG 0x08a61813（raw 3883 / norm 3836）
// ---------------------------------------------------------------------------
// [2026-09-10 SweepSTQuestScript-2 收口判词] 本函数三项修复（逐项实测）：
// ① [type]/[exception quest] 状态机 27+1 处块序翻转为
//    `if(!__inited){insert}else{dst;break}` —— 与 ORIG 汇编逐条同构：
//    `movzbl __inited;xor $1;test;je <dst>` → insert 块 → `jmp <下一 case>`
//    → dst 块（ORIG 0x8a632b6..0x8a63ea4 / 0x8a65563 段实证）。效果：
//    xor 61→89（ORIG 87）、jmp 283→311（ORIG 315）、raw 3841→3897。
//    旧判词「翻转后 fn_diff 变差（3635→3676）」系在未做 ②③ 的基线上测得；
//    三项联改后 difflib 对齐率 0.7255→0.7544（matched 2803→2932）⇒ 保留。
// ② ConvertHHMMSSStringToInt 6 处改直接传 entry 成员引用（去 h/m/sec
//    具名局部）：ORIG 0x8a6214d/15a/167/1e1/1ee/1fb 为
//    `lea -0x45c; add $0x18/19/1a/1b/1c/1d`，我方原多 6 组
//    `movb $0 + movb %al,entry` ⇒ 修后 raw 3897→3885。
// ③ entry.m_eventId 改经具名局部 eventId（ORIG -0x40 中间槽实证
//    0x8a61ee7/0x8a61efc：`mov %eax,-0x40; …; mov -0x40,%eax; mov %eax,-0x45c`）
//    ⇒ raw 3885→3887（+2 条与 ORIG 对齐）。
// 现状：raw 3883/3887（gap -4）、xor 87/89、jmp 315/311、
// p167 517/517 CLEAR、四数 458/917/0/12 不回退。
// 残余 gap -4 = 4.1/4.4 块布局差（非语义缺口，逐点核对）：
//   · ORIG 序言 +0x84..+0xbf 的 20 组 `nop;jmp loop_head` continue 岛，
//     20 个前驱全部实证在案（0x8a61a5a jne / 0x8a61e16 je / 0x8a623cd…/
//     0x8a6572c je；**旧判词「17 组无前驱=死岛」系误判，已更正**）；
//     我方 4.4 把 71 处 continue 汇入 1 个中继（jmp loop_head）并把 14 处
//     break 生成 `jmp 收尾;nop` 岛链 ⇒ 岛归属/落点不可控（P223/N82 族）。
//   · 13 处 `if(!ok) break;` 我方 `xor;test;jne <break 岛>` + fall-through
//     到 continue；ORIG `xor;test;je <continue 岛>` + `jmp 收尾` —— 两侧
//     极性/落点互补、语义逐点等价（抽查 .o 岛链 60b3..60c5 与
//     ORIG 0x8a64e37 等 7 点）。
//   · 槽漂移（line -0x320↔-0x308 族）与 setne/lea 级分散形态差 ⇒ N82/N126。
// 以下为 2026-09-09 历史判词（gap 42 /「翻转变差」结论已被本次推翻）：
// vs 我方（raw 3841 / norm 3811，gap 25）；p167 调用点差集 517/517 CLEAR
// （唯一差异为 C1↔C2 / D1↔D2 镜像对，豁免）。残余 byte 层 N 封顶：
// ① P223 实测复验（BigQuestScript 2026-09-09）：ORIG +0x86..+0xbf 为 19 组
//    `nop;jmp +0xc0` 中继岛（仅 +0x86/+0x8c/+0x92 有 jne 前驱 +0x247/+0xbba/
//    +0xcf1，其余 16 组无前驱）；我方 4.4 只发 15 组且全部落在函数尾
//    （+0x3f6a 附近 8 连 + 7 处分散）⇒ 岛数/落点不可控，19 条封顶。
// ② [type]/[exception quest] 守卫块序镜像（P272/P194）：ORIG 为
//    `if(!__inited){insert}else{dst;break}`（movzbl;xor $1;test;je dst，
//    insert 后 jmp 下一 case），我方为 `if(__inited){dst;break}insert`
//    （movzbl;test;je insert，insert fall-through）。27+1 处全量翻转实测：
//    ORIG-only 指令 181→36、xor 61→88（ORIG 87）、jmp 283→311（ORIG 315）
//    ⇒ 补回 54 条真实指令，但净 gap +25→-31、fn_diff 不等行 3635→3676
//    （本区固有 mov+22/jne+11/movb+8 冗余未消）⇒ 保持现形，判镜像块序差。
// ③ 其余 6 条为 je/add/lea/sete 级分散形态差（我方 3 处 setcc 分支形、
//    我方多 mov/movb/movzbl 与 ORIG 多 je/add/lea 互为抵消），非语义缺口。
// [b28d-solo-STQuestScript · P292/P315 档①] int→bool（与头同步）：
// ORIG 调用点 0x08353e51 `call; xor $0x1,%eax; test %al,%al; je`（无 setne）。
// 函数体 return 0/1 均为常量 ⇒ bool 化不引入 setne，定义侧形态不变；
// mangled _Z17importQuestScriptP11QuestScriptPKc 不变（返回类型不入 mangled）。
// [b28d-solo-STQuestScript 2026-09-09 复核] 定义侧同快照 A/B（bool vs int）
// 四数逐项相同 460/906/0/23 ⇒ 返回类型改动零形态影响；fresh 读数
// 458/917/0/12（missing 362，全 TU）。gap 42（raw 3883/3841）主体 = ORIG
// 序言 +0x32..+0x71 共 20 组 `nop;jmp 0x8a618d3` 中继岛（仅 +0x86/+0x8c/
// +0x92 三组有 jne 前驱，余 17 组无前驱 = 死岛）vs 我方 1 组（+0x21cb
// jmp +0x21cd nop）⇒ 4.4 jump-thread 合并；死岛无法用合法 C++ 生成
// （P337 volatile 守卫每处固有 +4 条 > 收益，净负）⇒ P223 负例封顶；
// 余 4 条为槽漂移（非恒定：line -0x320↔-0x308、qd0 -0x3d8↔-0x3e0）
// 与 [type] 块序 ⇒ N82/N126 封顶。p167 517/517 CLEAR（差集全 C1/C2·D1/D2
// 同址镜像，P93 豁免），miss=0（无 ORIG-only 游戏域 callee）。
bool importQuestScript(QuestScript* script, const char* path)
