// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 EventUpgradeJar/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— Event_Upgrade_Jar / EventUpgradeJarInfo。
// 逐函数依据 ORIG objdump/Ghidra 反汇编转写（见 docs/class_func_reports/Event_Upgrade_Jar/）。
// 布局与字段见 EventUpgradeJarInfo.h。
// ============================================================================

#include "EventUpgradeJarInfo.h"

#include <algorithm>

#include "DNFLexWrapper.h"   // loadRDARScriptFile/ScanType/ScanInt/ScanFloat（真实符号）

// ---- ST_Event_Upgrade_Jar_EventNPCs_Script 已内联 ctor（头文件）。----

// ============================================================================
// EventUpgradeJarInfo 生命周期
// ============================================================================

// ORIG 0x080f49b8。仅转发到 EventUpgradeJarInfo::EventUpgradeJarInfo。
#include "EventUpgradeJar/Event_Upgrade_Jar.inc"
}

// vector<ST_...>(+0x74)。标量成员（m_npcCount/m_discountRate/m_upgradeMax/
// m_materialKind/m_materialIdx）ORIG 一律不初始化（工作状态运行时才写）。
#include "EventUpgradeJar/EventUpgradeJarInfo.inc"
}

// ORIG 0x080f51d2（W）。dtor 依构造逆序析构全部 STL 成员（vector/map 自动释放）。
#include "EventUpgradeJar/dtor_EventUpgradeJarInfo.inc"
}

// ============================================================================
// Event_Upgrade_Jar 生命周期
// ============================================================================

// ORIG 0x080f49cc。仅转发到 EventUpgradeJarInfo::~EventUpgradeJarInfo。
#include "EventUpgradeJar/dtor_Event_Upgrade_Jar.inc"
}

// ============================================================================
// Event_Upgrade_Jar 方法
// ============================================================================

// ORIG 0x080f49e0。转发到 importEventUpgradeJarScript((EventUpgradeJarInfo*)this, path)。
// 第一个参数 scriptDir 未使用（ORIG 仅用第二个）。
// ORIG 返回 int（call 后 test %al,%al;je;mov$1;jmp;mov$0 的无 xor 物化，P9）：
// 直接 return bool 生 movzbl %al; 需三元 ?1:0 强制分支化转换。
#include "EventUpgradeJar/LoadScript.inc"
}

// ORIG 0x080f4bb0。条件：item[+0x02]==0x28be7d 且 item[+0x07]>=1。
#include "EventUpgradeJar/check_requiredRecipe.inc"
}

 // ORIG 0x080f4be0。Fortune 分支：在 m_materialIndices 中查 materialIdx，命中则在对应的
// m_failRateByFortune[i] 段中按 upgrade 取概率；越界/未命中返回 100000。
#include "EventUpgradeJar/_getFailProb_Fortune.inc"
}

// ORIG 0x080f4c8e。NewNPC 分支：按 m_npcScripts[idx].m_failRate 取值；越界返回 100000。
#include "EventUpgradeJar/_getFailProb_NewNPC.inc"
}

// ORIG 0x080f4ccc。NewNPC 分支：按 m_npcScripts[idx].m_meterialCount 取值；越界返回 1000000。
#include "EventUpgradeJar/_getMeterialCount_NewNPC.inc"
}

// ORIG 0x080f4d0c。读 m_discountRate（+0x70）。
#include "EventUpgradeJar/_getDiscountRate_NewNPC.inc"
}

// ORIG 0x080f4d20。设 m_materialKind：index==0x28be7b → 2（newNPC）；在 m_materialIndices
// 中命中 → 1（fortune）；否则 0。m_materialIdx 清 0。
#include "EventUpgradeJar/CheckMaterialIndex.inc"
}

// ORIG 0x080f4dac。按 m_materialKind 填充 upgrade_info_t：
//   kind==1（fortune）：info.m_meterialCount=1，m_recipeItem 在 m_materialIndices 中查
//     materialIdx 命中则取对应索引值；kind==2（newNPC）：m_meterialCount=打折后材料数、
//     m_recipeItem=0x28be7b、m_discountBase=round(discountRate*原值)。末写 m_materialIdx=upgrade。
#include "EventUpgradeJar/SettingRecipeNUpConunt.inc"
}

// ORIG 0x080f4ee2。按 m_materialKind 取失败概率写入 outProb：kind==1 → _getFailProb_Fortune，
// kind==2 → _getFailProb_NewNPC。kind==0 或其它不改 outProb。
#include "EventUpgradeJar/GetProbability.inc"
}

// ORIG 0x080f4f62。条件：m_materialIndices.size()>1 且 [1]==upgrade 且 item.GetUpgrade()>=m_upgradeMax
// → 返回 false；否则 true。
#include "EventUpgradeJar/CheckMaxLuckyLevel.inc"
}

// ORIG 0x080f4fd0。m_materialIndices.size()>=2 时返回 [1]，否则 0。
#include "EventUpgradeJar/GetIndexLuckyStrengtheningDeed.inc"
}

// ORIG 0x080f4a0a。在 m_boosterIndexMap 查 cardIdx/boosterIdx，两键都存在时返回对应
// m_recipeMap[(min,max)] 的值（min/max 为两键对应的 short 值）；否则 0。
#include "EventUpgradeJar/getBoosterIndex_byUpgradeCardIndex.inc"
}

// ORIG 0x0854b57c（W）。m_materialKind==1。
#include "EventUpgradeJar/isFortune.inc"
}

// ORIG 0x0854b590（W）。m_materialKind==2。
#include "EventUpgradeJar/isNewNPC.inc"
}

// ============================================================================
// 自由函数
// ============================================================================

// ORIG 0x089687d1。解析 EventUpgradeJar.etc 脚本，填充 EventUpgradeJarInfo 字段。
// 段：`[group info]`（npcCount + 索引 map + 配方 map）、`[fortune index]`（材料索引）、
// `[fortune fail rate]`（分段失败率）、`[event upgrade table]`（折扣率 + NPC 脚本）、
// `[upgrade max]`。任一 Scan 失败返回 false（0）。
// [b28d-solo-EventUpgradeJar] N 封顶：ORIG 405 insn / 我方 404 insn（round5：
// 402→404），p167 调用点 43/43 CLEAR（--demangle），残余为跨版本结构性差异：
// [round5 两项归位] ①槽位：ORIG 全帧槽号比原版我方**统一深 4**（token -0x6c vs
// -0x68、ok -0x71 vs -0x6d、id -0x78 vs -0x74、npc -0x80 vs -0x7c）而两侧 sub
// 同为 $0x90 ⇒ 差在槽基（N95）。在函数头补 4B 死局部 `int slotBasePad;`
// （P232：4.4-OO 对未使用局部照样占栈）后深区逐槽与 ORIG 全等；同法在标量区
// 补第二个 pad **反效**（深区被推深 4 而 recipeIndex 不动）已回退 ⇒ 标量区
// （recipeIndex -0x28 vs ORIG -0x2c）属 P212/N98 不可移。②[fortune fail rate]
// 原 `while ((groupIdx = ScanInt(&ok), ok == true))` 产 `test; jne`（无 xor）⇒
// 改 `for(;;){ groupIdx = ScanInt(&ok); if (!ok) break; ... }` 复现 ORIG 的
// `movzbl ok; xor $0x1; test %al,%al; je`（P168a②/P292 档①），402→404。
// 残余 1 条 + 4 个 ORIG `nop` 中继岛（P223 空 then 占位）：
// ① 4.4 对本函数**大 body**
// 的段分支反转布局（`jne <body>; jmp 循环头` vs ORIG `je <下一段>; body; jmp 循环头`；
// 小 body 探针 /tmp/eu_probe9.cpp f/g/h 三形均产 ORIG 形 ⇒ 属 4.4 reorder-blocks
// 启发式，非源形可控；__builtin_expect 实测反劣 404→407）；② 槽位分配
// （ORIG cardIdx=-0x28/boosterIdx=-0x24/pairCount=-0x20/i=-0x1c vs 4.4 反向，
// N82/N98 族）。已修项：id==-1 只 break 内层 while（真 bug）、string 声明序、
// id/recipeIndex 头部初始化、ScanType EOF 尾岛（P164）、(n+1)*n 书写序（P243 姊妹）。
#include "EventUpgradeJar/importEventUpgradeJarScript.inc"
}

// ORIG 0x08968d59。把 EventUpgradeJarInfo 的 NPC 脚本填进 upgrade_table_t 的 m_infos
// （m_recipeItem=0x28be7b、m_meterialCount=npc.m_meterialCount），并对 m_infos[0..99]
// 的折扣基数应用 m_discountRate。任一指针为空返回 0。
#include "EventUpgradeJar/updateUpgradeTableFromEventUpgradeJarScript.inc"
}
