// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 STQuestScript/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— G5 任务/城镇脚本域（STQuestScript.cpp）
// 对照 docs/class_func_reports/QuestScript.md 与 ORIG 反汇编：
//   QuestScript        ctor 0x0836e4e2 / copy 0x0837adc8 / dtor 0x0836eaf0
//                      clear 0x0836deba / destroy 0x08a6130a
//                      getName 0x08a611fc / getRequestMessage 0x08a61240
//                      getConditionMessage 0x08a61284 / getSolveMessage 0x08a612c6
//   TownScript         clear 0x086d1e2e / ctor 0x086d1f04 / dtor 0x086d2ab8
//   TrainingQuestScript ctor 0x08371cc8 / clear 0x08371dd4 / dtor 0x0837d488
//                      computeDayIndex 0x08aafc64 / get_random_fake_value 0x08aafc90
//                      suffleTrainingQuests 0x08aafcbc / getTrainingQuest 0x08aafb44
//                      getApplyLevel 0x08aaf8b6
// 解析辅助（loadRDARScriptFile/ScanType/ScanInt/loadRDARScriptListFile）经
// DNFLexWrapper.h 权威声明调用真实符号。
// ============================================================================

#include "STQuestScript.h"

#include <algorithm>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "Quest.h"
#include "MyRandom.h"          // suffleTrainingQuests 的 random_shuffle 随机数器

#include "DNFLexWrapper.h"
#include "CEnvironment.h"      // class SpinLock / less_TCHAR（importQuestScript [type] 静态表）
#include "SyncScriptDeps.h"    // checkValiedJobName（importQuestScript [target character]）

// loadRDARScriptListFile 定义于 STScriptFileList.cpp（ORIG T 0x088bbd77，
// mangled _Z22loadRDARScriptListFileP16STScriptFileListPKcS2_PSt3mapIiiSt4lessIiESaISt4pairIKiiEEE）。
// STScriptFileList 无权威头，此处仅前置声明 + 正常 C++ 自由函数声明（非 asm 桥）。
// STScriptFileList 权威定义见 NpcScript.h（经 Quest.h include 链可见）。
// 本 TU 按 bool 视图声明（P22：ORIG init* 尾 @0x8a616e9 xor $1,%eax;test %al
// 为 bool 取反形；返回类型不入 mangled，链接 STScriptFileList.cpp 的 int 定义，
// 其返回恒 0/1，ABI 兼容——StrikerScript.cpp 同例先）。
#include "STQuestScript/loadRDARScriptListFile.inc"
#include "STQuestScript/IsLightServer.inc"
#include "STQuestScript/OutputErrorMessage.inc"
#include "STQuestScript/ScanRewardItems.inc"
#include "STQuestScript/ConvertStringToTime.inc"
#include "STQuestScript/ConvertWeekdayStringToInt.inc"
#include "STQuestScript/ConvertHHMMSSStringToInt.inc"
#include "STQuestScript/ScanJobStr.inc"
extern STScriptFileList g_townScriptFileList;  // 定义于 GameWorld.cpp（ORIG B 0x9500ea0）
// g_questScriptFileList 权威定义见 InitScriptProducers_4.cpp（ORIG B 0x9500480）。
extern STScriptFileList g_questScriptFileList;

// ===================== QuestScript =====================

// [b28d-solo-STQuestScript · P69] ORIG C1 0x0836e4e2（487 条）：40+ 成员子对象隐式
// 构造（含 EH cleanup 链）后**单一 `call QuestScript::clear()`**（836e738），ctor 体
// 无任何 memset ⇒ 原码 `QuestScript::QuestScript() { clear(); }`。
#include "STQuestScript/QuestScript.inc"
}

// [b28d-Lane SymSrvParam · N84 处置③] ORIG _ZN11QuestScriptC1ERKS_ 0x0837adc8
// 为 W 隐式合成（0xd65）；CListTables.cpp 镜像类仅有拷贝 ctor **声明**（U 引用，
// 防 0x364/0x30c ODR 竞争，Main b28d 决定），全仓唯一 W 发射点必须在本 TU。原
// forceQuestScriptCopyEmit（t 符号，ORIG 无）改为数据锚：文件作用域 static 对象
// 定义（b 符号，非函数、不导出于链接命名空间）以拷贝初始化 ODR-use 合成拷贝
// ctor ⇒ 4.4 在本 TU COMDAT 发射同名 W；构造/析构并入既有
// __static_initialization_and_destruction_0 / _GLOBAL__I_*（t，ORIG 同名并存）。
// 已核 ORIG 无该类型全局对象：`nm -S $O | grep ' [BbDd] '` 无 size 0x30c
// （QuestScript）命中（同名前缀仅 g_questScriptFileList 0x18 /
// g_questScriptBaseDirectory 0x100）⇒ 本锚唯一作用是保留 W 发射点，
// 非物化任何 ORIG 数据。
// 语义中性：clear() 纯成员置零、destroy() 对 0 指针安全，无外部可观察副作用。
static QuestScript g_questScriptCopyEmitSrc;
static QuestScript g_questScriptCopyEmitUse(g_questScriptCopyEmitSrc);



// ORIG D1 0x08a70700：先 destroy()（delete 四组 11 指针），再成员自动析构。
#include "STQuestScript/dtor_QuestScript.inc"
}


#include "STQuestScript/clear.inc"
}

#include "STQuestScript/destroy.inc"
}

#include "STQuestScript/getName.inc"
}

#include "STQuestScript/getRequestMessage.inc"
}

#include "STQuestScript/getConditionMessage.inc"
}

#include "STQuestScript/getSolveMessage.inc"
}

// ===================== TownScript =====================

#include "STQuestScript/TownScript.inc"
}

#include "STQuestScript/dtor_TownScript.inc"
}

#include "STQuestScript/clear_1.inc"
}

// ===================== TrainingQuestScript =====================

#include "STQuestScript/ST_TrainingQuest_Select_Prob.inc"
}

#include "STQuestScript/dtor_ST_TrainingQuest_Select_Prob.inc"
}

#include "STQuestScript/TrainingQuestScript.inc"
}

#include "STQuestScript/dtor_TrainingQuestScript.inc"
}

#include "STQuestScript/clear_2.inc"
}

#include "STQuestScript/reset_same_apply_level_cnt.inc"
}

#include "STQuestScript/computeDayIndex.inc"
}

#include "STQuestScript/get_random_fake_value.inc"
}

#include "STQuestScript/suffleTrainingQuests.inc"
}

#include "STQuestScript/getTrainingQuest.inc"
}

// [b28d-solo-STQuestScript · P212] ORIG 先构造 tmp（-0x38）再 call computeDayIndex
// ⇒ 头部声明序 tmp 在前（4.1/4.4 全头部声明同为先声明更深）；ORIG 0x08aaf8b6。
#include "STQuestScript/getApplyLevel.inc"
}

// ===================== 脚本导入 =====================

// [b28d-solo-STQuestScript · P303] ORIG 追加分隔符段 = __builtin_strcat 内建展开
// （movl $2,0x8; movl $str,0x4; movl $buf,(%esp); call strlen; add $buf,%eax;
//  mov %eax,(%esp); mov (%esp),%eax; mov %eax,(%esp); call memcpy，ORIG 0x8a6166a 族）
#include "STQuestScript/initQuestScript.inc"
}

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
#include "STQuestScript/importQuestScript.inc"
}
// [b28d-solo-STQuestScript · P303] 同 initQuestScript：追加分隔符段 =
// __builtin_strcat 内建展开（ORIG 0x8aad9ec memcpy 段 8 条形）。
#include "STQuestScript/initTownScript.inc"
}

// [b28d-solo-STQuestScript · §6.2 三件套] ORIG 0x08aadaa8（382 条）vs 我方 384
// （gap −2）；p167 调用点差集 57/57 CLEAR（0/0，含 P304 权威型 &g_townScriptFileList
// 直压、TownAreaScript 循环外构造 + 块内 clear、ScanInt(NULL)、bool 直存 setne、
// if/else 正极性守卫、Main 代落 T_Rect 去用户析构）。残余 byte 层 N 封顶：
// ① N104/EH 类：我方 TownAreaScript+两 string 的 cleanup 链相对正常路径的块序与
//    ORIG 相反（内容逐条等价，仅尾块互换）；② P223 单源尾岛合并（`je` 落点）；
// ③ 少量 `mov %eax,A(%esp)` vs `(%esp)` 寻址形态。
#include "STQuestScript/importTownScript.inc"
}

// [b28d-solo-STQuestScript · P0/P244/P171] ORIG 0x08aaf554（257 条）逐条还原：
// ①具名指针副本 s（ORIG `mov 0x8(%ebp),%eax; mov %eax,-0x1c; mov -0x1c,%eax`
//   槽往返 ⇒ 原码有具名指针局部，P0）；②pair 循环外默认构造 + 成员赋值；
// ③各段尾 `sub` 与闭合标签比较（ORIG 中 sub 全函数无写入点 = 原码遗留恒假
//   死条件，P171 照抄）；④ScanType 失败/[training quest end] 走统一 `return ok`。
// [§6.2 三件套] 2026-09-09 复核：257 vs 253（gap 4）；p167 30/30 CLEAR。
// 逐条归一化 diff 实测 4 处残差全为跳转中继岛/对齐 nop（无语义）：
// ① +0x5e..+0x63 ORIG `jmp;nop;jmp;nop` 双中继岛（[level] else 的 continue
//    落 +0x5b4、[training quest end] 不等落 +0x5b7）vs 我方单 `jmp`（差 2）；
// ② +0x71e nop（`je` 后 `jmp` 前）与 +0x802 nop 两处 4.1 对齐填充（差 2）；
// ③ +0x680/+0x685 两条同目标 jmp（5b8）4.4 合并为一条（差 1），我方
//    +0x2cb/+0x2ee 侧多一条 jmp（+1）⇒ 净 -4。
// 4.4 对同址目标恒 jump-thread（P223 负例），nop 属 §6.1 对齐产物 ⇒ N 封顶。
// 尾部 `movzbl -0x25(%ebp),%ebx` 已与 ORIG 单段形一致（旧 N18a 残差已消）。
// [2026-09-10 SweepSTQuestScript-2] [level] 分支补 2 处显式 continue
// （ORIG 0x8aaf7d6 / 0x8aaf802 两条 `jmp loop_head`）⇒ raw 253→254；
// 残余 gap 3 = ORIG 循环头前 2 组 `jmp loop_head;nop`（0x8aaf5b2 入口 +
// 0x8aaf5b5，后者是 0x8aaf7df `jne` 的落点）被 4.4 jump-thread 合并为 1 组，
// 0x8aaf685 中继同被合并 ⇒ P223 族封顶（无合法 C++ 源形可分裂）。
// 四数 458/917/0/12 不回退；p167 无 ORIG-only 游戏域 callee。
#include "STQuestScript/importTrainingQuestScript.inc"
}

// ORIG B 0x09500380：脚本根目录（EpisodeScript 等 loadRDARScriptFile 首参）。
char g_questScriptBaseDirectory[0x100];

// ORIG T 0x8a60f78（0x95）：0x14 字节标量区清零（+0x10 为字节）+ m_str20 构造
// + m_field24 清零 + 11 个 dialog 指针清零（i=0..10，P151）。
#include "STQuestScript/dungeonEventHanaseStruct.inc"
}

// ORIG T 0x8a6100e：逐字段拷贝 + 深拷贝 11 个 dialog 串
// （new std::string + operator=，非拷贝构造；ORIG asm _Znwj→SsC1→SsaSERKSs 实证）。
#include "STQuestScript/dungeonEventHanaseStruct_1.inc"
}

// ORIG T 0x8a615fe（0x44）：m_field24!=0 且 0<=job<=10 且 m_dialogs[job]!=0 →
// 返回 m_dialogs[job]；否则返回 &m_str20（+0x20）。
#include "STQuestScript/getNewDialog.inc"
}
